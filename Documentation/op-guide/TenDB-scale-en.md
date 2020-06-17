# TenDB storage layer expansion and contraction
Storage layer expansion and reduction refers to the replacement of storage node hardware configuration, host, etc. So that single machine TenDB can provide higher and faster connection requests, thereby improving the throughput of the entire cluster and achieving the purpose of vertical expansion and reduction of the entire cluster.
When the request load on the application side is low and the storage node resources are idle, you can use the lower configuration slave machine to synchronize the master data by doing a slave to the idle TenDB storage node. After the master and slave data synchronization is completed, after the consistency check pass, the data routing is switched from the high-configuration Master node to the low-configuration Slave node by modifying the routing table online, thereby reducing storage resources and achieving capacity reduction.
When the request load on the application side is high and the pressure on the storage node is high, you can use the higher configuration slave machine to synchronize the master data by using the slave mode for the TenDB node, and modify the routing table online to reduce the data routing The configured Master node is switched to the highly configured Slave node, so that the TenDB storage node can provide higher throughput, thereby improving the overall cluster performance.
Regardless of whether it is downsizing or expansion operation, currently only vertical expansion can be supported for the time being, by adjusting the storage configuration of TenDB to meet online needs.

## TenDB Scale Up
Suppose that there is a TenDB Cluster with the current topology. It is found that the host4 machine has a high load when running, which affects the performance of the entire cluster. The host9 machine requires higher hardware resources. The goal is to replace host4 in the route with host9.

| Node type | 	Host|host|Server_name|
| :--- | :----|:----|:----|
|TSpider|host1|25000|SPIDER0|
|TSpider|host2|25001|SPIDER1|
|Tdbctl|host3|26000|TDBCTL|
|__TenDB__|__host4__|__20000__|__SPT0WaitingScaleUp__|
|TenDB|host5|20001|SPT1|
|TenDB|host6|20002|SPT2|
|TenDB|hosts|20003|SPT3|

### Deploy TenDB node
Reference:[install-TenDB](manual-install.md/#jump1)Deploy a new TenDB node on the host9 machine. My.cnf configuration can refer to the SPT0 instance of host4, but it needs to be adjusted according to the current hardware configuration like ```innodb_buffer_pool_size```and other parameters to meet higher performance, specific adjustments can refer to[MySQL-manual](https://dev.mysql.com/doc/refman/5.7/en/innodb-parameters.html#sysvar_innodb_buffer_pool_size).


### Establish master-slave synchronization

 - Use msyqldump to backup the instance of host4 and save consistent backup points `binlog_file_xxx`, `binlog_file_pos_xxx`. 
 - Import backup to host9 instance.
 - copy permissions from host4 to host9.

 ```sql
 #Syntax example
 grant SELECT, RELOAD, PROCESS, SHOW DATABASES, REPLICATION CLIENT  on *.* to  '$USER'@'host9' IDENTIFIED BY '$PASS'; 
 ```
 -  Establish data synchronization
 ```sql
 change master to master_user='$USER', master_password='$PASS', master_host='host9', master_port='$PORT', master_binlog_file='binlog_file_xxx', master_binlog_pos='binlog_file_pos_xxx';
 #Start slave
 start slave;
 #Show slave status
 show slave status\G
 ```

- Data checksum 
```
After the master-slave synchronization  is completed (the general judgment is based on show slave status to see if the seconds_behind_master value is 0), you can use some data verification tools to check whether the master-slave data is consistent to ensure data availability.
```
Reference: percona [pt-table-checksum](https://www.percona.com/doc/percona-toolkit/3.0/pt-table-checksum.html)


### Permission Config
```
Need to ensure that all TSpider nodes in the cluster and Tdbctl nodes have all privileges to access host9.
Because the new host9 has imported all the permissions of host4 when importing the backup, it does not need to be authorized separately, you can use it as a check item in strict scenarios.
```
### Online Switch
> Before switching, it is necessary to check whether each TSpider node has a long transaction, or query which takes a long time. This type of SQL may cause a long switching time or even a failure. The cluster switchover instruction will take a read lock to ensure the data consistency  between new data node and old data node.

- Change Tdbctl node router info

```sql
#access Tdbctl node 
mysql -umysql -pmysql -hhost3 -P26000
#change router info 
update mysql.servers set Host='host9' where Server_name='SPT0'; 
#flush routing. Tdbctl's extended SQL syntax
tdbctl flush routing;
```

### Switch verification
> Connect to the host9 instance to see if any request which source is the TSpider node of this cluster.

### Switch Clean
```sql
#Execute stop slave on host9
stop slave;
reset slave all;
```
> shutdown MySQL instance on host4 

## TenDB Downsizing
The capacity reduction process of the storage layer is basically the same as the capacity expansion. The only difference is that the performance configuration requirements of the Slave host are lower than that of the Master, so we will not explain it here.
Through capacity reduction, the purpose of resource saving can be achieved, so that the entire cluster can provide services more reasonably.

## Other Instructions
When the storage layer is expanded and reduced, the most important step is the modification of routing ```tdbctl flush routing```. First, the SQL will  modify the mysql.servers table of each TSpider node (only modified, at this time the new routing rule does not take effect on the TSpider node), and then add a global read lock to block new write requests to prevent the master and slave at the same time when the route changes data inconsistency occurs due to read and write; after the lock is completed, a flush operation is performed on each TSpider node to make the new route take effect. At this time, the new connection on the TSpider side will use the new routing rules to request the back-end storage TenDB; finally release the global read lock to complete the switch.