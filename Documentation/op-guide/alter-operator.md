# TenDB Cluster变更
TenDB Cluster变更是指通过任意TSpider节点对集群进行库表的DDL操作，DDL语句会透传给Tdbctl节点, 由其重写后并行分发到TSpider，TenDB节点执行  
目前DDL语法与单机MySQL无太大差异性，具体参照DDL相关章节

## 变更示例 
连接任意TSpider节点
>mysql -umysql -pmysql -h127.0.0.1 -P25000

- 创建库
```sql
create database test1;
show create database test1;
+----------+-------------------------------------------------------------------+
| Database | Create Database                                                   |
+----------+-------------------------------------------------------------------+
| test1    | CREATE DATABASE `test1` /*!40100 DEFAULT CHARACTER SET utf8mb4 */ |
+----------+-------------------------------------------------------------------+
```

- 创建表
```sql
use test1;
#必须指定主键约束，其他语法与普通MySQL相同。
create table t1(a int primary key, b int) ENGINE=InnoDB;
#查看表结构信息
show create table t1;
CREATE TABLE `t1` (
  `a` int(11) NOT NULL,
  `b` int(11) DEFAULT NULL,
  PRIMARY KEY (`a`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8mb4
 PARTITION BY LIST (crc32(`a`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "test1_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "test1_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "test1_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "test1_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```
可以看到，在TSpider节点上，表是带有分片信息的，具体分片路由信息与mysql.servers一致  
表结构除了多了分片信息，引擎也变为了SPIDER，这些差异性都是由Tdbctl在收到原SQL后，重写下发给TSpider造成的
在TSpider建表时，指定任何引擎，都会下发给TenDB节点，如果TenDB不存在此引擎，则默认为InnoDB  
如果需要在TSpider建立非转发表，需要设定`ddl_execute_by_ctl`为off即可

- 修改表结构
```sql
use test1;
#增加字段c
alter table t1 add c int;
show create table t1;
CREATE TABLE `t1` (
  `a` int(11) NOT NULL,
  `b` int(11) DEFAULT NULL,
  `c` int(11) DEFAULT NULL,
  PRIMARY KEY (`a`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8mb4
 PARTITION BY LIST (crc32(`a`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "test1_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "test1_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "test1_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "test1_3", table "t1", server "SPT3"' ENGINE = SPIDER)
 ```

