# Ansible 存储层扩缩容

# 1# TenDB 扩容
在 TenDB 分片数固定的情况，TenDB 的扩容是指将实例分片，迁移到高配机器上，即 scale up 。

下面演示将 SPT1 的一对主备，通过链式 slave 迁移到新机器上。初始状态：
```
+-----------------+
|   tendb-spt1    |
+-----------------+
         ^  Master
         |        
         |        
+-----------------+
|   tendb-spt1-1  |
+-----------------+
```

### 1.1 添加新机器到对应分片
```
#### SPT1 ####
[SPT1]
tendb-spt1 ansible_host=192.168.1.95 mysql_shard=SPT1 mysql_port=20001 role=master
tendb-spt1-1 ansible_host=192.168.1.147 mysql_shard=SPT1 mysql_port=20001 role=slave master=tendb-spt1

# the new tendb pair machines that have higher performance
tendb-spt1-2 ansible_host=192.168.1.6 mysql_shard=SPT1 role=slave master=tendb-spt1 innodb_buffer_pool_size_mb=4096 mysql_data_dir=/data1/mysqldata/{{mysql_port}}
tendb-spt1-3 ansible_host=192.168.1.210 mysql_shard=SPT1 role=slave master=tendb-spt1-2 innodb_buffer_pool_size_mb=4096 mysql_data_dir=/data1/mysqldata/{{mysql_port}}
```
这里手动指定了 `innodb_buffer_pool_size_mb` 和 `mysql_data_dir`，如果期望自动计算 buffer pool，你需要将目标ip上其它将要扩容的实例，都写进去，因为自动计算是按照 inventory group `tendb` 的 `ansible_host` 汇总的

另外注意，新的两台机器新加入时 role=slave，但指向的 master 不同，这是为了后续切换 tendb-spt1 与 tendb-spt1-2 主备是，一步到位。
```
+-----------------+            +-----------------+
|   tendb-spt1    | <--------- |   tendb-spt1-2  |
+-----------------+            +-----------------+
         ^  Master                      ^         
         |                              |         
         |                              |         
+-----------------+            +-----------------+
|   tendb-spt1-1  |            |   tendb-spt1-3  |
+-----------------+            +-----------------+
```

### 1.2 做链式slave
```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2,tendb-spt1-3 init_common.yml
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2,tendb-spt1-3 build_slave_new.yml
```
使用 `-l` 指定要操作的新 TenDB node。

### 1.3 主备切换
```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1,tendb-spt1-2 -e "master_tgt=tendb-spt1-2" switch_master_slave.yml
```
使用 `-l` 指定要互切的两个 TenDB node，并指定 `master_tgt` 谁成为 master。

```
+-----------------+            +-----------------+         
|   tendb-spt1    | ---------> |   tendb-spt1-2  |         
+-----------------+            +-----------------+         
         ^                     Master   ^                  
         |                              |                  
         |                              |                  
         |                              |                  
+-----------------+            +-----------------+         
|   tendb-spt1-1  |            |   tendb-spt1-3  |         
+-----------------+            +-----------------+         
                                                    
```

切换后会同时修改 Tdbctl 里面的路由

### 1.4 停止老实例
```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1,tendb-spt1-1 --tags=stop stop_tendb.yml
```
如果一切正常，停止原机器上的主备实例
```
+-----------------+
|   tendb-spt1-2  |
+-----------------+
Master   ^         
         |         
         |         
+-----------------+
|   tendb-spt1-3  |
+-----------------+
```

> 重要：删掉 inventory 里面老实例的信息，并更新role 和 master

```
#### SPT1 ####
[SPT1]
tendb-spt1-2 ansible_host=192.168.1.6 mysql_shard=SPT1 role=master innodb_buffer_pool_size_mb=4096 mysql_data_dir=/data1/mysqldata/{{mysql_port}}
tendb-spt1-3 ansible_host=192.168.1.210 mysql_shard=SPT1 role=slave master=tendb-spt1-2 innodb_buffer_pool_size_mb=4096 mysql_data_dir=/data1/mysqldata/{{mysql_port}}
```

### 1.5 部分扩容时，修改其它分片配置（可选）
扩容时建议原机器上的其它分片，都扩到对应的新机器上，不保留原机器。这样的好处是**不需要**修改其它分片 buffer pool 配置，来让它达到扩容的目的。

如果要保留原机器，那么剩余的分片可以设置更大的 buffer pool，再运行 `update_config_tendb.yml` 。
修改剩余分片 SPT0 的缓冲池：
```
ansible-playbook -i hosts.tendbcluster -l SPT0 update_config_tendb.yml

# 默认修改配置不会重启实例，加入 restart=true 会自动重启 slave
ansible-playbook -i hosts.tendbcluster -l SPT0 update_config_tendb.yml -e "restart=true"
```

### TenDB一键扩容
> 正式环境不建议使用

一键扩容是将上面的做slave与主备切换串起来，但同样需要开始前设置好 inventory 信息，结束后删除旧的host并更新role/master：

```
ansible-playbook -i hosts.tendbcluster -l SPT1 -e "master_tgt=tendb-spt1-2" migrate_tendb.yml
```

## 2 TenDB缩容
TenDB 缩容过程，与扩容完全相同，区别只是在给定新机器是，使用低配置机器，并评估好多个实例运行在一台机器上，磁盘空间是否足够。

