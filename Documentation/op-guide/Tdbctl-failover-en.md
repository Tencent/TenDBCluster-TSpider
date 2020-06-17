# Tdbctl node high availability
The high availability of the Tdbctl node  is relatively simple. At present, we adopt the GROUP REPLICATION program recommended by MySQL, with 3 nodes and a master-slave MGR mode.

## Fault discovery
Unlike TSpider and TenDB nodes that discover faults through peripheral detection, Tdbctl's fault discovery relies more on the fault detection mechanism of the MGR scheme itself to determine faults. 
Specific details can refer to the document [MySQL GROUP REPLICATION](https://dev.mysql.com/doc/refman/5.7/en/group-replication-network-partitioning.html)

## Failover
After detecting a fault, MGR will remove the faulty node from the replication group and promote a slave node to be the master node through election to ensure service.
Information such as the status of specific group members can be use MGR [group-replication-monitoring](https://dev.mysql.com/doc/refman/5.7/en/group-replication-monitoring.html).

Since our mysql.servers table records all routing information of MGR nodes, in addition to the fault removal function of the MGR itself, we also need to remove the faulty node from the mysql.servers table, otherwise it will affect the DDL request of the cluster.
```sql
delete from mysql.servers where Host='$fault_tdbctl_ip' and Port='$fault_tdbctl_port';
```

## Fault Recovery

When the failed node is recovery, or a new node is added (full data has been imported), we can refer to [MGR configuration](manual-install.md/#mgr-cnf) ,to configure the new Tdbctl node to the replication group.
- Modify my.cnf to enable MGR related configuration
- Configure permission
- Enable MGR
```sql
START GROUP_REPLICATION;
```

After the MGR synchronization is normal, connect to the master node of Tdbctl, modify the mysql.servers table, and insert the new node information.

## Other Instructions
We take 26000 instance failure as an example to demonstrate the failover and failure recovery of Tdbctl

### Failover
Shutdown 26000 instance,  simulate member 26000 failure.

- Connect to 26001 instance, check MGR switching status
```sql
#Check the current node and find that 26000 instances have been removed from the group members
mysql>  select * from performance_schema.replication_group_members ;
+---------------------------+--------------------------------------+-------------+-------------+--------------+
| CHANNEL_NAME              | MEMBER_ID                            | MEMBER_HOST | MEMBER_PORT | MEMBER_STATE |
+---------------------------+--------------------------------------+-------------+-------------+--------------+
| group_replication_applier | ae2d0e0e-68c9-11ea-b716-6c0b84d5c2e5 | 127.0.0.1   |       26001 | ONLINE       |
| group_replication_applier | d34f771c-68c9-11ea-89e1-6c0b84d5c2e5 | 127.0.0.1   |       26002 | ONLINE       |
+---------------------------+--------------------------------------+-------------+-------------+--------------+
#Check the node status and find that 26001 has been promoted to the master node and changed from read-only to writable
mysql> SELECT VARIABLE_VALUE  FROM performance_schema.global_status  WHERE VARIABLE_NAME = 'group_replication_primary_member';
+--------------------------------------+
| VARIABLE_VALUE                       |
+--------------------------------------+
| ae2d0e0e-68c9-11ea-b716-6c0b84d5c2e5 |
+--------------------------------------+
```

- Check TenDB Cluster status
```sql
#Connect to TSpider Port:25000
mysql -umysql -pmysql -h127.0.0.1 -P25000
mysql> create database test_fail;
ERROR 4150 (HY000): Error happened before execute ddl in tdbctl when connect to tdbctl,  -1684530208
```
>As shown in the figure above, the cluster DDL is abnormal at this time 

- Connect to the Tdbctl 26001 instance and remove the 26000 fault node
```sql
mysql -umysql -pmysql -h127.0.0.1 -P26001
#TDBCTL0 corresponding node is 26000
delete from mysql.servers where Server_name='TDBCTL0';
tdbctl flush routing;
```
- Check TenDB Cluster status
```sql
#连接TSpider 25000端口
mysql -umysql -pmysql -h127.0.0.1 -P25000
mysql> create database test_ok;
Query OK, 0 rows affected (0.00 sec)
```
>It can be seen that after removing the faulty Tdbctl node from the route, the cluster DDL operation returns to normal 

### Fault Recovery
Restart Tdbctl 26000 node 

- Add 26000 node in MGR
```sql
#connect to 26000
mysql -umysql -pmysql -h127.0.0.1 -P26000
#MGR has been configured before , so no need to configure again, just enable group replication
start group_replication;

#Connect 26001 to view member status
mysql -umysql -pmysql -h127.0.0.1 -P26001
mysql> select * from performance_schema.replication_group_members ;
+---------------------------+--------------------------------------+-------------+-------------+--------------+
| CHANNEL_NAME              | MEMBER_ID                            | MEMBER_HOST | MEMBER_PORT | MEMBER_STATE |
+---------------------------+--------------------------------------+-------------+-------------+--------------+
| group_replication_applier | ae2d0e0e-68c9-11ea-b716-6c0b84d5c2e5 | 127.0.0.1   |       26001 | ONLINE       |
| group_replication_applier | b2a68fb5-5fbf-11ea-9c1f-6c0b84d5c2e5 | 127.0.0.1   |       26000 | ONLINE       |
| group_replication_applier | d34f771c-68c9-11ea-89e1-6c0b84d5c2e5 | 127.0.0.1   |       26002 | ONLINE       |
+---------------------------+--------------------------------------+-------------+-------------+--------------+
```
>Group members joined successfully

- Add new node in routing table
```sql
mysql -umysql -pmysql -h127.0.0.1 -P26001
mysql> insert into mysql.servers values('TDBCTL0','127.0.0.1','','mysql','mysql',26000,'','TDBCTL','');
Query OK, 1 row affected (0.00 sec)

mysql> tdbctl flush routing;
Query OK, 0 rows affected (0.00 sec)
```

- Check Cluster status
```sql
mysql -umysql -pmysql -h127.0.0.1 -P25000
mysql> create database test_ok;
Query OK, 0 rows affected (0.00 sec)

mysql> drop database test_ok;
Query OK, 0 rows affected (0.01 sec)
```
>The cluster is operating normally