# TenDB Cluster监控

TenDB Cluster涉及多个组件，本文档主要描述各组组件在日常运营中需要关注的项。
## TSpider
TSpider是TenDB Cluster集群的接入层。TSpider在接入到应用请求后，会通过数据路由规则对SQL改写然后分发到相应的存储节点TenDB执行。

TSpider主要的监控项如下：
1. TSpider实例端口是否能连接
2. TSpider连接数检查：连接数超过max_connections限制出现max_connect_errors
3. TSpider实例上到后端TenDB实例能否连接
4. TSpider mysqldata目录大小检查：TSpider上是不保留应用数据的，mysqldata目录一般不会太大，如果太大，需要确认是否正常。经验值是一般不会超过10G。
    以下几种情况可能导致mysqldata目录过大：
    *    数据迁移时relay log空间耗用
    *    TSpider上拉取数据临时空间占用
    *    如果TSpider上表数量过多，表统计信息空间耗用
5. TSpider上连接异常状态检查，主要是跟锁相关的异常状态检查，比如"Waiting for table flush"。
6. TSpider error log中特定报错误检查。
    关于TSpider的错误码，可以参考 [错误码](./../re-book/errorno.md)
7. OS层面主要关注以下指标：
    *  CPU利用率
    *  内存利用率
    *  磁盘空间利用率


## TenDB
TenDB是TenDB Cluster的数据存储层。一个集群后端一般会有多组TenDB实例，每组TenDB可以使用主备部署或者MGR的方式来保证存储层的可用性。

TenDB主要的监控项如下：
1. TenDB实例端口是否能连接
2. TenDB连接数检查：连接数超过max_connections限制出现max_connect_errors
3. TenDB上连接异常状态检查，主要是跟锁相关的异常状态检查
4. TenDB实例error log中特定报错误检查。关键字包含ERROR、restarted,hanging,Locked等
5. 慢查询数量，如果超过阈值，介入处理。
6. DB并发线程数监控
7. 复制状态检查，主要涉及一下：
    *  复制io线程检查
    *  复制sql线程异常
    *  复制Last Errno检查
    *  延迟检查
6. checksum执行结果检查
    *  推荐使用pt-table-checksum进行主备数据一致性检查
    *  定期检查checksum的结果，如果有一致性，介入处理
2. OS层面主要关注以下指标：
    *  CPU利用率
    *  I/O利用率
    *  磁盘空间利用率
    *  网卡流量

    
       
备注：性能数据和日志采集展示请参考其他章节.



  