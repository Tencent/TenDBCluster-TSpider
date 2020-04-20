# XA Transaction Optimization

TenDB Cluster has the ability to quickly process suspended transactions when dealing with distributed transactions, which relies on the support of TenDB internals. This article introduces the new features of TenDB in regards to XA transactions.

## Suspended Transactions Detect

In order to recover suspended transactions (transactions which fail to COMMIT or ROLLBACK and is in PREPARED state), quick detection of them is of greate importance.

Normally, we define a transaction which continuously in the PREPARED state for a certain time, like 30 seconds, to be a transaction that requires active intervention. The specific time can be determined by the application.     

TenDB provides a statement `xa recover with time`，which can detect PREPARED state transactions and their duration in the PREPARED state. Examples are as follows:    

```sql
mysql> xa recover;
+----------+--------------+--------------+------+
| formatID | gtrid_length | bqual_length | data |
+----------+--------------+--------------+------+
|        1 |            3 |            0 | 123  |
+----------+--------------+--------------+------+
1 row in set (0.00 sec)

mysql> xa recover with time;
+----------+--------------+--------------+------+---------------------+
| formatID | gtrid_length | bqual_length | data | prepare_time        |
+----------+--------------+--------------+------+---------------------+
|        1 |            3 |            0 | 123  | 2020-03-09 12:03:32 |
+----------+--------------+--------------+------+---------------------+
1 row in set (0.00 sec)
```
 
By using `xa recover with time`，we are able to quickly detect those PREPARED transactions and handle them if needed.

## Commit Logs

Whether a transaction needs COMMIT or ROLLBACK depends on whether other sibling nodes of transactions are commited. 

In order to quickly detect whether other transactions are committed, TenDB provides a one-phase commit instruction so that it can write logs at the same time as the transaction commit: `xa commit $xid one phase with logs`

This instruction make the transaction's xid to be writtent to the system table `mysql.xa_commit_logs`.
Both committing transactions and writting `xid` into `xa_commit_log` is atomic. If the transaction can be commited, then `xid` must have been recorded into  `xa_commit_log`. If `xid` is not recorded in `xa_commit_log`, the transaction is rolled back. Examples are as follows:

```sql 
mysql> xa start '123';
Query OK, 0 rows affected (0.00 sec)

mysql> insert into t1 values(1,'a');
Query OK, 1 row affected (0.00 sec)

mysql> xa end '123';
Query OK, 0 rows affected (0.00 sec)

mysql> xa commit '123' one phase with logs;
Query OK, 0 rows affected (0.00 sec)

mysql> select * from mysql.xa_commit_log;
+----+-----+---------------------+
| id | xid | commit_time         |
+----+-----+---------------------+
|  1 | 123 | 2020-03-09 15:04:23 |
+----+-----+---------------------+
1 row in set (0.00 sec)
```
The transaction `xid` '123' has been recorded into the `mysql.xa_commit_log`. 
</br>    

Since the submitted `xid` will be written to` mysql.xa_commit_log`, in order to avoid the excessive write amount of `mysql.xa_commit_log`, TenDB write it in a circular fashion. The `mysql.xa_commit_log` table is defined as follows:

```sql 
mysql> show create table mysql.xa_commit_log\G
*************************** 1. row ***************************
       Table: xa_commit_log
Create Table: CREATE TABLE `xa_commit_log` (
  `id` bigint(20) NOT NULL,
  `xid` char(128) NOT NULL,
  `commit_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `kx` (`xid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 STATS_PERSISTENT=0
1 row in set (0.00 sec)
```
The system variable `max_xa_commit_logs` controls the maximum number of records in` xa_commit_log`. The default value is 1000000. If this value is exceeded, the overwrite will start from id 1.