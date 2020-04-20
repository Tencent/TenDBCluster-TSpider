# FAQ

下面对TenDB Cluster集群使用中，用户的常见问题进行说明。

## 1. 常见问题

### 1.1 ERROR 12701
> ERROR 12701 (HY000): Remote MySQL server has gone away

12701错误有些也会伴随有错误1159，因网络导致的异常。   
接入层TSpider通过连接池与和存储层交互； 在存储实例单方便把连接断开时接入层TSpider没有感知到这个信息； 当前新请求过来TSpider使用到该异常的连接时就会报12701的错误。以下几种方式可以TSpider侧的异常连接快速释放：
1. 设置spider_idle_conn_recycle_interval=8（最小值），则8秒后所有空闲连接都会释放
2. 在TSpider上重复发送请求，新的请求即可正常执行。（旧的连接因为执行出错会被回收）
3. 重启TSpider节点，则一次性释放所有连接

### 1.2 ERROR 12723
> ERROR 12723 (HY000): Too many connections between spider and remote

接入层TSpider通过连接池与存储实例交互，TSpider和每个存储实例最多同时维护spider_max_connections个数的连接，在应用层瞬间新请求过多会造成spider_max_connections不够用的情况。  因此可通过如下方法解决此类问题：
1. 适当调大spider_max_connections到能够满足要求
2. 针对应用请求进行优化，如果SQL执行够快，那么占用的连接资源就会更少

### 1.3 ERROR 1477
> ERROR 1477 (HY000): The foreign server name you are trying to reference does not exist. Data source error:  SPT0

建表使用的server(SPT0)并不存在，可以检查mysql.servers表是否真的不存在； 如果mysql.servers表中存在SPT0却还报错，说明该server没有执行flush privileges生效

### 1.4 ERROR 1429
> MySQL error code 1429 (ER_CONNECT_TO_FOREIGN_DATA_SOURCE): Unable to connect to foreign data source: %.64s

某个后端存储实例故障，导致对应的server不可用；需要进行主备切换或者修复故障实例

### 1.5 Out of sync错误
非预期的TSpider向存储实例请求数据时出现异常，执行flush tables可解决。


### 1.6 自增列
TSpider只保证自增列的唯一性， 有空洞、且不保证自增字段的有序；更多信息参见[自增列](../re-book/auto-increase.md)章节

### 1.7 SQL请求慢
1. 可以打开general_log、 spider_general_log，查看当前请求的分发请求是否符合预期；分发到存储实例执行的请求是否命中索引   
2. 可以设置spider_bgs_mode=1让读操作并行
3. 对涉及多个shard的update/delete/insert操作，在打开spider_bgs_mode前提下打开spider_bgs_dml，让update/delete/insert操作并行
4. 关注存储实例和TSpider间的网络延迟   

### 1.8 OOM
`TSpider OOM`   
用户一次拉取的数据量过大； 尽量避免在TSpider一次性拉取过量数据，非要这样用避免OOM的办法是设置spider_quick_mode=3   
`TenDB OOM`   
通常是buffer pool配置过大，且瞬间连接过多导致；   解决方法是调低buffer pool，使用连接池处理连接问题； TSpider侧也可以调下spider_max_connections；

### 1.9 多唯一键错误
TSpider的唯一键约束规则和MySQL分区表规则相同，多唯一键问题详见[多唯一键调整](multi-unique-key-adjust.md)章节


### 1.10 调整shard key
调整shard key详见 [shard key选取](shard-key-choose.md)章节

### 1.11 TSpider出入流量相差大
一般是TSpider的入流量比出流量大，导致这个原因是部分请求需要拉取大量数据到TSpider层进行计算，TSpider只返回给应用计算后的结果； 比如全表排序取最大的记录，会就导致入流量大于出流量。

### 1.12 存储实例负载不均
考虑shard_key是否配置合理，导致数据及请求分布不均

### 1.13 查询超时
执行复杂查询，可能返回结果超时，一般是因为spider_net_read_timeout/spider_net_write_timeout配置过小

### 1.14 统计信息
通过全局参数spider_get_sts_or_crd开启统计信息，统计信息存储在系统表`mysql`.`spider_table_status`中；   `spider_modify_status_interval`为刷新统计信息的间隔时间。



## 2. 与MySQL的差异性
详见章节[与MySQL的差异](../re-book/mysql-compatibility.md/#jump)
## 3. 架构
详见章节[架构](../architecture.md)
## 4. 事务
详见章节[事务](../re-book/transaction.md)
## 5. SQL语法
详见章节[SQL语法](../re-book/sql-grammar.md)  

