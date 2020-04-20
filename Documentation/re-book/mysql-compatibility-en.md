<a id="jump"></a>

# Similarities and Differences with MySQL
A TenDB Cluster is made up of multiple proxy layer TSpiders, and data layer TenDB instances, which contain distributed data by sharding rules.

Although the spider engine used by TSpider is a MySQL engine, and is able to support most of the standard MySQL commands, there are several unsupported commands. Some features may differ to MySQL.

The differences are summarized in the below table:

|    | MySQL   | Tendb Cluster   |
|:----|:----|:----|
| Multiple unique keys   | Supported   | Unsupported   |
| Foreign key constraint   | Supported   | Unsupported   |
| Change table shard key | Supported   | Unsupported  |
| CREATE TABLE tblName AS SELECT| Supported   | Unsupported |
| CREATE TEMPORARY TABLE   | Supported      | Unsupported   |
| XA transaction | Supported    | Unsupported |
| SAVEPOINT | Supported   | Unsupported |
| Auto-increment column | Supported   | Supported   (TSpider only guarantees uniqueness of auto-increment columns, and not guarantees that they are continuous and increment globally)
| Stored Procedure/Trigger/Function/View   | Supported   | Supported (But not suggested)   |
| KILL THREADS ALL/ALL SAFE| Unsupported | Supported |
| KILL thread_id SAFE | Unsupported | Supported |

More details are discussed as below:

<a id="jump1"></a>  

## Unsupported Features
- Multiple unique keys
- Foreign key constraint
- Change table shard key
- Add/delete primary key
- CREATE TABLE tblName AS SELECT statement
- CREATE TEMPORARY TABLE
- XA Transactions
- SAVEPOINT

## Features Differs to MySQL

<a id="jump21"></a>

### **DDL**

Differences between TSpider and MySQL are discussed in CREATE TABLE AND ALTER TABLE statements, respectively as below

#### CREATE TABLE
Create an innodb table with below SQL statement:
```
CREATE TABLE `t1` (
  `c1` int(11) NOT NULL,
  `c2` int(11) DEFAULT NULL,
  PRIMARY KEY (`c1`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
```

Above SQL will be sent to control node Tdbctl, then distributed by Tdbctl to proxy layer TSPider nodes and data layer TenDB nodes. After user got creation success confirmation, issue show create table will get below output:

```
mysql> show create table t1\G
********************* 1. row**********************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `c1` int(11) NOT NULL,
  `c2` int(11) DEFAULT NULL,
  PRIMARY KEY (`c1`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8mb4
 PARTITION BY LIST (crc32(`c1`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
1 row in set (0.00 sec)
```

Unlike MySQL, show create table yields a table definition partitioned by primary key, and the storage engine is spider. Users are able to access the t1 table and perform INSERT/DELETE/UPDATE/SELECT operations in TSpider like in InnoDB.

After creation, database tendb_test_0, tendb_test_1, tendb_test_2, and tendb_test_3 will be created on TenDB nodes  respectively. And each database has a t1 table.

```
mysql> show create table tendb_test_0.t1\G
********************* 1. row *********************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `c1` int(11) NOT NULL,
  `c2` int(11) DEFAULT NULL,
  PRIMARY KEY (`c1`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
1 row in set (0.00 sec)
```
TenDB Cluster exposed appropriate inner details to user, which may help user to understand TSpider better thus to utilize it better.

#### ALTER TABLE
As demonstrated above, An InnoDB table created in TenDB Cluster will be transformed into a partitioned table using spider engine. As a partitioned table, certain rules should be followed, such as partition key must be part of a unique index. Therefore below SQL that adds unique key will not be supported:
```
mysql> alter table t1 add unique(c2);
ERROR 1503 (HY000): A UNIQUE INDEX must include all columns in the table's partitioning function
```
However, if the new unique key contains the partition key, the alter table statement can be accepted.
```
mysql> alter table t1 add unique(c1,c2);
Query OK, 0 rows affected (0.01 sec)
```
The consideration behind this constraint is that partitioned tables can not make sure that the uniqueness still holds after merging data from other partitions. For the same reason, Each partitioned table corresponds to one TenDB storage instance, and TenDB Cluster can not guarantee data uniqueness after merging data from instances neither.

<a id="jump22"></a>

### **Storage Engine**
Tables created in TenDB Cluster with InnoDB/MyISAM/etc. will be transformed into spider engine unanimously, by TSpider proxy layer. Whereas the TenDB instance who stores the actual data will use the storage engine specified by users.

<a id="jump23"></a>

### **Auto_increment Column**

TenDB Cluster only guarantees the uniqueness of the auto-increment column,  and not guarantees that they are continuous and increment globally.  
In TenDB Cluster, a table will be distributed in multiple TenDB storage instances, and multiple TSpider nodes will read and write this table equally and simultaneously. Differing from single instance MySQL, an auto-increment column needs to be maintained by multiple TSpider nodes together.

To achieve the same behavior with that in single instance MySQL, All tables in TenDB Cluster must be locked when TSpider nodes update auto-increment column, or use third-party resource as a critical resource to ensure the uniqueness. Both solutions will introduce noticeable performance overhead, therefore it is not suitable for production environments.

The implementation strategy of auto-increment in TenDB Cluster is, to let each TSpider node maintain its own auto-increment column, and to make sure that the increment generated in each TSpider node is different to that generated in other nodes. Due to this autonomy, it can not be guaranteed that the auto-increment column is continuous and incremental in Cluster aspect, however, this approach has low cost in maintaining auto-increment column and high efficiency during updating them.   
In a word, TSpider will generate a global non-continuous unique identity for auto-increment.

There are 3 parameters in each TSpider related to auto-increment column:
> spider_auto_increment_mode_switch: Whether the auto-increment is enabled. The value can be 0 (or OFF) to disable or 1 (or ON) to enable. If on, TSpider will generate a global non-continuous unique identity for new rows. Identity only ensure incremental on the same TSpider node.    
> spider_auto_increment_step: The step of the global non-continuous unique identity generated by TSpider node.  All TSpider nodes must be the same.    

> spider_auto_increment_mode_value: TSpider node generate global non-continuous unique identity's start number. All TSpider's value must be different. Valid value can be computed by TSpider's increment value modulo spider_auto_increment_step.     

With auto-increment column enabled, given spider_auto_increment_step size of 17, and spider_auto_increment_mode_value=3, then the auto-increment sequence generated on that TSpider node will be 3, 3+17, 3+17+17, ...
> According to above, with the same spider_auto_increment_mode_switch setting, TSpider nodes within a cluster should have the same spider_auto_increment_step and different spider_auto_increment_mode_value.

<a id="jump24"></a>

### **Query Plan**
Proxy layer of TenDB Cluster focuses on how to forward users requests to backend storage efficiently, instead of how the SQL will be executed on the storage node. Therefore when reviewing a query plan on TSpider with explain, you may find even a query with a primary key can request a full table scan, in below example, where t1 table have a primary key c1, and explain results as followed:

```
mysql> explain select * from t1 where c1=1;
+------+-------------+-------+------+---------------+------+---------+------+------+-----------------------------------+
| id   | select_type | table | type | possible_keys | key  | key_len | ref  | rows | Extra                             |
+------+-------------+-------+------+---------------+------+---------+------+------+-----------------------------------+
|    1 | SIMPLE      | t1    | ALL  | NULL          | NULL | NULL    | NULL |    2 | Using where with pushed condition |
+------+-------------+-------+------+---------------+------+---------+------+------+-----------------------------------+
```

<a id="jump25"></a>

### **Transaction**
TenDB Cluster supports transcations, but does not support XA transactions and savepoints.  
With default configuration, the proxy layer (TSpider) only interacts with the data layer (TenDB) using explicit transcations (starts with `start transaction` and ends with `commit`) when it receives the explicit transaction request from applications. Under this circumstance, only single shard is transactional.  
In the scenarios where strict consistency is required, the variable `spider_internal_xa` should be enabled, which can enable distributed transactions. 
 
For more details of transactions in TenDB Cluster see chapter [Transactions](transaction-en.md)

<a id="jump26"></a>

### **Stored Procedure and Trigger**
It's not suggested to use complex stored procedures or triggers.
In scenarios where strong data consistency is required, it's suggested to use distributed transactions. Otherwise only local transactions will be supported.
