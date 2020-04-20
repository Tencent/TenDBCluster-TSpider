# 路由管理
TenDB Cluster集群的数据分发、读写都会基于分区路由规则，因此在集群建立，数据管理，集群故障等场景维护路由显得至关重要
目前TSpider，Tdbctl节点都有一份路由表。其中TSpider节点的路由表由Tdbctl来同步，并全局保证各TSpider节点路由的一致性

## 路由表说明
目前路由信息都存储在mysql库下的servers表，各字段说明如下

| 字段名 |说明
| :--- | :----|
|Server_name|对应的各节点名，目前合法值为TDBCTL0~N, SPIDER0~N,SPT0~N|
|Db|对应server的db，已废弃，为空即可
|Username|访问server的帐号
|Password|访问server的密码
|Port|访问server的端口
|Socket|访问server的socket，N/A
|Wrapper|组件类别，目前合法值为TDBCTL，mysql，SPIDER, SPIDER_SLAVE
|Owner|N/A

- SPIDER
对应TSpider节点
- mysql
对应TenDB节点
- TDBCTL
对应Tdbctl节点
server_name默认从0~N递增

### 示例
下表是个4 TenDB分片，2 TSpider节点，1 Tdbctl中控的本地集群路由表

|Server_name|Host|Db|Username|Password|Port|Socket|Wrapper|
| :--- | :----|:--- | :----|:--- | :----|:--- | :---|
|TDBCTL0|127.0.0.1|de|mysql|mysql|26000||TDBCTL|
|SPIDER0|127.0.0.1||mysql|mysql|25000||SPIDER|
|SPIDER1|127.0.0.1||mysql|mysql|25001||SPIDER|
|SPT0|127.0.0.1||mysql|mysql|20000||mysql|
|SPT1|127.0.0.1||mysql|mysql|20001||mysql|
|SPT2|127.0.0.1||mysql|mysql|20002||mysql|
|SPT3|127.0.0.1||mysql|mysql|20003||mysql|


## 路由配置
目前路由配置INSERT方式，通过SQL在Tdbctl中控节点维护，由Tdbctl下发同步给其他节点，因此只需要在Tdbctl配置即可
Tdbctl读取mysql.servers表中的SPIDER信息后，会与TSpider节点建立连接，将自身的mysql.servers表同步覆盖
>路由配置完成后，需要在Tdbctl节点执行flush操作来强制同步，语法如下
```sql
tdbctl flush routing;
```

>集群内部都是采取mysql协议通信，因此在配置完成后，还需要授权操作，确保各节点能正常请求  
TSpider： 需要有连接操作TenDB节点所有库表的权限
Tdbctl：需要有连接操作TSpider，TenDB节点的所有库表权限

## 路由一致性
Tdbctl会定期检查TSpider与Tdbctl的差异性，并定时同步自身配置到TSpider节点，并执行刷新操作使路由生效
### 重要参数说明
- tc_check_repair_routing_interval
> 定期检查TSpider和Tdbctl路由的时间间隔，默认300s
- tc_check_repair_routing
> 检查到TSpider和Tdbctl路由不一致后，是否修复覆盖（以Tdbctl为准），默认为on


## SLAVE路由
TenDB Cluster可配置成读写分离的集群方案，在TenDB启用主从场景下，可以配置slave集群，让读请求只访问从节点，来分担主库的压力
对于Slave集群，其路由也是维护在Tdbctl节点，配置方式与主集群基本相同，唯一区别是Wrapper字段为SPIDER_SLAVE
- SPIDER_SLAVE
> Tdbctl在同步路由时，会特殊处理Wrapper为SPIDER_SLAVE的路由信息。
不会将路由同步过滤给Slave Spider节点，只会同步schema
关于slave集群的详细信息，可以参考Slave集群章节