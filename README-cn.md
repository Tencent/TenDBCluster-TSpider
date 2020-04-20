# TenDB Cluster
TenDB Cluster是腾讯DBA团队提供的MySQL分布式关系型数据库解决方案，主要包括三个核心组件：TSpider，TenDB 、Tdbctl。   
TSpider是TenDB Cluster的接入层，是腾讯游戏DBA基于MariaDB 10.3.7开发定制的版本，主要完善并定制spider这一分布式MySQL存储引擎；TSpider [github地址](https://github.com/TencentDBA/TendbCluster3)  
TenDB是TenDB Cluster的存储层，是腾讯游戏DBA基于Percona 5.7.20开定制的MySQL分支，主要提供一些更贴近业务运营的基础能力； TenDB [github地址](https://github.com/TencentDBA/TenDB3)  
Tdbctl是集群的中控节点，提供集群路由管理、集群变更、集群监控等能力。Tdbctl [github地址](https://github.com/TencentDBA/Tdbctl)  
TenDB Cluster Operator则提供在主流云基础设施（Kubernetes）上部署管理TenDB Cluster集群的能力。TenDB Cluster Operator [github地址]()  

## TenDB Cluster简介
TenDB Cluster是腾讯游戏DBA团队提供的MySQL分布式关系型数据库解决方案，主要特点包括：透明分库分表、高可用的MySQL集群服务，透明及在线的扩容及缩容；使得开发者可以仅专注于业务逻辑的开发及运营，无需编写数据分片逻辑，在海量用户并发情况下，也无须关心DB存储层的负载压力。

## 快速体验
> docker-compose使用方式

## [使用文档](Documentation/SUMMARY.md)
具体见[参考手册](Documentation/SUMMARY.md)

## 联系我们
对TenDB Cluster的任何问题或者意见，欢迎通过提issue的方式反馈：   
TSpider:  https://github.com/TencentDBA/TendbCluster3/issues   
TenDB:    https://github.com/TencentDBA/TenDB3/issues   
Tdbctl:   https://github.com/TencentDBA/Tdbctl/issues 