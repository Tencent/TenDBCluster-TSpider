<a id="jump"></a>

# 与单实例MySQL异同

TenDB Cluster集群主要是由多个接入层TSpider实例及存储TSpider通过分片规则打散后数据的TenDB存储实例构成。

虽然TSpider利用了spider这一MySQL引擎，能够兼容绝大多数MySQL标准用法，但还是有些用法不支持，或者和单实例MySQL使用存在差异。

Tendb Cluster和MySQL的区别总结如下表：

|    | MySQL   | Tendb Cluster   |
|:----|:----|:----|
| 多唯一键   | 支持   | 不支持   |
| 外键约束   | 支持   | 不支持   |
| 更改表的Shard key | 支持 | 不支持  |
| CREATE TABLE tblName AS SELECT stmt| 支持 | 不支持 |
| CREATE TEMPORARY TABLE   | 支持   | 不支持   |
| XA事务 | 支持 | 不支持 |
| SAVEPOINT | 支持 | 不支持 |
| 自增列 | 支持 | 支持(只保证自增ID的唯一性，不保证连续和递增)
| 存储过程/触发器/函数/视图   | 支持   | 支持(但不建议使用)   |
| KILL THREADS ALL/ALL SAFE| 不支持 | 支持 |
| KILL thread_id SAFE | 不支持 | 支持 |




下面详细进行描述。

<a id="jump1"></a>   

## 不支持特性
- 多唯一键
- 外键约束
- 修改shard key（分区键）
- 增加/删除主键
- CREATE TABLE tblName AS SELECT stmt语法
- CREATE TEMPORARY TABLE 
- XA事务
- SAVEPOINT

## 与MySQL有差异的特性

<a id="jump21"></a>

### **DDL**
分别从CREATE TABLE和ALTER TABLE来说明和单实例MySQL的部分差异
#### CREATE TABLE
用户建一个InnoDB表，建表SQL如下：
```
CREATE TABLE `t1` (
  `c1` int(11) NOT NULL,
  `c2` int(11) DEFAULT NULL,
  PRIMARY KEY (`c1`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
```
上述建表SQL会转发到中控节点Tdbctl，再由Tdbctl分发各个接入层TSpider节点和各个存储节点TenDB。在用户得到建表成功的反馈后， 执行show create table展示如下：
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
show create table得到的结果和使用单实例MySQL不一样，展现给用户的一个对PRIMARY KEY字段进行分区的分区表，且存储引擎是spider。但用户可以完全像访问InnoDB表一样对TSpider上的表t1进行增删改查。
此时，各存储实例TenDB分别有库tendb_test_0、tendb_test_1、tendb_test_2、tendb_test_3，每个库下都有一个t1表。
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

TenDB Cluster认为适当将一些细节暴露给用户，有助于用户理解集群内在逻辑，从而更合理有效的使用集群。

#### ALTER TABLE

如上在TenDB Cluster中建一个InnoDB表，会转换为spider引擎的分区表。作为分区表，那么需要额外遵守分区表的规则，其中一点就是分区字段必须是唯一索引的一部分。因此下面增加唯一键的SQL不能支持：
```
mysql> alter table t1 add unique(c2);
ERROR 1503 (HY000): A UNIQUE INDEX must include all columns in the table's partitioning function
```
但是如果增加的唯一键包含有分区字段，则可以成功。
```
mysql> alter table t1 add unique(c1,c2);
Query OK, 0 rows affected (0.01 sec)
```
这个限制产生的原因是分区表无法保证非分区键的唯一约束，在各个分区的数据汇集后还是唯一的。 同理，TenDB Cluster中每个分区会对应一个TenDB存储实例，TenDB Cluster也无法保证各个实例的数据汇集后的唯一性。 

<a id="jump22"></a>

### **存储引擎**

在TenDB Cluster上建表使用InnoDB/MyISAM或者其它，都会在接入层TSpider上转换为spider引擎；实际存储数据的TenDB实例会按用户要求使用存储引擎。

<a id="jump23"></a>

### **自增列**

TenDB Cluster只保证自增ID的唯一性，不保证连续和递增。   
TenDB Cluster集群中一个表会被TSpider将数据打散分布在多个TenDB存储实例中；且集群中存在多个平行的TSpider节点同时读写这个表。   和单实例MySQL中的自增列中不同的地方是， 需多TSpider要维护多个TenDB实例中同一自增列的值。   
如果要保证和单实例MySQL中自增列一样的特性， 需要各个TSpider节点在写自增字段时都要对全部表（全部TenDB实例中的表）加锁、或者使用第三方资源作为临界资源唯一维护自增属性。 这两种方案都会有很大的性能损耗或者资源开销，不适合在工程环境使用。   
TenDB Cluster集群自增的策略是，让每一个TSpider节点维护各自的自增字段信息，且保证某个TSpider节点生成的自增值一定和其它TSpider节点生成的自增值不同。 因为各个节点各自独立维护自增字段，因此无法保证集群维度的自增字段的连续递增，但是维护自增字段的成本最低，写效率最高。   
集群中每个TSpider节点中有3个参数用来维护自身自增信息：
> spider_auto_increment_mode_switch: 表示是否启用TSpider自增列功能   
  spider_auto_increment_step：表示TSpider的自增步长  
  spider_auto_increment_mode_value：表示TSpider自增字段值求模spider_auto_increment_step后的余数

启用TSpider自增列后，假定spider_auto_increment_step为17，spider_auto_increment_mode_value为3；那么该TSpider节点生成的自增序列就为3、3+17、 3+17+17 ...   
> 根据集群自增列特性，在spider_auto_increment_mode_switch相同的前提下，集群中各个TSpider需要维护相同spider_auto_increment_step和不同的spider_auto_increment_mode_value。

<a id="jump24"></a>

### **查询计划**

TenDB Cluster的接入层TSpider更关注如何高效的将用户请求转发到后端存储节点，不关注该SQL在原InnoDB会走什么样的执行计划。因此如果在TSpider上用explain查看执行计划，会发现基于主键的查询也会走全表扫描，t1表的主键为c1，explain对应的结果如下：

```
mysql> explain select * from t1 where c1=1;
+------+-------------+-------+------+---------------+------+---------+------+------+-----------------------------------+
| id   | select_type | table | type | possible_keys | key  | key_len | ref  | rows | Extra                             |
+------+-------------+-------+------+---------------+------+---------+------+------+-----------------------------------+
|    1 | SIMPLE      | t1    | ALL  | NULL          | NULL | NULL    | NULL |    2 | Using where with pushed condition |
+------+-------------+-------+------+---------------+------+---------+------+------+-----------------------------------+
```

<a id="jump25"></a>

### **事务**

TenDB Cluster支持事务（但不支持应用使用XA事务，不支持savepoint）。   
默认配置下，接入层TSpider只有在接受到应用的显示事务请求，才会以`start transaction/commit`的显示事务方式与存储层TenDB进行交互，这样仅能保证单分片上的事务特性。 在要求数据强一致的场景下，可以通过启用参数`spider_internal_xa`让TSpider使用XA协议和存储层TenDB进行交互，从而保证分布式场景下的事务。   

更多关于TenDB Cluster事务详细说明见章节 [事务](transaction.md)

<a id="jump26"></a>

### **存储过程、触发器**
不建议在TenDB Cluster中使用复杂的存储过程和触发器。   
在对数据一致性高要求的场景中使用存储过程或者触发器，建议配置分布式事务；否则TenDB Cluster只能支持单机事务。 