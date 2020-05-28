# TenDB Cluster
TenDB Cluster是腾讯游戏CROS DBA团队提供的MySQL分布式关系型数据库解决方案，主要包括三个核心组件：TSpider，TenDB 、Tdbctl。   
TSpider是TenDB Cluster的接入层，是腾讯游戏CROS DBA基于MariaDB 10.3.7开发定制的版本，主要完善并定制spider这一分布式MySQL存储引擎；TSpider [github地址](https://github.com/TencentDBA/TendbCluster3)  
TenDB是TenDB Cluster的存储层，是腾讯游戏CROS DBA基于Percona Server 5.7.20开定制的MySQL分支，主要提供一些更贴近游戏特点的基础能力，包括在线加字段、大字段压缩、binlog压缩及限速等； TenDB [github地址](https://github.com/TencentDBA/TenDB3)  
Tdbctl是集群的中控节点，是腾讯游戏CROS DBA团队基于TenDB开发（主要复用TenDB自身的SQL解析能力及MGR能力），主要提供集群路由管理、集群变更、集群监控等能力。Tdbctl [github地址](https://github.com/TencentDBA/Tdbctl)  

## TenDB Cluster简介
TenDB Cluster是腾讯游戏CROS DBA团队提供的MySQL分布式关系型数据库解决方案，主要特点包括：透明分库分表、高可用的MySQL集群服务，透明及在线的扩容及缩容；使得开发者可以仅专注于业务逻辑的开发及运营，无需编写数据分片逻辑，在海量用户并发情况下，也无须关心DB存储层的负载压力。

## 快速体验
可以使用Docker-compose快速部署TenDB Cluster进行体验。    
关于Docker-compose的细节见章节[使用Docker Compose快速部署](Documentation/op-guide/docker-compose-tendbcluster.md)

## [使用文档](Documentation/SUMMARY.md)
具体见[参考手册](Documentation/SUMMARY.md)


## 灵活部署
建议使用TSpider + TenDB + Tdbctl的方式部署TenDB Cluster集群。    
但开发者也可以根据自己的线上情况，灵活部署TenDB Cluster集群。 目前存在公司使用TSpider+腾讯云RDS 或 TSpider+AWS RDS的方式部署TenDB Cluster集群。


## 联系我们
对TenDB Cluster的任何问题或者意见，欢迎通过提issue的方式反馈：   
TSpider:  https://github.com/TencentDBA/TendbCluster3/issues   
TenDB:    https://github.com/TencentDBA/TenDB3/issues   
Tdbctl:   https://github.com/TencentDBA/Tdbctl/issues 


## 路线图
详情见章节[路线图](Documentation/roadmap.md)


## 开源协议
TenDB Cluster主要是使用GNU General Public License Version 2开源协议，项目中使用的第三方组件遵循各自使用的协议。详情见TenDBCluster-License。


## 致谢
感谢MariaDB及Percona在MySQL开源生态做出的杰出贡献，正是站在前人的肩膀上TenDB Cluster才得以诞生。   
另外，也特别感谢帮助TenDB Cluster达到其现状的杰出开发人员、贡献人和支持人员，详情见[致谢章节](Documentation/acknowledgements.md)