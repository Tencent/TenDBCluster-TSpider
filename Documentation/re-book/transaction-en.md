# Transactions in TenDB Cluster

<a id="usage"></a>

TenDB Cluster supports transcations, but does not support XA transactions and savepoints.  
With default configuration, the proxy layer (TSpider) only interacts with the data layer (TenDB) using explicit transcations (starts with `start transaction` and ends with `commit`) when it receives the explicit transaction request from applications. Under this circumstance, only single shard is transactional.  
In the scenarios where strict consistency is required, the variable `spider_internal_xa` should be enabled, which can enable distributed transactions. 
 
Details of transactions in TenDB Cluster are described blow.

<a id="jump2"></a>

## How to Use Transactions in TenDB Cluster

The way to use transactions in TenDB Cluster is exactly the same as it is in MySQL.
```sql 
begin/start transaction/set autocommit=0;
dml;
commit/rollback;
```
If trasaction is not explicity specified in the applications, then TSpider will use transactions implicitly. In addition, TSpider does not support the use of XA transactions in the application side, nor does it support savepoints.

After receiving the transaction request from the application layer, TSpider will forward the transaction to the corresponding data node(TenDB) for execution. TSpider provides two types of transactions to interact with the data node(TenDB), namely *common transactions* and *distributed transactions*, which will be explained respectively below.

<a id="jump3"></a>

## Common Transactions

With default configuration, TSpider will implicitly forward transactions to the applications. For example, the query will be forwarded as a query, and the update will be forwarded as an update to each shard.  
Here is an example of updating based on non-shard_key:

Structure of the table is:
```sql 
mysql> show create table t1\G
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `c1` int(11) NOT NULL,
  `c2` int(11) DEFAULT NULL,
  `c3` int(11) DEFAULT NULL,
  PRIMARY KEY (`c1`),
  KEY `k2` (`c2`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8mb4
 PARTITION BY LIST (crc32(`c1`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```
The request executed on TSpider is:
```sql 
update t1 set c3=c3+1 where c2=1;
```
The requests which TSpider distributed to data node are:
```sql 
update `tendb_test_0`.`t1` set `c3` = (`c3` + 1) where (`c2` = 1)
update `tendb_test_1`.`t1` set `c3` = (`c3` + 1) where (`c2` = 1)
update `tendb_test_2`.`t1` set `c3` = (`c3` + 1) where (`c2` = 1)
update `tendb_test_3`.`t1` set `c3` = (`c3` + 1) where (`c2` = 1)
```

With the same table structure, if the application uses explicit transactions, TSpider will forward them in the manner of displaying transactions. Examples are as follows:  
The execution request on TSpider:
```sql 
begin;
update t1 set c3=c3+1 where c2=1;
commit;
```
The requests which TSpider distributed to storage instances are:
```sql 
begin // for SPT0
update `tendb_test_0`.`t1` set `c3` = (`c3` + 1) where (`c2` = 1)
begin // for SPT1
update `tendb_test_1`.`t1` set `c3` = (`c3` + 1) where (`c2` = 1)
begin // for SPT2
update `tendb_test_2`.`t1` set `c3` = (`c3` + 1) where (`c2` = 1)
begin // for SPT3
update `tendb_test_3`.`t1` set `c3` = (`c3` + 1) where (`c2` = 1)
commit // for SPT0
commit // for SPT1
commit // for SPT2
commit // for SPT3
```

The way common transactions forward can only guarantee the transaction on single storage instance, rather than transactions on the entire cluster. Therefore, the common transactions can be applied to scenarios where strict consistency is not required.  
In addition, the variable `spider_with_begin_commit` can implicitly convert implicit transactions to explicit transactions when interacting with storage nodes.

<a id="jump4"></a>

## Distributed Transactions

If the atomicity and consistency is strictly required, then we recommend the use of distributed transactions of the cluster. To use it, `SPIDER_INTERNAL_XA` should be enabled. After enabling this, TSpider will interact with storage instances using the XA protocol to ensure distributed transactions. An example is shown below:  
On the application side：
```sql 
begin; 
insert into t1(c1, c2) values(1, 2);
update t2 set c2=3 where c1=2;
commit;
```
TSpider converts the above transactions and uses XA protocol to interact with TenDB; the corresponding SQL sequence is as follows: (table t1 has two shards)
```sql 
TenDB1: xa start xid;
TenDB1: insert into rm1.t1(c1,c2) values(1,2); 
TenDB2: xa start xid;
TenDB2: update rm2.t2 set c2=3 where c1=2; 
TenDB1: xa end xid; xa prepare xid; 
TenDB2: xa end xid; xa prepare xid; 
TenDB1: xa commit xid one phase with logs; 
TenDB2: xa commit xid;
```
After the application starts the transaction and the first request is sent (like the `insert` above), TSpider will send `xa start` to the backend, and execute the insertion at the nodes which are routed; every subsequent request (like the `update` abouve) will be sent to relevent TenDB node. When `commit` or `rollback` is received from the application, TSpider will send `xa end` or `xa prepare` to pre-commit the transaction and then use `xa commit` to commit it.

<a id="jump41"></a>

### **`xa commit xid one phase with logs`**

When the transaction above is commited，TenDB1 execute a statement `xa commit xid one phase with logs`; This is a extension TenDB implemented, which is used to commit XA transactions and record `xid` into `mysql.xa_commit_log`.

**Why xid Should be Recorded?**

This is because XA transaction can be failed due to some software and hardware exceptions, which would make `xa prepare` don't know how to handle it. A `xa_commit_log` can records suspended transactions. Therefore, transactions in `xa_commit_log` should be commited, others should be rolled back.

<a id="jump42"></a>

### **Variable `spider_trans_rollback`**

The definition of Atomicity in Databases is：
> An atomic transaction is an indivisible and irreducible series of database operations such that either all occur, or nothing occurs.

However, subsequent transactions in InnoDB will continue to commit if some previous SQL fails.
```sql 
mysql> select * from t1;
+----+------+
| c1 | c2   |
+----+------+
|  1 | a    |
|  2 | b    |
|  3 | c    |
+----+------+
3 rows in set (0.00 sec)

mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> insert into t1 values(4,'d');
Query OK, 1 row affected (0.00 sec)

mysql> update t1 set c2='aaa' where c3=1;
ERROR 1054 (42S22): Unknown column 'c3' in 'where clause'
mysql> commit;
Query OK, 0 rows affected (0.00 sec)

mysql> select * from t1;
+----+------+
| c1 | c2   |
+----+------+
|  1 | a    |
|  2 | b    |
|  3 | c    |
|  4 | d    |
+----+------+
4 rows in set (0.00 sec)
```
If the above transaction is executed in TenDB Cluster, and the `update` query failed. It can be caused by the reason that, in the backend, execution were successful in some shards but also failed in some other shards. Under this circumstance, if we follow the InnoDB transactions' rules, where subsequent statements can be commited, then it is likely to cause certain statement (like the `update` above) to be partially successful.  

Therefore, if the atomicity of the transaction is strictly followed, the entire transaction cannot continue to execute after one of the above SQL execution failures, and directly rolls back.

When the parameter `spider_trans_rollback` is turned on, that is, if a SQL request fails in the transaction, the transaction is automatically rolled back; thus automicity can be guaranteed.

<a id="jump43"></a>

### **Results Return from Transactions**

There are three results can be returned from TenDB Cluster，which are **success**，**fail**，**timeout**。   
**success** means that the current transaction is fully executed;  
**fail** means that the current transaction has failed and the data has not been modified;  
**timeout** suggests that the current transaction may be successful or it may need to be actively confirmed by the application.

<a id="jump44"></a>

### **Suspended Transactions Handling**

You can use TenDB's extension `xa recover with time` to watch transactions which in prepared state and the current duration of prepared state.  
TenDB Cluster defines a suspended transaction that needs to be intervened when its prepare state lasted for 30 seconds. Suspended transactions need to be submitted or rolled back.   
There will be a daemon in each Tdbctl to detect whether there is a transaction in prepared state for more than 30 seconds in TenDB in the current cluster; it only detects the existence of suspended transactions. If it finds one, it will check whether the suspended transaction is the `xa_commit_log`, if true it will execute `XA COMMIT xid`, otherwise, it will execute `XA ROLLBACK xid`.  
The flowchart of suspended transaction processing is as follows:

![pic](../pic/xarecover.png)


<a id="jump5"></a>

## Miscellaneous

### Ignore `autocommit=0`

Some applications may unintentionally use the default `aotocommit = 0` (unexpected) in the framework. Through the global variable `spider_ignore_autocommit`, after the transaction is being commited, it will set `autocommit = 1` to make subsequent queries commit automatically.
> Turn on SPIDER_IGNORE_AUTOCOMMIT, which would ignore set autocommit = 0 distributed in the application;


### `Save_point` is Not Supported

```
MariaDB [test1]> create table t1(c1 int primary key);
Query OK, 0 rows affected (0.03 sec)

MariaDB [test1]> begin;
Query OK, 0 rows affected (0.00 sec)

MariaDB [test1]> insert into t1 values(1);
Query OK, 1 row affected (0.00 sec)

MariaDB [test1]> savepoint a;
ERROR 1178 (42000): The storage engine for the table doesn't support SAVEPOINT

```