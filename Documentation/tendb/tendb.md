# TenDB简介
TenDB作为TenDB Cluster集群的存储节点，承担着数据存储的重要作用。    
TenDB是基于社区Percona版本高度定制的MySQL，目前支持MySQL 5.5，5.6，5.7，在完全兼容源生MySQL的基础上，开发集成了一些实用的功能特性，无论是集群应用还是单机使用，TenDB都是不错的选择。  
相比Percona已有的特性，TenDB支持binlog压缩，binlog限速，在线加字段，blob压缩，XA事务优化等。这些特性使得TenDB可以胜任在实际应用时常会遇到的痛点，下文将会着重介绍这些重要的特性。


[在线加字段](instant-add-column.md)

[大字段压缩](blob-compress.md)

[binlog压缩](binlog-compress.md)

[binlog限速](binlog-speed-limit.md)

[XA事务优化](xafeatures.md)

[语法分析工具](tmysqlparse.md)
