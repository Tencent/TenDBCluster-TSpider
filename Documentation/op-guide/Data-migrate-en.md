# Data Migrate
This chapter mainly introduces how to migrate data from MySQL to TenDB Cluster, and how to migrate data from TenDB Cluster to MySQL.

<a id="jump1"></a>

## Migrate Data From MySQL to TenDB Cluster

When MySQL is not enough to carry more workload, we often meet the application needs by data/table sharding, optimizing application SQL, upgrading hardware, etc. But these methods are not only complicated to operate, but also do not fundamentally solve the problem, but only temporarily relieve the pressure.
Therefore, we can evaluate and consider moving the application to a TenDB Cluster with horizontal scalability to solve the problem fundamentally.

### Feasibility Check

Before migrating to a TenDB Cluster, you need to perform certain checks on MySQL data to ensure that the conditions for accessing the cluster are met, such as the validity of table construction constraints, whether there are a large number of cross-shard operations, and a large number of distributed transactions.    
For specific cluster constraints, please refer to [Similarities and Differences with MySQL] (../re-book/mysql-compatibility.md).

In addition, we also need to consider the application's usage characteristics, such as whether to use the name service, load balancing, connection reconnection mechanism, etc.  If the requirement is to migrate online to the cluster without affecting the service, these characteristics need to be carefully evaluated to prevent  to short-term service unavailability during online cluster switchover. 

### Create a TenDB Cluster Cluster
Refer to the chapter `Cluster Installation`

### Import Table Schema
Use mysqldump to backup the table structure on the MySQL, and import the backed up table structure to the entire cluster through the TSpider node.

### Import Data

<a id="jump2"></a>

Use mysqldump to backup data on the MySQL (ignoring the table structure), then import the data into the entire cluster through TSpider.

It is generally recommended to add a high-configuration temporary TSpider node to the cluster. The disk space, CPU, and memory are relatively close to the stand-alone MySQL, which can effectively improve the imported data performance.

### Switching

When the data import is complete, you need to consider switching application services from MySQL to TenDB Cluster. According to the application requirements, we can use online switching or offline switching

#### Online Switching

Online switching refers to switch services from the MySQL to TenDB Cluster without affecting application requests.   
The prerequisite for online switching is that the incremental data of the MySQL has been written to the TenDB Cluster in real time, so as to ensure the consistency of the data before and after the switching.

- Configure Master-slave Replication 
> After the data is imported completely, establish the master-slave relationship between the TSpider node and the original MySQL instance.  The configuration of using TSpider as a slave node is consistent with the usage of MySQL.

- Start Master-slave Synchronization  
> Do "start slave" on the TSpider node to start synchronization.

- Online Switching  

> Do `show slave status` on the TSpider node to check the synchronization status and ensure that there is no synchronization backward or synchronization error.   
Do `flush tables with read lock` to block the write request of the entire business on the MySQL node, and then modify the name service to allow the application configuration to resolve the TSpider node of the TenDB Cluster. After the configuration modification takes effect, you can see The new request has been accessed to the TSpider.

> If the application does not use the name service but requests directly through the ip connection, the ip information of the application configuration needs to be modified to the TSpider information of the TenDB Cluster. And the reconnection mechanism of the application needs to be considered to prevent the service from being affected after the switch.
If the application has a long time-consuming write operation, `flush tables with read lock` may cause the lock time to be long or fail to affect the service. In this case, execute` unlock tables` to release the lock in time and resume the application request. Then attempting to switch after the long write operation were completed or terminated.

> After the switching is successful, the old stand-alone MySQL is best to shut down the service in time to prevent unknown factors such as connection cache from still requesting the old service and abnormal data.

#### Offline Switching
Offline switching refers to switch the service from stand-alone MySQL to TenDB Cluster solution after the application stops writing requests.
Compared with online switching, the process of offline switching is much simpler. According to the different tolerance of applications to stop service time, it is divided into cold switching and hot switching.
- Cold switch
> Cold switch refers to shutting down the write request of the stand-alone MySQL first, and then starting the backup, and then importing the full amount of backup data into the  TenDB Cluster through the TSpider node. After the import is completed, the application connection information (or name service) is modified and the service is started.
- Hot switch
> The process of hot switch is almost the same as that of online switching, and the data is synchronized by configuring the master-slave switching. The only difference is that during the switching, the switching is not completed by the global read lock, but the application's write request is directly stopped and modified. Application connection configuration. After the data synchronization of TSpider is completed, after the configuration takes effect, start the application write service.

### Other Instructions
Whether switching online or offline, be sure to ensure data consistency before switching. Before switching, check the permissions of TSpider and apply the configuration to prevent abnormal service due to abnormal permissions and configuration after switching. 
For online switching, the switching time will directly affect the time when the application cannot be written. In addition to the long time-consuming operation mentioned above, which may affect the locking time, the effective time of the configuration file, load balancing, name service, etc. And the application of reconnection mechanisms will affect the switching time. You need to fully evaluate these times for the entire switching.

## Migrating from TenDB Cluster to the MySQL

<a id="jump3"> </a>

In some scenarios, we have the need to migrate data from TenDB Cluster to the MySQL instance.
From TenDB Cluster to the MySQL, we complete it through data backup, import, and switching.

### Data Backup

Unlike the data storage of the MySQL, data in TenDB Cluster is distributed to each TenDB storage node according to a certain sharding rule, and the table schema has also undergone some changes

#### Backup Databasee and Table Schema

In TenDB Cluster, the table schema exists on both TSpider and TenDB nodes, but there are certain differences. For specific change rules, please refer to [Creating Table Description] (../re-book/ddl-syntax-en.md/#jump1).

After understanding the creation rules of the cluster database and table, we can choose to back up at any TSpider node or TenDB node when backing up the schema. After the backup, the reverse operation based on the original rules can be modified to modify the changes introduced by the cluster.

Considering that the TenDB node has the same table structure as the single machine except for the database name change, we chose to backup and modify the schema on the TenDB node.

After using mysqldump to back up the table schema, the schema at this time does not match our usage requirements, and we also need to modify the database name. For example, the stand-alone database name is `test`, then on __shard0__, the database name will be rewritten to` test_0` by Tdbctl; on __shard1__, the database name will be rewritten into `test_1`. If we choose __shard0__ backup, we can remove the matched ___0__ by simple regular replacement and get the correct database name `test`.

The rules for generating database names in clusters will not change after the cluster is established and the number of TenDB shards (mysql.servers) is determined. Therefore, it is reliable that we can obtain the correct database name based on the sharding rules.

In actual use, we can extend mysqldump, or integrate a set of cluster backup and recovery tools based on mysqldump to simplify our daily operations

#### Backup Data

Since the data is distributed on each TenDB storage node, we need to only export data from the TSpider node, the points to be considered when exporting data are roughly the same as [import data](#jump2)

#### Service Switching

When switching from a cluster to MySQL, in addition to the previously mentioned considerations for name services, application configuration, and reconnection mechanisms, we also need to ensure data consistency.

At present, our recommended solution is to stop service, backup and import data after stopping application access.

If stand-alone MySQL supports multi-source replication and other features, consider using stand-alone MySQL as a slave, each TenDB as a master to synchronize data incremental data, and finally add a read lock at the access layer to switch online.

