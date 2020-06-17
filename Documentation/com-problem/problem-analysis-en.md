# TenDB Cluster Common Problem(FAQ)

The following describes common problems of using TenDB cluster.

## 1. Client Error Code
###  1.1 ERROR 12701
> ERROR 12701 (HY000): Remote MySQL server has gone away
	 
Usually, Error Code 12701 come with Error Code 1159, it maybe caused by Network partition error.
TSpider Node use connection pool to interactive with all the TenDB Node, some network partition error may cause TenDB Node unilateral close the connection from the TSpider Node. When the TSpider Node accept new query from the client, it will reuse the problematic connection so it will return Error Code 12701 to the client. that is three ways to quickly release the problematic connection:

a. Set the global variables on TSpider Nodes, spider_idle_conn_recycle_interval = 8 which is the minimum value, it means that after 8 seconds all the idled connections will be released by Spider Node.   
b. The Client resend the query to TSpider Node, and the problematic connection will be released immediately and reconnect to TenDB Node again.   
c. Restart TSpider Node, so all the connection will be Re-established.
	  
### 1.2 ERROR 12723
> ERROR 12723 (HY000): Too many connections between TSpider Node and TenDB Node
	
TSpider Node use connection pool to interactive with all the TenDB Node, each TSpider Node will establish spider_max_connections connections with each TenDB Node, when 
a large number of new connections flooded in an instant to TSpider Node, it may encounter no enough connnection to use and so Client will receive Error Code 12723. That is two ways to solve this problem:  

a. set a bigger `spider_max_connections`.

b. optimizate the SQL, when the SQL execute more quickly, it will occupy the connection less time.

### 1.3 ERROR 1477

> ERROR 1477 (HY000): The foreign server name you are trying to reference does not exist. Data source error: SPT0
	
We store the TenDB Instances info and partition info in mysql.servers in TSpider Node , when you receive this Error Code, It seems that the configuration table mysql.servers not exists or you have the table but when you create the TenDB Cluster but you forget to execute FLUSH PRIVILEGES to make the router table available.

### 1.4 ERROR 1429

> MySQL error code 1429 (ER_CONNECT_TO_FOREIGN_DATA_SOURCE): Unable to connect to foreign data source: %.64s
	
When you receive this Error Code, it means that some TenDB master is unavailable, you need to switch to Slave or fix the problem on master quickly.

## 2. Other problem

### 2.1 Out of Sync Error

Unexpected exception occurs when TSpider Node query to the TenDB Node, execute "set global spider_idle_conn_recycle_interval=8"(free idle conn quickly)  and "FLUSH TABLES" on TSpider Node may resolve it

### 2.2 Auto-Increment

TenDB Cluster will ensure the Auto-Increment Column to be unique in the cluster scope, but TenDB Cluster cannot ensure the Auto-Inc column increase 1 by 1, It also will miss some value and the smaller value will be generated later.

for example, follow the timeline, the generated auto-inc ID will be like this: 

      2 6 10 3 7 11 1 5 9 etc.
      
You should know this detail and make sure your program should not relie the Auto-Inc feature.     
For details see [chapter Autoincrement](../re-book/auto-increase-en.md)


### 2.3 Analyse the SlOW QUERY
There is some steps to solve the TenDB Cluster SLOW QUERY:

1. Set global general_log = on and set global spider_general_log = on, and use pt-query-digest to analyse the SQL.
2. Set global spider_bgs_mode=1, which TSpider Node will make read operation executed in parallel on all the TenDB Nodes.
3. When update/delete/insert SQL involve seraval TenDB Nodes, you can `set global spider_bgs_mode=1` and `set global spider_bgs_dml = 1`, which TSpider Node will make  update/delete/insert SQL executed in parallel on all the involved TenDB Nodes
4. Check the network latancy between TSpider Node and TenDB Node, you can ping from TSpider Node and TenDB Node to check the ttl time between the two Nodes.

### 2.4 OOM Problem

#### TSpider Node OOM
This will happen when Client fetch large data from TSpider Node, also you can Set spider_quick_mode=3 to avoid this problem, bug we suggest you don't use TSpider in this way.

#### TenDB Node OOM
This will happen when you config the larger innodb_buffer_pool_size on TenDB Node, and a large number of new connections flooded in an instant, you can lower down the buffer pool size and use connection pool between client and TSpider Node.

### 2.5 Network Flow_In and Flow_Out In TSpider Node is much different
Commonly, network Flow In on TSpider Node will be larger than Flow Out, because TSpider Node need to fetch more data from TenDB Node and calculate the result and return to Client. for example, TSpider Node may execute `SELECT max(COLUMN) from TBALE`.

### 2.6 TenDB Nodes may have different work load
It may happen that you use the unsuitable Shard Key, so it result that different TenDB Node will have different data and request.

### 2.7 Query TimeOut
When you execute complicated QUERY, it will result in query TimeOut, you can set global spider_net_read_timeout/spider_net_write_timeout to avoid this TimeOut problem.

### 2.8 Statistics Information
Set global spider_get_sts_or_crd = on on TSpider Node to collect table statistics information from TenDB Nodes, and TSpider Node will store these information into mysql.spider_table_status table. Because this operation will produce some work load and you can use spider_modify_status_interval variable to control the interval time.