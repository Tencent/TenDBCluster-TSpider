# 集群权限
本章节主要介绍集群权限相关  
从权限作用上来区分，集群权限可以分为集群内部权限和集群外部权限  
集群内部权限主要用于集群内部的管理，对维护集群的正常工作非常重要，如果权限异常、可能直接导致集群服务不可用或服务异常
集群外部权限主要用于集群外部应用请求的权限控制，应用通过在集群上申请对应的权限来控制请求行为，确保数据的安全，如果权限异常，可能导致应用服务异常

## 集群内部权限
在集群内部，各个节点组件之间会存在连接请求信息，因为我们需要给一些特定的权限，以保证集群能正常运转, 集群权限说明如下  

| 请求节点| 	授权节点| 权限|说明|
| :--- | :----|:----|:----|
|TSpider|Tdbctl|ALL PRIVILEGES WITH GRANT OPTION|TSpider收到客户端DDL请求后，会直接转发给Tdbctl，Tdbctl重写后分发给TSpider、TenDB节点执行|
|TSpider|TenDB|SELECT, INSERT, UPDATE, TRUNCATE|TSpider需要有读写后端TenDB的权限
|Tdbctl|TSpider, TenDB|ALL PRIVILEGES WITH GRANT OPTION| Tdbctl需要连接TSpider，TenDB节点分发DDL操作|
|TenDB|N/A|N/A|N/A|

具体权限的授权，管理可以参考集群安装的[集群权限](manual-install.md/#cluster-privilege)

## 集群外部权限
集群外部权限是指应用请求集群时所需要的权限  
我们可以使用管理用户直接连接任意TSpider节点，根据应用需要给特定的账户、主机授权，在权限管理上与单机MySQL没有区别，具体参考[集群授权](grant-operator.md)
