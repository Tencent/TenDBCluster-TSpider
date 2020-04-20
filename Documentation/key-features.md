# TenDB Cluster核心特性

## 透明分库分表
TenDB Cluster默认会将数据水平分布到各个TenDB节点上，分库分表逻辑对应用层完全透明。另外，TenDB Cluster支持哈希、范围、列表等算法，用户也可自定义数据分布策略，提高应用的存储和处理性能。   
## 兼容MySQL协议
TenDB Cluster的接入层TSpider作为一个分布式MySQL存储引擎，原则上支持任意类型的SQL；而且应用层通过MySQL API即可访问TenDB Cluster。     
## 在线扩展
TenDB Cluster支持在线动态扩展，且在线扩展其间集群业务运行不受影响。下面分别说明3个组件的在线扩展。
### **TSpider**
接入层TSpider并不存储数据，是无状态的节点。因此TSpider的扩容只需要将相同权限及schema的TSpider节点，加入到当前业务请求的名字服务中。
如果是缩容，则只需将TSpider节点从名字服务中剔除即可。
### **TenDB**
存储层TenDB的扩容或者缩容，先线下利用MySQL建热备的方式将各TenDB实例搬迁到待扩容的机器上，待数据同步完后再刷新Tdbctl的路由信息，即可不影响集群服务的前提下完成扩展。
### **Tdbctl**
Tdbctl通常没有扩展需求。如果需求，在Tdbctl构建的MGR集群增加节点即可。
## 高可用
TenDB Cluster是一个稳定可靠的MySQL分布式数据库解决方案，因此各个节点都有冗余与互备，在硬件或软件故障时，有效保障数据处理不间断。下面分别说明3个组件故障后的高可用。
### **TSpider**
在某个TSpider节点软硬件故障时，将该节点从名字服务中剔除让业务无法访问到该节点即可。
### **TenDB**
若TenDB节点使用主备部署，当某个主TenDB节点故障，则刷新Tdbctl中的路由信息即可将请求路由到备TenDB节点上；   
若TenDB节点使用MGR方式部署，当某个TenDB节点故障，则会自动从集群中剔除。
### **Tdbctl**
Tdbctl是一个MGR集群，某个节点故障则自动会被集群中剔除。