## TenDB Cluster Added Grammer
TSpider has added some grammer, the following will introduce how to use.

### Kill Thread
Three ways to kill threads:    
1). kill thread_id safe(kill 9 safe)  
If  the `thread id` is in sleep condition and not in transaction, then can be killed. Otherwise print error:  ER_THREAD_IN_USING_OR_TRANSACTION , "thread %lu is in using or in transaction, can't be killed safe".   
2). kill threads all  
  Kill all threads safely. The threads which are in sleep condition and not in transaction, can be killed. The others should be killed until query or transaction ended.  
3). kill threads all force   
kill all threads in the TSpider node.  

Add a session status: `Max_thread_id_on_kill`, which means the current max `thread id` when execute `kill threads all (force)`.    

Examples are as follows:
```
MariaDB [tendb_test]>  kill threads all force;
Query OK, 1 row affected (0.00 sec)

MariaDB [tendb_test]> show status like 'Max_thread_id_on_kill';
+-----------------------+--------+
| Variable_name         | Value  |
+-----------------------+--------+
| Max_thread_id_on_kill | 171010 |
+-----------------------+--------+
1 row in set (0.00 sec)
```

### FLUSH TABLE WITH WRITE LOCK
<a id="write_lock"></a>
For the master-slave switch in distributed transaction scenario， `flush tables with read lock` can't make sure the the data consistency. So Tencent Game DBA add feature `flush tables with write lock` to get transaction lock  make sure the data consistency. 

1. After execute `flush  table with write lock` on TSpider node, in the period of applying for transaction lock, the start of new transaction will be blocked.  
2. After all the exist transactions committed, the above SQL will get transaction lock. Then user can modify mysql.servers and flush privileges.    
3. Execute `unlock tables` to release transaction lock. After unlcok, application can send request to TSpider node.  At this time, the new connection on the TSpider node will use the new sharding rules to request the TenDB node and switch completed.

<font color="#dd0000">Note</font>   
>This feature only takes effect when spider_internal_xa = ON.

### Cross-shard Scanning Information Query
Similar to percona, TSpider use QUERY_RESPONSE_TIME to count  the number of queries and the total time cost of the request time in a certain interval. In addition, TSpider additionally counts the response time of cross-shard operations. 
The view of Cross-shard depends on two columns: P_COUNT, P_TOTAL.  P_COUNT means the number of cross-shard requests and P_TOTAL means the time spent on Cross-shard requests.


User can get the response time view in the following ways:
```
MariaDB [mysql]> show QUERY_RESPONSE_TIME;
+----------------+-------+-----------------------+---------+-----------------------+
| TIME           | COUNT | TOTAL                 | P_COUNT | P_TOTAL               |
+----------------+-------+-----------------------+---------+-----------------------+
|       0.000015 | 70865 |       0.126465        |       0 |       0.000000        |
|       0.000030 |  7352 |       0.202521        |       0 |       0.000000        |
|       0.000061 | 86036 |       4.340680        |       0 |       0.000000        |
|       0.000122 | 45729 |       3.788752        |       0 |       0.000000        |
|       0.000244 |   342 |       0.047792        |       0 |       0.000000        |
|       0.000488 |    42 |       0.016067        |       6 |       0.002419        |
|       0.000976 |    71 |       0.042179        |       0 |       0.000000        |
|       0.001953 |     5 |       0.008613        |       0 |       0.000000        |
|       0.003906 |    19 |       0.051909        |       0 |       0.000000        |
|       0.007812 |     1 |       0.006720        |       0 |       0.000000        |
|       0.015625 |     6 |       0.075120        |       1 |       0.014913        |
|       0.031250 |     3 |       0.074285        |       1 |       0.022601        |
|       0.062500 |     6 |       0.244472        |       0 |       0.000000        |
.............
| TOO LONG       |     0 | Since 200316 14:57:02 |       0 | Since 200316 14:57:02 |
+----------------+-------+-----------------------+---------+-----------------------+
```

User can  also get the response time from information_schema.QUERY_RESPONSE_TIME:
```
MariaDB [mysql]> SELECT * FROM information_schema.QUERY_RESPONSE_TIME;
+----------------+-------+-----------------------+---------+-----------------------+
| TIME           | COUNT | TOTAL                 | P_COUNT | P_TOTAL               |
+----------------+-------+-----------------------+---------+-----------------------+
|       0.000015 | 70865 |       0.126465        |       0 |       0.000000        |
|       0.000030 |  7352 |       0.202521        |       0 |       0.000000        |
|       0.000061 | 86036 |       4.340680        |       0 |       0.000000        |
|       0.000122 | 45729 |       3.788752        |       0 |       0.000000        |
|       0.000244 |   342 |       0.047792        |       0 |       0.000000        |
|       0.000488 |    42 |       0.016067        |       6 |       0.002419        |
|       0.000976 |    71 |       0.042179        |       0 |       0.000000        |
|       0.001953 |     5 |       0.008613        |       0 |       0.000000        |
|       0.003906 |    19 |       0.051909        |       0 |       0.000000        |
|       0.007812 |     1 |       0.006720        |       0 |       0.000000        |
|       0.015625 |     6 |       0.075120        |       1 |       0.014913        |
|       0.031250 |     3 |       0.074285        |       1 |       0.022601        |
|       0.062500 |     6 |       0.244472        |       0 |       0.000000        |
.............
| TOO LONG       |     0 | Since 200316 14:57:02 |       0 | Since 200316 14:57:02 |
+----------------+-------+-----------------------+---------+-----------------------+
```
It is clear that there are 42 SQL which  response time between 0.000244 seconds and 0.000488 seconds, the total response time is 0.016067 seconds; And  there are 6 Cross-shard  SQL, the total response time is  0.002419 seconds.