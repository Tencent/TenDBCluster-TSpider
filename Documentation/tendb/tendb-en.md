# TenDB简介
As the storage node of TenDB Cluster, TenDB bears the important role of data storage.  

TenDB is highly customized MySQL based on Percona, which is compatible with MySQL 5.5, 5.6 and 5.7. On the basis of full compatibility with the MySQL, it has developed and integrated some practical functions and features. It is of great performance both in cluster applications and stand-alone use.  

Besides features Percona has, TenDB supports:   
binlog compression (Contributed to MariaDB: https://mariadb.com/kb/en/changes-improvements-in-mariadb-102/)   
instant add column (Merged into MySQL 8.0: https://dev.mysql.com/worklog/task/?id=11250)   
big column compression (Contributed to MariaDB: https://mariadb.com/kb/en/mariadb-1032-release-notes/)    
And other features like binlog speed limit, XA transaction optimization, etc. These features make TenDB capable of handling some pain points often encountered in practical applications, which will be described in the following articles.
 

[Instant Add Column](instant-add-column-en.md)

[Big Column Compression](blob-compress-en.md)

[Binlog Compression](binlog-compress-en.md)

[Binlog Speed Limiter](binlog-speed-limit-en.md)

[XA Transactions Optimization](xafeatures-en.md)

[SQL Syntax Analysis Tool](tmysqlparse-en.md)
