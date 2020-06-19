# TenDB Cluster DDL

TSpider supports range, hash, list sharding algorithm just like MySQL partition table. TSpider expands comment field of the partition column, so as to specify the server name, database name, table name of the TenDB node. And TSpider will get TenDB's IP, PORT from mysql.servers  according to server name.    
When ddl_execute_by_ctl=ON, the ddl on TSpider will be routed to Tdbctl, the Tdbctl will do ddl to all TSpider and TenDB, which means Cluster DDL operating.      
When ddl_execute_by_ctl=OFF, the ddl on TSpider will not be  routed to Tdbctl, user should do ddl on TSpider and TenDB respectively.  

# 1. TenDB Cluster With Tdbctl

When ddl_execute_by_ctl=ON, Tdbctl is responsible for Cluster DDL operating.    
It should be noted: DDL in databases such as `performance_schema,information_schema,mysql,test,db_infobase`, will not be distributed to  TSpider  node and TenDB node by default.

<a id="jump1"></a>

## 1. Create Table
User can create table just like MySQL,  but the created table is different from MySQL. For example:
```
MariaDB [tendb_test]>  create table t1(
    ->     c int primary key)
    ->     ENGINE=InnoDB DEFAULT CHARSET=utf8;
Query OK, 0 rows affected (0.05 sec)
```

The above SQL on TSpider will be distributed to Tdbctl, then Tdbctl will rewrite the SQL through data sharding rules and distribute it to the corresponding TenDB data node for execution. After create table succeed, we will get result of  executing `show create table` in TSpider:   
```
MariaDB [tendb_test]> show create table t1\G;
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `c` int(11) NOT NULL,
  PRIMARY KEY (`c`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`c`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```

At this time, each TenDB node has schema `tendb_test_0.t1, tendb_test_1.t1, tendb_test_2.t1, tendb_test_3.t1`.

```
mysql> show create table t1\G;
********************* 1. row *********************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `c` int(11) NOT NULL,
  PRIMARY KEY (`c`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
1 row in set (0.00 sec)
```


As can be seen from the table structure on TSpider:   
TSpider will shard table according to shard_key. For example a cluster with 4 TenDB node, the sharding algorithm is `crc32(primary_key) % 4`. And `crc32` is supported by TSpider which make sure distribute data evenly. The column `c` is specified as shard_key.

So how does TSpider choose the shard_key and how should the user specify the shard_key ?

### 1.1 Shard_key

For the SQL specified with shard_key, SQL will only be routed to the specified shard  according to the sharding rules, which can improve the request performance. <font color="#dd0000">
Therefore, users should be very careful in choosing shard_key.</font>   
The below tips explain the default sharding rules of TSpider and how to specify shard_key.
<a id="shard_key"></a>

####  Without Specified Shard_key

##### If there is only one unique key (including primary key) and the shard_key is not specified, the first column of the unique key will be used as the shard_key by default.

```
MariaDB [tendb_test]> create table  t1(
    ->      inf_id  int(11)  auto_increment    not  null,
    ->      name  varchar(50)  not  null,
    ->      sex  varchar(5)  not null,
    ->      birthday  varchar(50)  not  null,
    ->      primary key   info(inf_id,sex)
    ->      );
Query OK, 0 rows affected (0.03 sec)

MariaDB [tendb_test]> show create table t1\G;
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `inf_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `sex` varchar(5) NOT NULL,
  `birthday` varchar(50) NOT NULL,
  PRIMARY KEY (`inf_id`,`sex`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`inf_id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
1 row in set (0.00 sec)
```

##### If there are multiple unique keys and shard_key is not specified, the first field of the unique key will be used as the shard key by default. But user need to ensure that the shard key is the first field of each unique key, otherwise the table cannot be created.

```
MariaDB [tendb_test]> create table t1(c1 int primary key,c2 int,unique key t(c1,c2));
Query OK, 0 rows affected (0.05 sec)

MariaDB [tendb_test]> show create table t1\G;
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `c1` int(11) NOT NULL,
  `c2` int(11) DEFAULT NULL,
  PRIMARY KEY (`c1`),
  UNIQUE KEY `t` (`c1`,`c2`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`c1`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
1 row in set (0.00 sec)

ERROR: No query specified
```
```
MariaDB [tendb_test]> drop table t1;
Query OK, 0 rows affected (0.03 sec)

MariaDB [tendb_test]> create table t1(c1 int primary key,c2 int,unique key t(c2,c1));
ERROR 4151 (HY000): Failed to execute ddl, Error code: 12021, Detail Error Messages: CREATE TABLE ERROR: ERROR: too more unique key with the different pre key
```


##### If there is only one secondary index and shard_key is not specified, the first column of the index will be used as the shard_key by default. 
```
MariaDB [tendb_test]> create table  t1(
    ->     inf_id  int(11)  auto_increment    not  null,
    ->     name  varchar(50)  not  null,
    ->     sex  varchar(5)  not null,
    ->     birthday  varchar(50)  not  null,
    ->     index  info(inf_id,sex)
    ->     );
Query OK, 0 rows affected (0.04 sec)

MariaDB [tendb_test]>  show  create table t1\G;
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `inf_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `sex` varchar(5) NOT NULL,
  `birthday` varchar(50) NOT NULL,
  KEY `info` (`inf_id`,`sex`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`inf_id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
1 row in set (0.00 sec)
```


####  With Specified Shard_key

##### Shard_key can only be the common part of keys if there are multiple unique keys (including the primary key). Otherwise, the table cannot be created.

```
MariaDB [tendb_test]>     create table t1 (
    ->         id int unsigned not null auto_increment, 
    ->         code tinyint unsigned not null, name char(20) not null, 
    ->         primary key (id), 
    ->         unique (id,name)) comment='shard_key "name"';
ERROR 4151 (HY000): Failed to execute ddl, Error code: 12021, Detail Error Messages: CREATE TABLE ERROR: ERROR: name as TSpider key, but not in some unique key
```
```
MariaDB [tendb_test]> create table t1 (
    ->     id int unsigned not null auto_increment, 
    ->     code tinyint unsigned not null, name char(20) not null, 
    ->     primary key (id), 
    ->     unique (id,name)) comment='shard_key "id"';
Query OK, 0 rows affected (0.06 sec)

MariaDB [tendb_test]> show  create table t1\G;
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `code` tinyint(3) unsigned NOT NULL,
  `name` char(20) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`,`name`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8 COMMENT='shard_key "id"'
 PARTITION BY LIST (crc32(`id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
1 row in set (0.00 sec)
```


##### If there are multiple secondary indexes and without unique key(including primary key), user must specify shard_key (Restrictions made on Tdbctl).
```
MariaDB [tendb_test]> create table t1 (
    ->     id int unsigned not null auto_increment, 
    ->     code tinyint unsigned not null, 
    ->     name char(20) not null, 
    ->     key a(id), 
    ->     key b(name));
ERROR 4151 (HY000): Failed to execute ddl, Error code: 12021, Detail Error Messages: CREATE TABLE ERROR: ERROR: too many key more than 1, but without unique key or set shard key
```
```
MariaDB [tendb_test]> create table t1 (
    ->     id int unsigned not null auto_increment, 
    ->     code tinyint unsigned not null, 
    ->     name char(20) not null, 
    ->     key a(id), 
    ->     key b(name)) comment='shard_key "id"';
Query OK, 0 rows affected (0.05 sec)

MariaDB [tendb_test]> show create table t1\G;
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `code` tinyint(3) unsigned NOT NULL,
  `name` char(20) NOT NULL,
  KEY `a` (`id`),
  KEY `b` (`name`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8 COMMENT='shard_key "id"'
 PARTITION BY LIST (crc32(`id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
1 row in set (0.00 sec)
```

##### Shard_key must be Part of the Index (Restrictions made on Tdbctl).

```
MariaDB [tendb_test]> create table t1(
    ->     id int(10) unsigned NOT NULL AUTO_INCREMENT) 
    ->     COMMENT='shard_key "id"';
ERROR 1075 (42000): Incorrect table definition; there can be only one auto column and it must be defined as a key
```
```
MariaDB [tendb_test]> create table t1(
    ->     id int(10) unsigned NOT NULL AUTO_INCREMENT,
    ->     key a(id)) 
    ->     COMMENT='shard_key "id"';
Query OK, 0 rows affected (0.03 sec)

MariaDB [tendb_test]> show  create table t1\G;
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  KEY `a` (`id`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8 COMMENT='shard_key "id"'
 PARTITION BY LIST (crc32(`id`) MOD 4)
 (PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
1 row in set (0.00 sec)
```


### 1.2  Constraint of Create Table

#### GEOMETRY is Unsupported
```
MariaDB [tendb_test]> create table if not exists t1(a int not null primary key, b geometry not null, d int ) engine=innodb;
ERROR 4151 (HY000): Failed to execute ddl, Error code: 12027, Detail Error Messages: DETAIL ERROR INFO: 
Spider Node Error:
```

#### Temporary Table is Unsupported
```
MariaDB [tendb_test]> create temporary table t1 (c int primary key, a char(1) ) engine=innodb;
ERROR 4149 (HY000): SQL TYPE: CRREATE TEMPORARY TABLE ,can not be supported when set ddl_execute_by_ctl
```


#### `CREATE TABLE ... SELECT` is Unsupported

```
MariaDB [tendb_test]>  create table if not exists t2(a int not null, b int, primary key(a)) engine=innodb;
MariaDB [tendb_test]>  create table if not exists t4(a int not null, b int, primary key(a)) engine=innodb select * from t2; 
ERROR 4148 (HY000): SQL TYPE: CRREATE TABLE ... SELECT ,can not be supported when set ddl_execute_by_ctl
```

#### Blob Columns as Primary key is Unsupported
```
CREATE TABLE bug58912 (a BLOB, b TEXT, PRIMARY KEY(a(1))) ENGINE=InnoDB;

MariaDB [tendb_test]> CREATE TABLE bug58912 (a BLOB, b TEXT, PRIMARY KEY(a(1))) ENGINE=InnoDB;
ERROR 4151 (HY000): Failed to execute ddl, Error code: 12027, Detail Error Messages: DETAIL ERROR INFO: 
```


#### Foreign key is Unsupported
```
CREATE TABLE users (
 id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
 doc JSON
);
```
```
CREATE TABLE orders (
 id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
 user_id INT NOT NULL,
 doc JSON,
 FOREIGN KEY fk_user_id (user_id) REFERENCES users(id)
);

ERROR 4151 (HY000): Failed to execute ddl, Error code: 12021, Detail Error Messages: CREATE TABLE ERROR: ERROR: no support key type
```


## 2. Alter Table

ALTER TABLE changes the structure of a table. For example, user can add or delete columns, create or destroy indexes, change the type of existing columns, or rename columns or the table itself. user can also change characteristics such as the storage engine used for the table or the table comment. 

```
ADD，DROP or  RENAME index
ADD，DROP，MODIFY or CHANGE column
```

<font color="#dd0000">Noted: on the TSpider node, it is prohibited to modify the sharding rules when alter table.</font> 
```
MariaDB [tendb_test]> create table t1(c1 int , c2 int,primary key id(c1,c2));
Query OK, 0 rows affected (0.04 sec)

MariaDB [tendb_test]> show create table t1\G;
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `c1` int(11) NOT NULL,
  `c2` int(11) NOT NULL,
  PRIMARY KEY (`c1`,`c2`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`c1`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
1 row in set (0.00 sec)

ERROR: No query specified

MariaDB [tendb_test]> alter table t1 add unique key  idx(c2);
ERROR 4151 (HY000): Failed to execute ddl, Error code: 12023, Detail Error Messages: CREATE/DROP INDEX ERROR: create or drop primary/unique key is not supported
```



### 2.1 ADD COLUMN
`ALTER TABLE.. ADD COLUMN` statement is used to add columns to an existing table.
```
MariaDB [tendb_test]> CREATE TABLE t1 (id INT NOT NULL PRIMARY KEY AUTO_INCREMENT);
MariaDB [tendb_test]> INSERT INTO t1 VALUES (NULL);
MariaDB [tendb_test]> SELECT * FROM t1;
+----+
| id |
+----+
|  1 |
+----+
1 row in set (0.00 sec)
```
```
MariaDB [tendb_test]> ALTER TABLE t1 ADD COLUMN c1 INT NOT NULL;
MariaDB [tendb_test]> ALTER TABLE t1 ADD c2 INT NOT NULL AFTER c1;
MariaDB [tendb_test]> SELECT * FROM t1;
+----+----+----+
| id | c1 | c2 |
+----+----+----+
|  1 |  0 |  0 |
+----+----+----+
1 row in set (0.00 sec)
```






### 2.2 DROP COLUMN
`DROP COLUMN` statement is used to delete columns from the table.

```
MariaDB [tendb_test]> CREATE TABLE t1 (id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, col1 INT NOT NULL, col2 INT NOT NULL);
MariaDB [tendb_test]> INSERT INTO t1 (col1,col2) VALUES (1,1),(2,2),(3,3),(4,4),(5,5);
MariaDB [tendb_test]> SELECT * FROM t1;
+----+------+------+
| id | col1 | col2 |
+----+------+------+
|  1 |    1 |    1 |
|  2 |    2 |    2 |
|  3 |    3 |    3 |
|  4 |    4 |    4 |
|  5 |    5 |    5 |
+----+------+------+
5 rows in set (0.01 sec)
```
```
MariaDB [tendb_test]> ALTER TABLE t1 DROP COLUMN col1, DROP COLUMN col2;
MariaDB [tendb_test]> SELECT * FROM t1;
+----+
| id |
+----+
|  1 |
| 18 |
| 35 |
| 52 |
| 69 |
+----+
```


<font color="#dd0000">Note:  cannot drop the shard_key.</font>   
```
MariaDB [tendb_test]> create table t1(c1 int , c2 int,primary key id(c1,c2));
Query OK, 0 rows affected (0.04 sec)

MariaDB [tendb_test]> show create table t1\G;
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `c1` int(11) NOT NULL,
  `c2` int(11) NOT NULL,
  PRIMARY KEY (`c1`,`c2`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`c1`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
1 row in set (0.00 sec)

ERROR: No query specified

MariaDB [tendb_test]> alter table t1  drop column c1;
ERROR 4151 (HY000): Failed to execute ddl, Error code: 12027, Detail Error Messages: DETAIL ERROR INFO: 
Spider Node Error:
```

###  2.3 ADD INDEX
`ALTER TABLE .. ADD INDEX` statement is used to create  index .

```
MariaDB [tendb_test]> CREATE TABLE t1 (id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, c1 INT NOT NULL);
MariaDB [tendb_test]> show create table t1\G;

CREATE TABLE `t1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `c1` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```

```
MariaDB [tendb_test]> ALTER TABLE t1 ADD INDEX (c1);
MariaDB [tendb_test]> show create table t1\G;

CREATE TABLE `t1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `c1` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `c1` (`c1`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```




### 2.4 DROP INDEX
`ALTER TABLE .. DROP INDEX` statement is used to delete  index.

```
MariaDB [tendb_test]> CREATE TABLE t1 (id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, c1 INT NOT NULL);
MariaDB [tendb_test]> show create table t1;

CREATE TABLE `t1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `c1` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```

```
MariaDB [tendb_test]> CREATE INDEX c1 ON t1 (c1);
MariaDB [tendb_test]> ALTER TABLE t1 DROP INDEX c1;
MariaDB [tendb_test]> show create table t1;

CREATE TABLE `t1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `c1` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```




### 2.5 MODIFY COLUMN
`ALTER TABLE .. MODIFY COLUMN` statement is used to modify the columns on the existing table, including the column data type and attribute.
```
MariaDB [tendb_test]>CREATE TABLE t1 (id int not null primary key AUTO_INCREMENT, col1 INT);
MariaDB [tendb_test]>show create table  t1;

CREATE TABLE `t1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `col1` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```

```
MariaDB [tendb_test]>ALTER TABLE t1 MODIFY col1 BIGINT;
MariaDB [tendb_test]>ALTER TABLE t1 MODIFY col1 INT;
MariaDB [tendb_test]>ALTER TABLE t1 MODIFY col1 BLOB;
MariaDB [tendb_test]>ALTER TABLE t1 MODIFY col1 BIGINT, MODIFY id BIGINT NOT NULL;
MariaDB [tendb_test]>show create table  t1;

CREATE TABLE `t1` (
  `id` bigint(20) NOT NULL,
  `col1` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```




## 3. ALTER DATABASE
`ALTER DATABASE` enables user to change the overall characteristics of a database. These characteristics are stored in the db.opt file in the database directory. This statement requires the ALTER privilege on the database. ALTER SCHEMA is a synonym for ALTER DATABASE. 

```
MariaDB [tendb_test]> ALTER DATABASE  tendb_test CHARACTER SET  utf8;
MariaDB [tendb_test]> SHOW CREATE DATABASE  tendb_test;
+----------------+---------------------------------------------------------------------+
| Database       | Create Database                                                     |
+----------------+---------------------------------------------------------------------+
| tendb_test     | CREATE DATABASE `tendb_test` /*!40100 DEFAULT CHARACTER SET utf8 */ |
+----------+---------------------------------------------------------------------------+
1 row in set (0.00 sec)
```






## 4. CREATE TABLE LIKE

`CREATE TABLE ... LIKE` is used to create an empty table based on the definition of another table, including any column attributes and indexes defined in the original table. And TSpider will process the comment of the corresponded partition.

```
MariaDB [tendb_test]>create table t1(c int primary key);
MariaDB [tendb_test]>show create table t1\G;

 CREATE TABLE `t1` (
  `c` int(11) NOT NULL,
  PRIMARY KEY (`c`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`c`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```

```
MariaDB [tendb_test]>create table t2 like t1;
MariaDB [tendb_test]>show create table t2;

 CREATE TABLE `t2` (
  `c` int(11) NOT NULL,
  PRIMARY KEY (`c`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`c`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t2", server "SPT0"' ENGINE = SPIDER,
PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t2", server "SPT1"' ENGINE = SPIDER,
PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t2", server "SPT2"' ENGINE = SPIDER,
PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t2", server "SPT3"' ENGINE = SPIDER)
```







#  2. TenDB Cluster Without Tdbctl

When ddl_execute_by_ctl=OFF, `truncate table` is allowed. But Tdbctl will not distribute ddl to the TenDB node, user needs to execute  ddl on TSpider and TenDB respectively.

## 1. Create Table 
Create table  on TSpider node ：  

```
 MariaDB [tendb_test]> CREATE TABLE `t1` (
  `c` int(11) NOT NULL,
  PRIMARY KEY (`c`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`c`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```

Create table  on TenDB node ：  
```
mysql> create table tendb_test_0.t1(c int primary key);
mysql> create table tendb_test_1.t1(c int primary key);
mysql> create table tendb_test_2.t1(c int primary key);
mysql> create table tendb_test_3.t1(c int primary key);
```




##  2. Create Table like 
 When execute `create table like` on the TSpider node, the comment information of each partition is ignored, so as to avoid misuse on TSpider node.

```
MariaDB [tendb_test]>create table t2 like t1;
MariaDB [tendb_test]>show create table t2\G;

CREATE TABLE `t2` (
  `c` int(11) NOT NULL,
  PRIMARY KEY (`c`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`c`) MOD 4)
(PARTITION `pt0` VALUES IN (0) ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) ENGINE = SPIDER)
```






