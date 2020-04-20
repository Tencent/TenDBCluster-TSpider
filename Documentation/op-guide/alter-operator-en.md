# TenDB Cluster Alter Operator
TenDB Cluster alter operator means do table DDL operation through any TSpider node.  
The DDL statement will be transparently transmitted to the Tdbctl node, which is rewritten and distributed to all TSpider node in parallel.
At present, there is not much DDL syntax difference between TenDB Cluster  and stand-alone MySQL, please refer to the relevant chapters of DDL for details

## Example of Alter Operator  
Connect to any TSpider node
>mysql -umysql -pmysql -h127.0.0.1 -P25000

- create database 
```sql
create database test1;
show create database test1;
+----------+-------------------------------------------------------------------+
| Database | Create Database                                                   |
+----------+-------------------------------------------------------------------+
| test1    | CREATE DATABASE `test1` /*!40100 DEFAULT CHARACTER SET utf8mb4 */ |
+----------+-------------------------------------------------------------------+
```

- create table 
```sql
use test1;
#Primary key constraint must be specified. The other syntax is the same as ordinary MySQL.
create table t1(a int primary key, b int) ENGINE=InnoDB;
#show table schema status
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
You can see the table on  TSpider node is with sharding information, and the specific sharding routing information is consistent with mysql.servers .

In addition to the fragmentation information in the table schmea , the engine has become SPIDER. These differences are caused by Tdbctl rewriting and sending it to TSpider after receiving the original SQL.

When TSpider create a table, any engine specified will be delivered to the TenDB node. If the engine does not exist in TenDB, the default is InnoDB
If you need to create a non-forwarding table in TSpider, you need to set `ddl_execute_by_ctl` to off.

- change table schema
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