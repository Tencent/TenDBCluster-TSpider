# TenDB storage layer high availability
High availability of the TenDB storage layer, including rapid fault detection and fast switching after fault

## Fault discovery
The storage layer fault detection method is basically the same as TSpider, see  [TSpider-failover](TSpider-failover.md#/jump1)

## Failover

### Pre-switching requirements
When deploying a cluster, each TenDB on the backend requires multiple copies to be deployed. Depending on the actual resources, one master, one slave, one master, multiple slaves, and multiple masters, such as MGR, can be deployed. In this way, when the master node is abnormal, it can quickly switch to the data-consistent slave node, effectively reducing the failure time

### Choose Slave
After the abnormality of the master node is detected, the service needs to be switched from the master node to the slave library in time.
When choosing a slave node, whether it is a master one slave, or a master multi-slave architecture, the slave node we choose must meet the following requirements:

- Data Consistency

```
The data from the slave node needs to be consistent with the master node.
At present, the generally recommended method is to use the data verification tool to generate the data verification SQL in the master database (the checksum value calculation will be performed on the current data). The data is also calculated for the checksum value, and the checksum value of the master and slave is compared to determine whether there is a data inconsistency.
MySQL's own checksum syntax can also be used for data verification, but in actual verification, it is necessary to consider the impact on existing services, verification time-consuming and so on.
In the consistency check, the 100% data accuracy cannot be satisfied, and more reliability checks are performed on the data results that have been verified before the failure. Of course, in some strict scenarios, we can also rely on MGR, semi-synchronization and other solutions to ensure data consistency during synchronization
```

- Master-Slave Delay

```
In scenarios such as asynchronous replication, due to network, non-parallel replication and other reasons, there may be a delay in the slave node. We also need to find this problem when switchover.
IO thread delay: Under logical replication, we know that MySQL synchronizes data through IO thread pulling binlog and replay. If the IO thread gets stuck for some unpredictable reasons, just check the synchronization delay through the show slave status command it is inaccurate. At this time, the results of the command SHOW SLAVE STATUS can only verify that the SQL thread does not fall behind in execution (all binlogs pulled locally are executed).  We can consider periodically executing a SQL statement with the current time on the master node, relying on the synchronization mechanism to detect the execution of this statement on the slave node to determine whether the IO thread is behind.

SQL thread delay: In the normal synchronization scenario, it is normal to have a certain delay in the SQL thread. When the master node fails, the service is already affected at this time, and no new data is written. Therefore, the slave node can continue execute the binlog that has been pulled locally and follow up.  Therefore, when judging backward, we can appropriately adopt the principle of loop waiting and detection until the slave follow-up is completed; of course, considering the real-time nature of the switch, we can adjust the loop wait time threshold to decide whether to meet the switch, sometimes the system does not abnormal alarms are thrown in time when switching is satisfied, which is better than waiting for a long time for decision
```

### Failover
Before switching, ensure that the selected permissions on the slave node are normal (otherwise, the request will fail due to abnormal permissions after switching). For specific permissions, please refer to [TenDB-GRANT](manual-install-en.md/#jump3)

- Change Cluster Router
```sql
update mysql.servers set Host='$slave_ip', Port='$slave_port' where Host='$master_ip' and Port='$master_port';
```

- Flush Router
```sql
#Flush router by force
tdbctl flush routing force;
```
In the fault scenario, some abnormal tcp connections may sometimes affect the real-time performance of the switch before being destroyed normally. Therefore, Tdbctl provides a forced refresh route function, which can effectively avoid the interference of these abnormal connections.

### Fault reconstruction
After the failover is successful, TenDB may be a single point at this time, and we need to restore the new master-slave environment in time to allow normal operation.

