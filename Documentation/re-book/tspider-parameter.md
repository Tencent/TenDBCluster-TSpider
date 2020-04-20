# TSpider参数说明

TSpider兼容MySQL的参数，下文主要介绍新增的参数。

**`log_sql_use_mutil_partition`**  
作用域: GLOBAL  
默认值: OFF  
值为true时会把跨分区query记录到慢查询中。

**`spider_max_connections`**  
作用域: GLOBAL  
默认值: 0  
用于控制TSpider节点对单个后端TenDB的最大连接数。spider_max_connections默认为0，为不打开连接池功能； spider_max_connection配置为大于0，为指定TSpider节点与单个后端TenDB实例中最大的连接个数； spider_max_connections的经验值为200。

**`spider_index_hint_pushdown`**  
作用域: SESSION|GLOBAL  
默认值: OFF  
值为ON时，表示force index下发到后端；否则表示不下发。

**`spider_auto_increment_mode_switch`**  
作用域: GLOBAL  
默认值: OFF  
只读  : 1  
值为ON时表示开启自增列功能，否则不开启。

**`spider_auto_increment_step`**  
作用域: GLOBA  
默认值: 17  
只读  : 1  
值为17表示当前TSpider节点以步长17为自增单位

**`spider_auto_increment_mode_value`**  
作用域: GLOBAL  
默认值: 1  
只读  : 1  
值表示为当前TSpider节点自增列值模spider_auto_increment_step后的值，需要保证各个TSpider节点spider_auto_increment_mode_value不同。


**`spider_net_read_timeout`**  
作用域: SESSION|GLOBAL  
默认值: 3600(秒)  
值为等待从远程服务器接收数据的超时时间，单位为秒

**`spider_net_write_timeout`**  
作用域: SESSION|GLOBAL  
默认值: 3600(秒)  
值为等待发送数据到远程服务器超时时间, 单位为秒

**`SPIDER_SUPPORT_XA`**  
作用域: GLOBAL  
默认值: ON  
只读  : 1  
值为ON表示支持XA事务

<a id="spider_bgs_mode"></a>

**`spider_bgs_mode`**  
作用域: SESSION|GLOBAL  
默认值: 0  
值为0表示不开启并行功能;值为1表示开启并行功能，TSpider集群在接受应用层的SQL请求后，将判定SQL需要路由到哪些具体的后端TenDB实例执行，然后并行在各个后端TenDB实例执行，最后统一汇总结果。limit x,y、多表关联、非跨分区行为、事务等场景的，不使用并行功能。



<a id="spider_bgs_dml"></a>

**`spider_bgs_dml`**  
作用域: SESSION|GLOBAL  
默认值: 0  
当开启并行功能时，值为1表示insert  update  delete开启并行功能;否则insert  update  delete不开启并行功能。


<a id="SPIDER_INTERNAL_XA"></a>

**`SPIDER_INTERNAL_XA`**  
作用域: GLOBAL  
默认值: OFF  
值为OFF表示不开启分布式事务；否则表示使用分布式事务。

**`SPIDER_TRANS_ROLLBACK`**  
作用域: SESSION  
默认值: OFF
值为OFF表示，在事务begin/commit过程中某query出错，后续SQL可以继续执行；值为ON表示某个query出错则事务回滚，保证数据一致性，使用xa事务的环境建议打开。


**`SPIDER_GENERAL_LOG`**  
作用域: GLOBAL  
默认值: OFF  
值为OFF表示不打开general log；否则表示打开general log。


**`SPIDER_IGNORE_CREATE_LIKE`**  
作用域: GLOBAL  
默认值: ON  
值为ON表示，在TSpider节点上执行create table like时，忽略每个分区中comment信息。 避免TSpider使用create table like后的误操作。

<a id="SPIDER_IGNORE_AUTOCOMMIT"></a>
**`SPIDER_IGNORE_AUTOCOMMIT`**  
作用域: GLOBAL   
默认值: OFF  
当值为1。若后端TenDB使用了autocommit=0，在启用这个参数后，当前事务提交，再次使用到该TSpider到remote实例的，会自动将session级状态为设置autocommit=1（不再是应用层指定的0），后续执行的SQL都为自动提交；否则不将TenDB session级状态为设置autocommit=1。

<a id="quick-mode"></a>
**`SPIDER_QUICK_MODE`**  

作用域: SESSION|GLOBAL  
默认值：1  
决定后端query结果集，缓冲到后端缓存还是本地缓冲区  
0 本地缓冲区，要求通过store_result一次性获得结果。  
1 后端缓冲区，要求逐一的获得结果。中断不等待，上下文切换恢复。  
2 后端缓冲区，要求逐一的获得结果。获取完所有数据才中断。  
3 本地缓冲区，当结果集比spider_quick_page_size大的时候,需要在磁盘上使用临时表。 

**`SPIDER_IDLE_CONN_RECYCLE_INTERVAL`**   
作用域: GLOBAL  
默认值: 3600  
值为3600，表示TSpider空闲连接的回收时间间隔为3600秒。


**`SPIDER_GET_STS_OR_CRD`**  
作用域: GLOBAL  
默认值: OFF  
值为OFF表示不开启统计信息；否则表示启用统计信息。


**`DDL_EXECUTE_BY_CTL`**  
作用域: SESSION|GLOBAL  
默认值: OFF  
值为ON表示，将DDL转发给Tdbctl处理；否则表示不转发给Tdbctl处理


**`TDBCTL_WRAPPER_NAME`**  
作用域: GLOBAL  
默认值: TDBCTL  
只读  : 1  
本参数约束集群中Tdbctl节点的路由信息对应的Server_name前缀。


**`TDBCTL_SKIP_DDL_CONVERT_DB`**  
作用域: GLOBAL  
默认值: performance_schema,information_schema,mysql,test,db_infobase  
只读  : 1  
默认值`performance_schema,information_schema,mysql,test,db_infobase`， 即这些库下在ddl操作不起ddl分发到控制节点Tdbctl的逻辑。


**`SPIDER_PARALLEL_LIMIT`**  
作用域: GLOBA  
默认值: OFF  
值为ON表示对于包含limit的SQL启用并行执行功能，否则不开启。


**`SPIDER_PARALLEL_GROUP_ORDER`**  
作用域: GLOBAL  
默认值: ON  
值为ON表示对于包含group by, order by的SQL启用并行执行功能，否则不开启。

 **`SPIDER_CONN_RECYCLE_MODE`**  
作用域: SESSION|GLOBAL  
默认值: 1  
值为1表示连接资源在当前实例中循环被使用；


**`SPIDER_FORCE_COMMIT`**  
作用域: SESSION|GLOBAL  
默认值: 0  
控制xa prepare, xa commit, and xa rollback.阶段的行为。  
0 出错即返回  
1 xid不存在即返回，否则继续执行  
2 继续执行，忽略错误


**`SPIDER_INTERNAL_OFFSET`**  
作用域: SESSION|GLOBAL  
默认值: 0  
值为代表后端的偏移数，例如：
`set SPIDER_INTERNAL_OFFSET = 2`
在TSpider执行 `select c from t `，将下发 `select  c from t limit 2,9223372036854775807` 到后端。默认不偏移。


**`SPIDER_INTERNAL_LIMIT`**  
作用域: SESSION|GLOBAL  
默认值: 0  
值为请求后端记录的限制数，例如：
set SPIDER_INTERNAL_LIMIT=2；
在TSpider执行 `select c from t` , 将下发 ` select c from t limit 2; ` 到后端。默认不限制个数。



**`SPIDER_SPLIT_READ`**  
作用域: SESSION|GLOBAL  
默认值: -1  
值为下发到后端sql的获取的最多记录数。  
默认配置，在TSpider执行 `select * from t1`，将下发  `select * from t1`到后端。
如果SPIDER_SPLIT_READ=3，
将下发以下sql到后端，每次最多读取3条记录：  
select `c` from `spider_test_0`.`t1` limit 3 ;  
select `c` from `spider_test_0`.`t1` limit 3,3;   
select `c` from `spider_test_0`.`t1` limit n,3;  


**`SPIDER_SEMI_SPLIT_READ`**  
作用域: SESSION|GLOBAL  
默认值: 1  
值为含有limit的SQL，分发到后端扩大的倍数。如果SPIDER_SEMI_SPLIT_READ=2。在TSpider上执行类似limit语句 select id from test_incr limit 2;在每个后端TenDB是执行select id from test_incr limit 4; 每次都会将limit扩大2倍，存在无意义的性能损耗。默认值为1，表示不扩大。



**`SPIDER_SEMI_SPLIT_READ_LIMIT`**  
作用域: SESSION|GLOBAL  
默认值: -1  
值为-1表示对于含有limit的SQL语句没有极限值,即不做拆分。例如:  
spider_semi_split_read=1
spider_semi_split_read_limit=-1
在TSpider执行select id from  test_incr limit 4；在remote每个节点是执行select id from test_incr limit 4。
如果spider_semi_split_read_limit=2，则执行select id from test_incr limit 2;select id from test_incr limit 2,2;





**`SPIDER_SEMI_TRX_ISOLATION`**  
作用域: SESSION|GLOBAL  
默认值: -1  
值为控制执行sql时的隔离级别：
-1 不开启  
0  未提交读  
1 已提交读  
2 可重复读  
3 可串行化




**`SPIDER_SELUPD_LOCK_MODE`**  
作用域: SESSION|GLOBAL    
默认值: 1  
决定select with update时候的加锁情况  
0：不加锁  
1：共享锁  
2：排他锁  



**`SPIDER_SYNC_AUTOCOMMIT`**  
作用域: SESSION|GLOBAL  
默认值: ON  
值为ON， 表示会分发auto-commits sql到后端TenDB；否则不分发。



**`SPIDER_SYNC_TIME_ZONE`**  
作用域: SESSION|GLOBAL  
默认值: ON  
值为ON，表示会分发时间信息到后端TenDB；否则不分发。




**`SPIDER_INTERNAL_SQL_LOG_OFF`**  
作用域: SESSION|GLOBAL  
默认值: 1  
值为1表示，在General  Log打开的场景下sql不写入后端TenDB  General Log;为0表示写入。


**`SPIDER_BULK_SIZE`**  
作用域: SESSION|GLOBAL  
默认值: 16000  
值为16000表示，TSpider bulk insert时，缓冲区的大小是16000字节


**`SPIDER_BULK_UPDATE_SIZE`**  
作用域: SESSION|GLOBAL  
默认值: 16000  
值为16000表示，TSpider bulk update 和delete时，缓冲区的大小是16000字节



**`SPIDER_CONNECT_TIMEOUT`**  
作用域: SESSION|GLOBAL  
默认值: 0  
表示TSpider与后端连接的超时时间，单位为秒。


**`SPIDER_DIRECT_DUP_INSERT`**  
作用域: SESSION|GLOBAL  
默认值: 1  
值为1表示对insert on duplicate类语句时，直接分发到remote实例中，无须先执行select。否则表示需要先执行select。


**`SPIDER_REMOTE_ACCESS_CHARSET`**  
作用域: SESSION|GLOBAL  
默认值: NULL  
表示TSpider与后端TenDB建立连接时设置的字符集。


**`SPIDER_REMOTE_AUTOCOMMIT`**  
作用域: GLOBAL  
默认值: -1  
值为-1，表示不改变后端auto-commit值；值为0，表示改变后端auto-commit值为0；值为1，表示改变后端auto-commit值为1；



**`SPIDER_REMOTE_DEFAULT_DATABASE`**  
作用域: GLOBAL  
默认值: NULL   
表示与后端TenDB建立连接时，设置的库名


**`SPIDER_CONNECT_RETRY_INTERVAL`**  
作用域: SESSION|GLOBAL  
默认值: 1000  
值为1000，表示建立连接失败后重试的时间间隔为1000秒；


**`SPIDER_QUICK_MODE_ONLY_SELECT`**  
作用域: GLOBAL  
默认值: ON  
值为ON，表示控制复杂select语句（如：delete/insert into from select .. 通过limit x, y分批拉取数据）使用quick_mode为0的逻辑。


**`SPIDER_IGNORE_XA_LOG`**  
作用域: SESSION|GLOBAL  
默认值: ON  
值为ON，表示不记录TSpider中的xa事务执行日志，记录日志非常消耗性能；否则表示记录xa事务日志。


**`SPIDER_GROUP_BY_HANDLER`**  
作用域: GLOBAL  
默认值: OFF  
值为OFF，表示禁止掉TSpider的direct join行为。


**`SPIDER_RONE_SHARD_SWITCH`**  
作用域: GLOBAL  
默认值: ON  
值为ON，表示随机访问某个分区，比如select spider_rone_shard from tb limit 1。 随机从tb中某一个分片获取结果。


**`SPIDER_QUERY_ONE_SHARD`**  
作用域: GLOBAL  
默认值: OFF  
值为ON，表示update/delete/select必须带等值的shard_key作为条件。对于两表的join，如果其中一个表指定shard_key，亦可支持；否则无此限制。


**`SPIDER_TRANSACTION_ONE_SHARD`**  
作用域: GLOBAL  
默认值: OFF  
值为ON，表示事务中多个query必须路由到同一个shard中。否则无此限制。  


>如果指定某表为config_table，则跳过spider_query_one_shard的限制，但不能跳过spider_transaction_one_shard的限制。config_table用法如下。  
    CREATE TABLE `t6` (  
        `id` int(11) NOT NULL,  
         PRIMARY KEY (`id`)  
    ) ENGINE=InnoDB  COMMENT='shard_key "id", config_table "true"'  
需要注意的是，前面两个参数spider_query_one_shard、spider_transaction_one_shard仅在非super权限下生效。



**`SPIDER_DIRECT_LIMIT_IN_GROUP`**  
作用域: GLOBAL  
默认值: FALSE  
值为FALSE，表示对group by + limit 的query，不分发limit 到后端TenDB。


**`SPIDER_MODIFY_STATUS_INTERVAL`**  
作用域: GLOBAL  
默认值: 28800  
用于控制spider_table_status的更新频率



**`SPIDER_SLOW_LOG`**  
作用域: GLOBAL  
默认值: OFF  
值为ON表示在TSpider节点记录的慢查询中增加明细，即TSpider是具体是分发了哪些sql到后端TenDB实例中；否则表示不增加明细。



**`SPIDER_IGNORE_SINGLE_SELECT_INDEX`**  
作用域: GLOBAL  
默认值: ON  
值为ON表示在TSpider上，单表select不走索引；否则表示走索引。



**`SPIDER_IGNORE_SINGLE_UPDATE_INDEX`**  
作用域: GLOBAL  
默认值: ON  
值为ON，表示在TSpider上，单表update不走索引；否则表示走索引。


**`SPIDER_DELETE_ALL_ROWS_TYPE`**  
作用域: SESSION|GLOBAL  
默认值: 1  
值为1，表示走正常delete逻辑，会成功返回delete影响的行数；



**`SPIDER_CONNECT_ERROR_INTERVAL`**  
作用域: GLOBAL  
默认值: 1
表示连接失败后返回错误码的时间间隔


**`SPIDER_VERSION`**
作用域: GLOBAL  
默认值: 1  
TSpider引擎版本号   


**`SPIDER_INTERNAL_XA_ID_TYPE`**  
作用域: SESSION|GLOBAL  
默认值: 0  
xid的类型


**`SPIDER_CONNECT_RETRY_COUNT`**  
作用域: SESSION|GLOBAL  
默认值: 20  
表示建立连接失败后重试的次数；


**`SPIDER_CONNECT_MUTEX`**  
作用域: GLOBAL  
默认值: OFF  
值为OFF。表示建立连接时，不使用锁；否则表示使用锁。



**`SPIDER_READ_ONLY_MODE`**  
作用域: SESSION|GLOBAL  
默认值: 0  
值为0，表示TSpider相关表可写；值为1，表示TSpider相关表只读。



**`SPIDER_LOG_RESULT_ERRORS`**  
作用域: GLOBAL  
默认值: 1  
值为1，表示TSpider上执行出错信息记录到错误日志中；否则表示不记录。


**`SPIDER_LOG_RESULT_ERROR_WITH_SQL`**  
作用域: GLOBAL  
默认值: 3  
日志级别，例如值为3， 即在执行出错时，在错误日志中记录主线程query及分发到remote执行出错的query；



**`SPIDER_CONN_WAIT_TIMEOUT`**  
作用域: GLOBAL  
默认值: 20  
值为20，表示从连接池中获取连接时的等待时间为20秒；



**`SPIDER_REMOTE_TIME_ZONE`**  
作用域: GLOBAL  
默认值: NULL  
表示与后端TenDB建立连接时设置的时区。



**`SPIDER_REMOTE_SQL_LOG_OFF`**  
作用域: GLOBAL  
默认值: 0  
值为0表示，后端TenDB在打开general_log后不能看到分发的query；值为1表示，后端TenDB在打开general_log后能看到分发的query。



**`SPIDER_REMOTE_TRX_ISOLATION`**  
作用域: GLOBAL  
默认值: -1  
表示与后端建立连接时设置的隔离级别：
-1 不设置  
0  未提交读  
1 已提交读  
2 可重复读  
3 可串行化  




**`SPIDER_QUICK_PAGE_SIZE`**  
作用域: SESSION|GLOBAL  
默认值: 1000  
值为1000，表示当匹取数据时一次取的行数为1000。


<a id="SPIDER_WITH_BEGIN_COMMIT"></a>

**`SPIDER_WITH_BEGIN_COMMIT`**  
作用域: SESSION|GLOBAL  
默认值: OFF  
值为OFF表示非显式事务不分发begin/commit到后端TenDB。



**`SPIDER_CLIENT_FOUND_ROWS`**  
作用域: SESSION|GLOBAL  
默认值: OFF  
值为OFF表示使用mysql_affected_rows的时候，不返回匹配的行数;否则返回匹配的行数。



**`SPIDER_LOCK_EXCHANGE`**   
作用域: SESSION|GLOBAL  
默认值: OFF    
值为OFF表示不将select lock 转换为lock tables;否则表示转化。