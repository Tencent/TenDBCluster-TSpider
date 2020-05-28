# TenDB简介
TenDB作为TenDB Cluster集群的存储节点，承担着数据存储的重要作用。    
TenDB是基于社区Percona Server版本高度定制的MySQL，目前支持MySQL 5.5，5.6，5.7，在完全兼容源生MySQL的基础上，开发集成了一些实用的功能特性，无论是集群应用还是单机使用，TenDB都是不错的选择。    
相比Percona已有的特性，TenDB支持:   
binlog压缩（已贡献到MariaDB: https://mariadb.com/kb/en/changes-improvements-in-mariadb-102/）;   
在线加字段（已合并到MySQL 8.0: https://dev.mysql.com/worklog/task/?id=11250）   
大字段压缩（已贡献到MariaDB: https://mariadb.com/kb/en/mariadb-1032-release-notes/）    
还有binlog限速、XA事务优化等。这些特性使得TenDB可以胜任在实际应用时常会遇到的痛点，下文将会着重介绍这些重要的特性。


[在线加字段](instant-add-column.md)

[大字段压缩](blob-compress.md)

[binlog压缩](binlog-compress.md)

[binlog限速](binlog-speed-limit.md)

[XA事务优化](xafeatures.md)

[语法分析工具](tmysqlparse.md)
