# XA事务优化

TenDB Cluster分布式事务有快速处理悬挂事务的能力，这一能力依赖TenDB提供的底层支持。本篇介绍TenDB在XA事务上的新功能。

## 悬挂事务发现

为保证悬挂事务（未能按预期COMMIT或者ROLLBACK的且处于PREPARED状态的事务）能够自动恢复，首先我们需要快速发现悬挂事务。    
通常我们会根据业务侧需求（事务最长执行时间）定义持续处于PREPARED状态（比如30秒）的事务，是需要主动干预的事务。但是官方MySQL并没有提供相关指令支持。    
为此TenDB实现指令`xa recover with time`，能够发现PREPARED状态事务及其处于PREPARED状态的持续时间。示例如下：    

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
 
通过`xa recover with time`，能够快速发现处于PREPARED状态的事务，及时对异常事务进行处理。

## 事务提交日志

悬挂事务是需要进行COMMIT还是ROLLBACK，完全依赖于该事务的其它兄弟节点是否有提交。为快速感知其它事务是否提交，TenDB扩展了一阶段提交指令使得一阶段提交能够在事务提交的同时写日志： `xa commit $xid one phase with logs`   
该指令让事务的提交时，将事务的`xid`写入系统表`mysql.xa_commit_logs`中。事务提交和写`xid`到`xa_commit_log`的操作是原子的，事务能提交表示`xid`一定有记`xa_commit_log`，`xid`没记录到`xa_commit_log`则表示事务就回滚了。 示例如下：

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
可以看到在`xid` '123'提交到， `mysql.xa_commit_log`有记录'123'。   
</br>    

由于提交的`xid`会写入到`mysql.xa_commit_log`中，为避免`mysql.xa_commit_log`写入数据量过大，TenDB采用了循环写的策略。`mysql.xa_commit_log`表定义如下：

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
系统参数`max_xa_commit_logs`控制`xa_commit_log`最多记录数，默认值为1000000， 超过这个值则从id为1重新开始覆盖写。