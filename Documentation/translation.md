# 翻译分工

* [Introduction](README.md) will `done`
* 主要概念 
    * [整体架构](architecture.md)  will `done`
    * [主要特性](key-features.md)  will `done`
* 操作指南
    * [快速上手]()
        * [使用Docker Compose快速部署]
        * [基本操作]
    * 软硬件环境
        * [软件介质]
        * [源码编译](op-guide/compile.md) jam
        * [硬件要求](op-guide/system.md) jam
    * 部署
        * [集群搭建]
        * [手动安装](op-guide/manual-install.md)  jam
        * [路由管理](op-guide/route-manager.md)  jam
        * [集群权限](op-guide/cluster-grant.md)  jam
    * [运维](op-guide/cluster-operator.md)
        * [变更](op-guide/alter-operator.md)
        * [授权](op-guide/grant-operator.md)
        * [备份](op-guide/backup.md)
        * [监控](op-guide/monitor.md)
        * 工具
    * [扩缩容](op-guide/scale-up-down.md)
        * [接入层扩缩容](op-guide/TSpider-scale.md)
        * [存储层扩缩容](op-guide/TenDB-scale.md)
    * [高可用](op-guide/High-availability.md)   
        * [接入层高可用](op-guide/TSpider-failover.md)    
        * [存储层高可用](op-guide/TenDB-failover.md)  
        * [中控节点高可用](op-guide/Tdbctl-failover.md)  
    * 数据迁移
        * [从单机MySQL到集群](op-guide/Data-migrate.md/#jump1)
        * [从集群到单机MySQL](op-guide/Data-migrate.md/#jump3)
* 参考手册
    * [SQL语法](re-book/sql-grammar.md)  alex
      * [DDL](re-book/ddl-syntax.md)   alex
      * [DML](re-book/dml-syntax.md)  alex
      * [权限](re-book/grant.md)   alex
      * [存储过程](re-book/stored-procedure.md) alex
      * [函数](re-book/function.md)  alex
      * [触发器](re-book/trigger.md)  alex
      * [新增语法](re-book/new-grammar.md)  alex
    * [自增列](re-book/auto-increase.md)  alex
    * [事务](re-book/transaction.md)  will   `done`
      * [事务使用](re-book/transaction.md#jump2) will `done`
      * [普通事务](re-book/transaction.md#jump3)  will `done` 
      * [分布式事务](re-book/transaction.md#jump4)  will `done`
        * [xa commit xid one phase with logs](re-book/transaction.md#jump41)  will `done`
        * [参数spider_trans_rollback](re-book/transaction.md#jump42)  will `done`
        * [事务执行结果](re-book/transaction.md#jump43)  will `done`
        * [悬挂事务处理](re-book/transaction.md#jump44)  will `done`
        * [其它](re-book/transaction.md#jump5)  will `done`
    * [与单实例MySQL异同](re-book/mysql-compatibility.md/#jump)
        * [不支持特性](re-book/mysql-compatibility.md/#jump1)
        * [DDL](re-book/mysql-compatibility.md/#jump21)
        * [存储引擎](re-book/mysql-compatibility.md/#jump22)
        * [自增](re-book/mysql-compatibility.md/#jump23)
        * [执行计划](re-book/mysql-compatibility.md#jump24)
        * [分布式事务](re-book/mysql-compatibility.md/#jump25)
        * [存储过程](re-book/mysql-compatibility.md#jump26)
    * [参数说明](re-book/parameter.md)  jam
      * [TSpider参数说明](re-book/tspider-parameter.md)   jam
      * [TenDB参数说明](re-book/tendb-parameter.md)     jam
      * [Tdbctl参数说明](re-book/tdbctl-parameter.md)  jam
    * [错误码](re-book/errorno.md)  jam
    * [API](re-book/api.md)  jam
* [关于TenDB](tendb/tendb.md) yongxin
    * [加字段](tendb/instant-add-column.md) yongxin
    * [大字段压缩](tendb/blob-compress.md)  yongxin
    * [binlog压缩](tendb/binlog-compress.md)  yongxin
    * [binlog限速](tendb/binlog-speed-limit.md)  yongxin
    * [XA事务优化](tendb/xafeatures.md)  yongxin
	* [语法审核工具](tendb/tmysqlparse.md)  yongxin
* [TenDB Cluster in Kubernetes](kubernetes/cluster-in-k8s.md)
    * [TenDB Cluster Operator简介](kubernetes/cluster-operator.md)
    * [部署]
    * [配置]
    * [监控]
    * [运维]
    * [升级]
    * [扩缩容]
    * [高可用]
    * [常见问题]
* 最佳实践
    * [普通应用](practice/common-bestpractice.md)
    * [大数据应用](practice/bigdatapractice.md)
    * [事务应用](practice/transactionapplication.md)
* 常见问题
    * [多唯一键调整](com-problem/multi-unique-key-adjust.md)
    * [shard key选取](com-problem/shard-key-choose.md)
* [性能测试](performance-test.md)
* 技术支持
    * [联系我们](contribution/concat.md)
* 贡献
    * [源码贡献](contribution/contribute.md)
    * [问题反馈](contribution/contribute.md#jump2)