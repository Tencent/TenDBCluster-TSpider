# TenDB存储层扩缩容
存储层扩缩容是指通过更换存储节点的硬件配置、主机等方式，让单机TenDB能提供更高更快的连接请求，从而提升整个集群的吞吐能力，达到垂直扩缩容整个集群的目的  
当应用侧请求负载较低、存储节点资源较空闲时，可以通过给空闲的TenDB存储节点做slave的方式，使用较低配置的slave机器来同步master数据，在主从数据同步完成，一致性检查通过后，通过在线修改路由表的方式，将数据路由从高配置的Master节点切换到低配置的Slave节点，从而减少存储资源, 达到缩容目的  
当应用侧请求负载较高、存储节点压力较大时，可以通过给TenDB节点做slave的方式，使用较高配置的slave机器来同步master数据，通过在线修改路由表的方式，将数据路由从低配置的Master节点切换到高配置的Slave节点，使得TenDB存储节点能提供更高的吞吐，从而整体提升集群性能
不论是缩容，还是扩容操作，目前暂时只能支持垂直扩容，通过调整TenDB的存储配置来满足线上需求

## TenDB扩容
假定有个当前拓扑结构的TenDB Cluster集群, 在运行时发现host4机器负载很高，影响整个集群性能，需要更高硬件资源的机器host9。目标是将路由中的host4替换为host9

| 节点类型 | 	Host|端口|Server_name|
| :--- | :----|:----|:----|
|TSpider|host1|25000|SPIDER0|
|TSpider|host2|25001|SPIDER1|
|Tdbctl|host3|26000|TDBCTL|
|__TenDB__|__host4__|__20000__|__SPT0待扩容__|
|TenDB|host5|20001|SPT1|
|TenDB|host6|20002|SPT2|
|TenDB|hosts|20003|SPT3|

### 部署TenDB节点
参考[部署TenDB](manual-install.md/#jump1)在host9机器部署一个新的TenDB节点，my.cnf配置可以参考host4的SPT0实例，不过需要根据当前硬件配置适当调整```innodb_buffer_pool_size```等参数，来满足更高性能，具体调整可以参考[MySQL文档](https://dev.mysql.com/doc/refman/5.7/en/innodb-parameters.html#sysvar_innodb_buffer_pool_size)


### 建立主从同步
 - 使用msyqldump备份host4的实例，保存一致性备份点binlog_file_xxx, binlog_file_pos_xxx
 - 导入备份到host9实例
 - 登录host4机器授权复制权限
 ```sql
 #语法示例
 grant SELECT, RELOAD, PROCESS, SHOW DATABASES, REPLICATION CLIENT  on *.* to  '$USER'@'host9' IDENTIFIED BY '$PASS'; 
 ```
 -  建立数据同步
 ```sql
 change master to master_user='$USER', master_password='$PASS', master_host='host9', master_port='$PORT', master_binlog_file='binlog_file_xxx', master_binlog_pos='binlog_file_pos_xxx';
 #开启同步
 start slave;
 #查看同步状况
 show slave status\G
 ```

- 数据校验  
```
在主从同步跟进完成后(一般判断依据为show slave status查看seconds_behind_master值是否为0)，可以通过一些数据校验工具来检查主从数据是否一致，确保数据可用性  
```
参考: percona的[pt-table-checksum](https://www.percona.com/doc/percona-toolkit/3.0/pt-table-checksum.html)


### 权限配置
```
需要确保集群的所有TSpider节点，Tdbctl节点有all privileges访问host9
因为新host9在导入备份时已经导入了host4的所有权限，因此不需要再单独授权，严格场景下可以作为一个检查项
```
### 在线切换
> 切换之前，需要检查各个TSpider节点是否有长事务，耗时较长的SQL，这类SQL可能会导致切换时间较长、甚至切换失败，因为在新旧路由切换时，为了保证数据一致性，集群切换指令是存在加锁操作的。

- 修改中控节点路由

```sql
#连接中控节点
mysql -umysql -pmysql -hhost3 -P26000
#修改路由
update mysql.servers set Host='host9' where Server_name='SPT0'; 
#刷新路由，Tdbctl扩展SQL语法
tdbctl flush routing;
```

### 切换验证
> 连接host9实例，查看是否有请求源为此集群的TSpider节点

### 切换清理
```sql
#在host9执行reset断开主从同步
stop slave;
reset slave all;
```
> 关闭host4的MySQL实例

## TenDB缩容
存储层的缩容流程与扩容基本是一样的，唯一区别在于做Slave的主机性能配置要求比Master要低，在此就不再展开讲解。  
通过缩容能达到资源节省的目的，让整个集群能更合理的提供服务

## 其他说明
存储层扩缩容时，最重要一环的是路由的修改```tdbctl flush routing```。该SQL首先会修改各TSpider节点的mysql.servers表(仅修改，此时新路由规则并没有在TSpider节点生效)，然后加全局读锁，阻塞新的写请求，防止在路由变更时主从同时读写发生造成数据不一致; 在加锁完成后，会在各个TSpider节点执行flush操作，使新路由生效。此时TSpider侧的新连接会使用新路由规则来请求后端存储TenDB；最后释放全局读锁，完成切换
