# TSpider Access Layer Scale
Access layer expansion and reduction refers to the way to increase or decrease the TSpider node to achieve the purpose of horizontal expansion and reduction of the entire cluster.
After the TSpider accesses the application request, it will rewrite the SQL through data routing rules and distribute it to the corresponding storage node TenDB for execution, and then process the return result of TenDB and finally return it to the application layer.
TSpider itself does not store data, and it is basically stateless (Some configuration of each TSpider node needs to be different). It can be expanded horizontally without considering the bottleneck of the back-end TenDB node. 
The application layer can access multiple peer TSpider nodes through the unified access address provided by load balancing components (such as LVS, L5, DNS)
When the request load on the application side is low, the cluster access layer can be reduced by reducing the TSpider nodes online, and certain cluster resources can be saved while ensuring stable application requests.
When the request load on the application side is high, you can increase the cluster access layer by adding TSpider nodes online, so that the cluster can quickly respond to more application requests and provide higher real-time throughput.
After adding or removing nodes, it is generally expected that the application side can quickly detect node changes without any configuration changes. The currently recommended method is to configure name services or load balancing components for the access layer nodes, and the application side requests through domain names and other methods. 
When the number of nodes changes, only the resolution result of the name service needs to be changed, so that the application side is not aware. For example, when using a domain name to connect to the cluster, since the domain name cannot distinguish the port numbers of different TSpider nodes, if you consider using a domain name, try to make the TSpider nodes distributed on different hosts use the same port number service before deployment to reduce system complexity degree.


## Scale Up

<a id="jump1"></a>


Suppose there is a TenDB Cluster with the current topology:

| Node type | 	Host|Port|Server_name|
| :--- | :----|:----|:----|
|TSpider|127.0.0.1|25000|SPIDER0|
|TSpider|127.0.0.1|25001|SPIDER1|
|Tdbctl|127.0.0.1|26000|TDBCTL|
|TenDB|127.0.0.1|20000|SPT0|
|TenDB|127.0.0.1|20001|SPT1|
|TenDB|127.0.0.1|20002|SPT2|
|TenDB|127.0.0.1|20003|SPT3|

### Deploy new TSpider node  

Refer to the manual deployment chapter[install-TSpider](manual-install.md/#jump2), deploy a TSpider instance with port number 25002 locally.

### Import table schema
Back up the table schema from any TSpider node (backup the mysql.servers table should be priority during backup) and import to this node.

__Attention__
- Before importing the schema to the new TSpider node, you need ```set ddl_execute_by_ctl = off```, to ensure that the imported schema will not be distributed by the Tdbctl node, and the TSpider node will not report errors.
- The mysql.users table must be backup to ensure consistency of permissions on the application side.
- You must import the schema after importing the mysql.users table, otherwise the import will report an error (because the servers table is empty, the spider engine is abnormal).


### Grant
Log in to Tdbctl and TenDB node to authorize the new TSpider node.
For the username and password, refer to the Username and Password fields in the mysql.servers table. You can also use a new username and password.
The following uses a new username and password as examples.

```sql
create user 'myql1'@'127.0.0.1' identified by 'mysql1';
grant all privileges on 'myql1'@'127.0.0.1';
```

### Refresh router table  
Connect to the Tdbctl node, insert new node information, and synchronize it to the entire cluster.
> mysql -umysql -pmysql -h127.0.0.1 -P26000
```sql
insert into mysql.servers values('SPIDER2','127.0.0.1','','mysql','mysql',25002,'','SPIDER','');
tdbctl flush routing;
```

### Change domain name
If the access layer accesses the domain name system, you need to modify the domain name configuration to access the new node ip.
In this example, since we are all using local deployment, the host name is 127.0.0.1, so skip this step.
If it is a different host, the port number of each access layer is also different. It is not possible to simply modify the domain name, and also consider the difference of the ports.

### New Topology After Scale Up 
| Node type | 	Host|Port|Server_name|
| :--- | :----|:----|:----|
|TSpider|127.0.0.1|25000|SPIDER0|
|TSpider|127.0.0.1|25001|SPIDER1|
|__TSpider__|__127.0.0.1__|__25002__|SPIDER2|
|Tdbctl|127.0.0.1|26000|TDBCTL|
|TenDB|127.0.0.1|20000|SPT0|
|TenDB|127.0.0.1|20001|SPT1|
|TenDB|127.0.0.1|20002|SPT2|
|TenDB|127.0.0.1|20003|SPT3|

## Scale Down
Suppose there is a TenDB Cluster with the current topology:

| Node type | 	Host|Port|Server_name|
| :--- | :----|:----|:----|
|TSpider|127.0.0.1|25000|SPIDER0|
|TSpider|127.0.0.1|25001|SPIDER1|
|Tdbctl|127.0.0.1|26000|TDBCTL|
|TenDB|127.0.0.1|20000|SPT0|
|TenDB|127.0.0.1|20001|SPT1|
|TenDB|127.0.0.1|20002|SPT2|
|TenDB|127.0.0.1|20003|SPT3|
The target node that needs to be scaled down is TSpider # 25001

### Change domain name 
If the access layer is configured with a domain name, before shrinking, remove the IP  from the domain name to ensure that new requests are not routed to the TSpider node.

### Refresh router table  
Connect to the Tdbctl node, delete node information, and synchronize it to the entire cluster.

> mysql -umysql -pmysql -h127.0.0.1 -P26000  
```sql
delete from mysql.servers where Server_name='SPIDER1';
#refresh router
tdbctl flush routing;
```

### Close Node
Shut down the MySQL service of the downsized TSpider node.

### New Topology After Scale Down

| Node type | 	Host|Port|Server_name|
| :--- | :----|:----|:----|
|TSpider|127.0.0.1|25000|SPIDER0|
|__TSpider__|__127.0.0.1__|__25001__|__SPIDER1Delete__|
|Tdbctl|127.0.0.1|26000|TDBCTL|
|TenDB|127.0.0.1|20000|SPT0|
|TenDB|127.0.0.1|20001|SPT1|
|TenDB|127.0.0.1|20002|SPT2|
|TenDB|127.0.0.1|20003|SPT3|

## Other instructions
In application scenarios with many long connections, capacity expansion cannot solve the cluster load problem immediately. If the application reconnection mechanism is relatively complete, you can perform a kill operation on the TSpider node with a higher load. Through the kill part of the connection, the new connection is gradually balanced to the newly added TSpider node.
```sql
show processlist;
#kill connection with specified id
kill thread_id;
#kill all connection,This is the instruction of TSpider extension
kill threads all
```

After the cluster is downsized, if the downsized TSpider node does not shut down the service in time, in the case of a long connection scenario or domain name cache, there may still be a connection request to the old TSpider node, and this TSpider node has been removed from the cluster routing table After cleaning, Tdbctl will no longer synchronize DDL operations, so the application side may use the old table structure to operate the request, causing unknown errors. Therefore, the reduced TSpider node is best to close the service in time to reclaim resources.
