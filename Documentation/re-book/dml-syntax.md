# TenDB Cluster DML

开发者在TSpider节点上使用的DML会直接路由给后端TenDB节点。下面以表test_tspider为例，说明select,update,insert,delete语句的用法。

```
CREATE TABLE `test_tspider` (
  `id` bigint(20) NOT NULL,
  `name` varchar(20),
  `age` int,
  `level` int,
  PRIMARY KEY (`id`),
  KEY idx_level(`level`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
/*!50100 PARTITION BY LIST (crc32(id)%4)
(PARTITION pt0 VALUES IN (0) COMMENT = 'database "d1_0", table "test_tspider", server "SP0" ENGINE = SPIDER,
 PARTITION pt1 VALUES IN (1) COMMENT = 'database "d1_1", table "test_tspider", server "SP1" ENGINE = SPIDER,
 PARTITION pt2 VALUES IN (2) COMMENT = 'database "d1_2", table "test_tspider", server "SP2" ENGINE = SPIDER,
 PARTITION pt3 VALUES IN (3) COMMENT = 'database "d1_3", table "test_tspider", server "SP3" ENGINE = SPIDER) */
```




## SELECT
SELECT 语句用于从后端 TenDB 读取数据,下面将依次说明几种常见类型SELECT语句的用法及原理。
### 指定分区键
select * from test_spider where id = 1 ;   
TSpider根据分区规则，语法分析识别分区键，将语句路由到指定后端TenDB。

### 不带分区键
```
select * from test_spider where level > 3;  
SQL会分发到所有后端TenDB执行，结果合并返回。
```

### order by  
```
select * from test_spider where level > 3 order by age desc;  
SQL会分发到所有后端TenDB执行 select * from test_spider where level > 3, TSpider负责排序。
```

### count/min/max/sum 
```
select count(*) from test_spider where age > 10 group by level;  
所有后端TenDB执行select count(*), level from test_spider where age > 10 group by level.   
TSpider负责将相同level结果相加后返回。
```

##  INSERT

```
MariaDB [tendb_test]> insert into test_tspider(id,name,age,level) values(1,'coco',28,1);
Query OK, 1 row affected (0.01 sec)

MariaDB [tendb_test]> select age from test_tspider   where id=1;
+------+
| age  |
+------+
|   28 |
+------+
1 row in set (0.00 sec)
```


## UPDATE
```
MariaDB [tendb_test]> update test_tspider  set age=29 where id=1;
Query OK, 1 row affected (0.00 sec)
Rows matched: 1  Changed: 1  Warnings: 0

MariaDB [tendb_test]> select age from test_tspider   where id=1;
+------+
| age  |
+------+
|   29 |
+------+
1 row in set (0.00 sec)
```

##  DELETE
```
MariaDB [tendb_test]> delete from  test_tspider   where id=1;
Query OK, 1 row affected (0.00 sec)

MariaDB [tendb_test]>  select age from test_tspider   where id=1;
Empty set (0.00 sec)
```


>对于select insert  update  delete，如果where条件指定了shard_key，TSpider根据分区规则，将SQL路由到指定后端TenDB。如果没有指定，SQL会分发到所有后端TenDB执行。   
当select语句需要分发到多个TenDB实例时，如果[spider_bgs_mode](tspider-parameter.md/#spider_bgs_mode)为0，则串行分发请求；为1则并行请求。  
当insert  update  delete语句需要分发到多个TenDB实例时，如果[spider_bgs_dml](tspider-parameter.md/#spider_bgs_dml)为0，则串行分发请求；为1则并行请求。









## 新增用法

### 随机访问

新增全局参数SPIDER_RONE_SHARD_SWITCH，默认开启，表示随机访问某个分区， 
比如select spider_rone_shard from test_tspider limit 1。 随机从test_tspider中某一个分片获取结果。

示例SQL如下：
```
MariaDB [tendb_test]> select id from test_tspider   limit 1;
+-----+
| id |
+-----+
|  28 |
+-----+
1 row in set (0.00 sec)

MariaDB [tendb_test]> select id from test_tspider   limit 1;
+-----+
| id |
+-----+
|  28 |
+-----+
1 row in set (0.00 sec)
```

```
MariaDB [tendb_test]> set global SPIDER_RONE_SHARD_SWITCH=1;
Query OK, 0 rows affected (0.01 sec)

MariaDB [tendb_test]> select  spider_rone_shard  id from test_tspider   limit 1;
+-----+
| id |
+-----+
|   4 |
+-----+
1 row in set (0.00 sec)

MariaDB [tendb_test]> select  spider_rone_shard  id from test_tspider   limit 1;
+-----+
| id |
+-----+
|  28 |
+-----+
1 row in set (0.00 sec)
```


### 指定等值shard_key

新增参数spider_query_one_shard，spider_transaction_one_shard

当spider_query_one_shard指定为true时,控制update/delete/select必须带等值的shard_key作为条件。对于两表的join，如果其中一个表指定shard_key，也可以符合此参数要求。  
当spider_transaction_one_shard指定为true时，控制事务中多个query必须路由到同一个shard中。
另外增加 config_table语法，指定某表为config_table，则跳过spider_query_one_shard的限制，但不能跳过spider_transaction_one_shard的限制。config_table用法如下。
```
    CREATE TABLE `t6` (
        `id` int(11) NOT NULL,
         PRIMARY KEY (`id`)
    ) ENGINE=InnoDB  COMMENT='shard_key "id", config_table "true"'
```

示例SQL如下：
```
MariaDB [tendb_test]> set global spider_query_one_shard=1;
MariaDB [tendb_test]>  select age from test_tspider   where id>4;
ERROR 1735 (HY000): Select across multiple partitions denied to user 'use'@'ip' for table 'test_tspider'

MariaDB [tendb_test]>  update test_tspider  set age='11'  where id>4;
ERROR 1735 (HY000): Update across multiple partitions denied to user 'use'@'ip' for table 'test_tspider'
```
```
MariaDB [tendb_test]> set global spider_transaction_one_shard=1;
MariaDB [tendb_test]>  begin;
Query OK, 0 rows affected (0.00 sec)

MariaDB [tendb_test]>  select age from test_tspider   where id=1;
+-----+
| age |
+-----+
|  10 |
+-----+
1 row in set (0.00 sec)

MariaDB [tendb_test]>  update test_tspider  set age='11'  where id=2;
ERROR 1734 (HY000): Transaction across different partitions denied to user 'use'@'ip'

```
<font color="#dd0000">注意：</font>   
>spider_query_one_shard、spider_transaction_one_shard仅在非super权限下生效。

## 建议
1.不频繁有对分区键的更新（频繁使用会影响性能，下同)  
2.join的时候，不建议全表join  
3.使用avg建议改用sum/count