# TenDB Cluster错误码
本篇文档描述在使用 TenDB Cluster 过程中会遇到的错误码及其含义。

## 错误码
TenDB Cluster 兼容 MySQL 的错误码，在大多数情况下会返回和 MySQL 一样的错误码。下表列出的则是另外一些TenDB Cluster特有的错误码。

|错误码|说明|
|-|-|
|12500|tspider返回了一个非预期内的错误行为 |
|12501|建表时指定了非法的connection信息|
|12502|建表时指定了非法的connection长度大于512，超过最大长度限制|
|12518|配置spider_read_only后仍然进行写行为|
|12521|建表是连接中配置的server的wrapper不合法|
|12524|找不到tspider的系统表|
|12601|spider_internal_xa启用时，不能同时使用一致性快照|
|12602|使用分布式事务时，被分配到一个已被占用的事务xid|
|12603|使用分布式事务时，尝试提交一个尝未prepare的事务xid|
|12604|使用分布式事务时，xa prepare一个已prepared的事务xid|
|12605|使用分布式事务时，xa start一个已start的事务xid|
|12609|tspider使用了非当前版本的系统表|
|12610|分布式事务可能产生部分提交|
|12701|tspider和tendb的连接被异常断开|
|12702|tendb上表缺失|
|12710|tendb表结构与tspider表结构不一致|
|12714|tspider上引擎层open table超时|
|12723|tspider和tendb层的连接池被占用完，且一定时间内也未空闲连接释放|
|12724|tspider上获取引擎层共享表对象失败|
|12725|tspider和tendb间使用了非法的连接对象|
|12726|非法或者不支持的where条件|
|12727|使用到不是最新版本的表对象|
|12728|执行分布式事务时返回超时错误；需要业务侧去判定事务状态是提交还是回滚|
|12729|执行分布式事务时失败|
