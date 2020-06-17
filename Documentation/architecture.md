# TenDB Cluster整体架构
TenDB Cluster 集群主要包括三个核心组件：TSpider，TenDB 和 Tdbctl。
此外还有简化云上部署管理的TenDB Cluster Operator。   

![pic](./pic/ar1.jpg)

## TSpider
TSpider是TenDB Cluster集群的接入层。   
TSpider由腾讯游戏CROS DBA团队基于MariaDB 10.3.7上的开源存储引擎spider定制研发而成，是游戏场景中规模最大的分布式MySQL存储引擎。TSpider使用了MySQL分区表的特性，却将每个分区的数据存储到一个远端MySQL实例中。   
![pic](./pic/spider.png)

作为一种MySQL引擎，TSpider天然的支持MySQL协议，而且使用MySQL标准API即可请求TSpider。   
TSpider在接入到应用请求后，会通过数据路由规则对SQL改写然后分发到相应的存储节点TenDB执行，再对TenDB的返回结果进行处理最终返回给应用层。   
TSpider本身并不存储数据，基本是无状态的（各TSpider节点部分配置需要不同），可无限水平扩展。 应用层可通过负载均衡组件（比如LVS, L5，甚至DNS)提供的统一接入地址访问多个对等的TSpider节点。

## TenDB
TenDB是TenDB Cluster的数据存储层。   
TenDB由腾讯游戏CROS DBA团队基于Percona Server 5.7.20定制而成，额外提供在线加字段、大字段压缩、binlog压缩/限速等特性及性能优化、分布式事务优化、BUG FIX等， 目前是腾讯游戏MySQL存储方案的默认版本。   
通常一个集群会有多个TenDB实例，均衡的存储集群数据。每个TenDB可以使用主备部署或者MGR的方式来保证存储层的可用性。

## Tdbctl
Tdbctl是TenDB Cluster集群的中央控制模块。   
Tdbctl是腾讯游戏CROS DBA团队基于TenDB开发（主要复用TenDB自身的SQL解析能力及MGR能力），主要作用是集群路由配置管理、集群变更、集群切换及一定的集群监控。   
集群路由配置管理，是指维护TSpider到TenDB的路由指向，并保证路由配置的可用性和唯一性。   
集群变更，是业务在TSpider执行的DDL会由TSpider转发到Tdbctl，在Tdbctl进行SQL改写后分别分发到TSpider/TenDB各节点执行。   
集群监控，是Tdbctl会监控TenDB Cluster整个集群的权限、路由、表结构及可用性等。   
Tdbctl是一个在MySQL基础上开发的工具，支持MySQL的MGR特性。因此在部署上我们使用3个及以上的Tdbctl节点，搭建成一个MGR集群，从而保证中控节点的高可用及路由配置的强一致。   

