# Routing Management
TenDB Cluster's data distribution, reading and writing are based on sharding rules, so it is very important to
maintain routing in scenarios such as cluster deployment, data management and cluster failover.
At present, TSpider and Tdbctl nodes have a routing table. The routing tables of TSpider nodes are synchronized by Tdbctl,
and ensure global consistency. 

## Routing Table 
At present, routing information is stored in mysql.server.
Below is the description of table's field


| Field_name |Description
| :--- | :----|
|Server_name|Sharding name，valid value:TDBCTL0\~N, SPIDER0\~N, SPT0\~N|
|Db|Db name to sharding, default to NULL
|Username|Username to access sharding
|Password|Password to access sharding
|Port|Port to access sharding
|Socket|Socket to access sharding，N/A
|Wrapper|Sharding Type，valid value: TDBCTL，mysql，SPIDER, SPIDER_SLAVE
|Owner|N/A

- SPIDER  
>TSpider node
- mysql  
>TenDB node
- TDBCTL  
>Tdbctl node

> Server_name default is increment from 0 

### Routing Table Example
Below is a local TenDB Cluster with 4 TenDB nodes，2 TSpider nodes，1 Tdbctl nodes 

|Server_name|Host|Db|Username|Password|Port|Socket|Wrapper|
| :--- | :----|:--- | :----|:--- | :----|:--- | :---|
|TDBCTL0|127.0.0.1|de|mysql|mysql|26000||TDBCTL|
|SPIDER0|127.0.0.1||mysql|mysql|25000||SPIDER|
|SPIDER1|127.0.0.1||mysql|mysql|25001||SPIDER|
|SPT0|127.0.0.1||mysql|mysql|20000||mysql|
|SPT1|127.0.0.1||mysql|mysql|20001||mysql|
|SPT2|127.0.0.1||mysql|mysql|20002||mysql|
|SPT3|127.0.0.1||mysql|mysql|20003||mysql|


## Routing Configuration
At present, the routing is configured with INSERT SQL, which is maintained by Tdbctl node, Tdbctl node will sync to other TSpider nodes.  
After Tdbctl reads the SPIDER information in the mysql.servers, it will establish connections to each TSpider node and overwrite their mysql.servers.
>After the routing configuration is completed, we need to execute a FLUSH operation on the Tdbctl node to trigger synchronization. The syntax is as follows
```sql
tdbctl flush routing;
```

>The cluster internal communication with mysql protocol, so after routing configuration completed, we need to do GRANT SQL to ensure node's privileges.  
Do Grant before execute `tdbctl flush routing`  

TSpider: need `SELECT, DELETE, INSERT, DROP` privileges  
Tdbctl: need `ALL PRIVILEGES` to TSpider, TenDB nodes
Detail privilege information refer to[xxxxxxxxxxxxxTODO]

## Routing Consistency
Tdbctl node will regularly check the difference between TSpider and Tdbctl, and periodically synchronize its own configuration to TSpider node, and perform a `FLUSH` operation to make the routing effective
### Variables Reference 
- tc_check_repair_routing_interval
> interval of regularly check routing's diffrence between TSpider and Tdbctl, default 300(s). 
- tc_check_repair_routing
> repiar TSpider node's routing if different from Tdbctl node， default to on


## Cluster Slave Routing
TenDB Cluster can be configured as a read-write separate cluster solution.   
When the master-slave replication is configured in TenDB, the slave cluster can be configured so that read requests only access TenDB slave nodes, and reduce the read pressure of the TenDB master nodes.
The Slave cluster's routing is also maintained on the Tdbctl node, which wrapper type is `SPIDER_SLAVE`，and all data layer routing point to TenDB slave nodes.
- SPIDER_SLAVE
>When Tdbctl sync routing to TSpider node, it will skip nodes whose Wrapper is `SPIDER_SLAVE`.  
For DDL SQL, Tdbctl still sync to `SPIDER_SLAVE`
