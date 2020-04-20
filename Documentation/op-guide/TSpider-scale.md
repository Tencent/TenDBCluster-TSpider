# TSpider接入层扩缩容
接入层扩缩容是指通过增减TSpider节点的方式，达到水平扩缩容整个集群的目的  
TSpider在接入到应用请求后，会通过数据路由规则对SQL改写然后分发到相应的存储节点TenDB执行，再对TenDB的返回结果进行处理最终返回给应用层  
TSpider本身并不存储数据，基本是无状态的（各TSpider节点部分配置需要不同），在不考虑后端TenDB节点瓶颈的情况下，可无限水平扩展。 应用层可通过负载均衡组件（比如LVS, L5，DNS)提供的统一接入地址访问多个对等的TSpider节点  
当应用侧请求负载较低时，可以通过在线减少TSpider节点来缩减集群接入层，在保证应用稳定请求的情况下，节省一定的集群资源  
当应用侧请求负载较高时，可以通过在线增加TSpider节点来扩增集群接入层，使集群能快速响应更多的应用请求，提供更高的实时吞吐能力  
在增减节点后，一般希望应用侧不需要做任何配置修改就能快速感知到节点变化，目前比较推荐的方式是给接入层节点配置名字服务或负载均衡组件，应用侧通过域名等方式请求。当节点数发生变化时，只需要变更名字服务的解析结果，从而做到应用侧无感知。例如在使用域名连接集群时，由于域名是无法区分不同TSpider节点端口号的，因此如果考虑使用域名的方式，在部署之前尽量让分布在不同主机的TSpider节点使用相同的端口号服务，降低系统复杂度

## 扩容

<a id="jump1"></a>


假定有个当前拓扑结构的TenDB Cluster集群  

| 节点类型 | 	Host|端口|Server_name|
| :--- | :----|:----|:----|
|TSpider|127.0.0.1|25000|SPIDER0|
|TSpider|127.0.0.1|25001|SPIDER1|
|Tdbctl|127.0.0.1|26000|TDBCTL|
|TenDB|127.0.0.1|20000|SPT0|
|TenDB|127.0.0.1|20001|SPT1|
|TenDB|127.0.0.1|20002|SPT2|
|TenDB|127.0.0.1|20003|SPT3|

### 部署新的TSpider节点  

参照手工部署章节[部署TSpider](manual-install.md/#jump2), 在本地部署一个端口号为25002的TSpider实例

### 导入表结构
从任一TSpider节点备份库表视图(备份时优先备份mysql.servers表)，并导入到此节点  
__注意事项__
- 在新的TSpider节点接入集群之前(以中控节点执行tdbctl flush routing为标记点)，整个集群不能存在DDL行为，否则会导致表结构不一样
- 在导入schema到新TSpider节点之前，需要```set ddl_execute_by_ctl = off```, 确保导入的schema不会被Tdbctl节点分发，TSpider节点不会执行报错 
- 必须备份mysql.users表，确保应用侧权限的一致性
- 必须在导入mysql.users表后，再导入schema，否则导入会报错（因为servers表为空，spider引擎异常）

### 授权
分别登陆TdbCTL，TenDB节点给新的TSpider节点授权
用户名、密码参考mysql.servers表的Username，Password字段，也可使用新的用户名密码
下面以新用户名密码为例示范
```sql
create user 'myql1'@'127.0.0.1' identified by 'mysql1';
grant all privileges on 'myql1'@'127.0.0.1';
```

### 更新路由表  
连接Tdbctl节点，插入新的节点信息，由其同步到整个集群
> mysql -umysql -pmysql -h127.0.0.1 -P26000
```sql
insert into mysql.servers values('SPIDER2','127.0.0.1','','mysql','mysql',25002,'','SPIDER','');
tdbctl flush routing;
```

### 修改域名
如果接入层接入了域名系统，需要修改域名配置将新的节点ip接入  
在此示例中，由于我们都是采用本地部署，主机名都是127.0.0.1，因此跳过此操作  
如果是不同主机，各个接入层端口号也不一样，仅仅修改域名是不行的，还有考虑端口的差异性

### 扩容后拓扑结构
| 节点类型 | 	Host|端口|Server_name|
| :--- | :----|:----|:----|
|TSpider|127.0.0.1|25000|SPIDER0|
|TSpider|127.0.0.1|25001|SPIDER1|
|__TSpider__|__127.0.0.1__|__25002__|SPIDER2|
|Tdbctl|127.0.0.1|26000|TDBCTL|
|TenDB|127.0.0.1|20000|SPT0|
|TenDB|127.0.0.1|20001|SPT1|
|TenDB|127.0.0.1|20002|SPT2|
|TenDB|127.0.0.1|20003|SPT3|

## 缩容
假定有个当前拓扑结构的TenDB Cluster集群  

| 节点类型 | 	Host|端口|Server_name|
| :--- | :----|:----|:----|
|TSpider|127.0.0.1|25000|SPIDER0|
|TSpider|127.0.0.1|25001|SPIDER1|
|Tdbctl|127.0.0.1|26000|TDBCTL|
|TenDB|127.0.0.1|20000|SPT0|
|TenDB|127.0.0.1|20001|SPT1|
|TenDB|127.0.0.1|20002|SPT2|
|TenDB|127.0.0.1|20003|SPT3|
需要缩容的目标节点是TSpider#25001

### 修改域名
如果接入层有配置域名使用，在缩容之前，先将待缩容ip从域名中去掉，确保新的请求不再路由到待缩容的TSpider节点

### 更新路由表
连接Tdbctl节点，删除待缩容的节点信息，由其同步到整个集群
> mysql -umysql -pmysql -h127.0.0.1 -P26000  
```sql
delete from mysql.servers where Server_name='SPIDER1';
#刷新路由
tdbctl flush routing;
```

### 关闭节点
关闭已缩容TSpider节点的MySQL服务  

### 缩容后拓扑结构

| 节点类型 | 	Host|端口|Server_name|
| :--- | :----|:----|:----|
|TSpider|127.0.0.1|25000|SPIDER0|
|__TSpider__|__127.0.0.1__|__25001__|__SPIDER1已删除__|
|Tdbctl|127.0.0.1|26000|TDBCTL|
|TenDB|127.0.0.1|20000|SPT0|
|TenDB|127.0.0.1|20001|SPT1|
|TenDB|127.0.0.1|20002|SPT2|
|TenDB|127.0.0.1|20003|SPT3|

## 其他说明
在长连接较多的应用场景，扩容并不能立刻解决集群的负载问题。如果应用重连机制比较完善的话，可以在负载较高的TSpider节点执行kill操作，通过kill部分连接，让新连接逐步均衡到新加入的TSpider节点
```sql
show processlist;
#kill指定id的连接
kill thread_id;
#kill所有连接,这个是TSpider扩展的指令
kill threads all
```

集群缩容后，如果已缩容的TSpider节点没有及时关闭服务，在长连接场景或域名缓存等情况下，可能依旧会有连接请求到旧的TSpider节点，而此TSpider节点因为已经从集群路由表清理，导致Tdbctl不会再同步DDL操作，这样应用侧可能会用旧的表结构来操作请求，造成未知错误。因此缩容后的TSpider节点最好及时关闭服务回收资源