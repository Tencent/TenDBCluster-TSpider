# Tdbctl Variables Reference

Tdbctl is implemented base on MySQL, so the configuration parameters is consistent with MySQL.   
This chapter mainly introduce the new variables in TenDB.

**`TC_ADMIN`**  
Scope: SESSION|GLOBAL   
Default Value: OFF     
If off, Tdbctl node works like single MySQL;   
If on, Tdbctl node will forward DDL statement to TSpider and TenDB node.
> Note: when TSpider node send DDL to Tdbctl node, tc_admin always be set to ON its session.


**`TC_FORCE_EXECUTE`**   
Scope: SESSION|GLOBAL   
Default Value: ON   
If on, Tdbctl still forward DDL to TSpider node even if TenDB node had executed failed.
> Note: Tdbctl always forward DDL to all TenDB nodes execute first.


**`TC_CHECK_REPAIR_ROUTING`**   
Scope: GLOBAL   
Default Value: ON   
If on, Tdbctl node will fix TSpider node's routing if is inconsistent with Tdbctl. 

**`TC_CHECK_REPAIR_ROUTING_INTERVAL`**   
Scope: GLOBAL   
Default Value: 300   
If on, Tdbctl node regular check routing's consistency between Tdbctl node and TSpider node. 


**`TC_MYSQL_WRAPPER_PREFIX`**   
Scope: GLOBAL   
Default Value: SPT   
Dynamic: 0  
This parameter constrains the Server_name prefix corresponding to the routing information of the TenDB node in the table mysql.servers.  
Server_name is generate by this with numeric suffix. 


**`TC_SPIDER_WRAPPER_PREFIX`**  
Scope: GLOBAL   
Default Value: SPIDER   
Dynamic: 0  
This parameter constrains the Server_name prefix corresponding to the routing information of the TSpider node in the table mysql.servers.  
Server_name is generate by this with numeric suffix. 


**`tc_set_changed_node_read_only`**   
Scope: GLOBAL   
Default Value: OFF   
If on, when update routing rules, changed TenDB node will be set to read-only.  
