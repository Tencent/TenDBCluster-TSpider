# Ansible 常用运维操作

## 修改 TenDB 配置文件

> 不要在目标TenDB机器上直接修改的配置文件，在运行改playbook后将被覆盖
> 暂未实现 set global xxx=xx 动态修改

比如修改 TenDB 的最大连接数 _group_vars/tendb_ :
```
mycnf_mysqld:
  innodb_flush_log_at_trx_commit: 0
  character-set-server: utf8mb4
  max_connections: 5000
```

生成新的 my.cnf:
```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2 update_config_tendb.yml

# 默认修改配置不会重启实例，加入 restart=true 会自动重启 slave
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2 update_config_tendb.yml -e "restart=true"
```

## 重启TenDB节点
```
# 关闭
ansible-playbook -i hosts.tendbcluster -l tendb-spt1,tendb-spt1-1 stop_tendbcluster.yml

# 启动
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-3 start_tendbcluster.yml

# 重启
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-3 restart_tendbcluster.yml

# 重启所有tendb
ansible-playbook -i hosts.tendbcluster -l tendb restart_tendbcluster.yml
```

你也可以提通过上面类似的命令，重启 TSpider 或者 Tdbctl 节点。

## 重建TenDB slave

- 重建 slave 要求 slave 没有在运行
- 目前采用 `mysqldump --single-transaction` 的方式，在 master 上进行备份
- 备份文件拷贝在中控机上进行，通过 `scp -3` 流量会经过中控机
- 用户可以自己实现重建 slave 的playbook

```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2,tendb-spt1-3 init_common.yml
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2,tendb-spt1-3 build_slave.yml
```

## 手动主备切换TenDB
```
ansible-playbook -i hosts.tendbcluster -l tendb-spt3-2,tendb-spt3 -e "master_tgt=tendb-spt3-2" switch_master_slave.yml
```

playbook 校验了指定的 host 是否属于同一分片，如果是不同分片会报错。

目前这里的手动切换主备的逻辑比较简单，没有校对数据的完整性。

用户可以自己实现手动起切换逻辑，来确保数据不丢失，切换完成后需要更新路由。可以参考 ansible role  `switch_master_slave`

切换后，记得手动更新 inventory 的 role 和 master 信息，以免与 Tdbctl 里面的 routes 不一致。

## 修改主备关系
将 TenDB Node `tendb-spt1-3` 的 master 设置为 `tendb-spt1-2`
```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-3 -e "master_tgt=tendb-spt1-2" tendb_change_master.yml 
```
change master 之后需要修改 inventory 中的 role/master 信息，保持一致。

## 同步TSpider表结构
将 `tspider` 组里面第一个节点的表结构，导入到 `tdbctl-node-03`
```
ansible-playbook -i hosts.tendbcluster -l tdbctl-node-03 sync_tspider_schema.yml
```

## 更新Tdbctl路由
根据 inventory 里面的信息，更新 Tdbctl `mysql.servers` 。
采用 `REPLACE INTO` 方式，一定要确保 inventory 中 `role` 和 `master` 设置正确。
```
ansible-playbook -i hosts.tendbcluster -l tdbctl update_config_tdbctl.yml
```

## TenDB故障恢复
tendb 在 slave 故障时，目前不需要特殊处理（slave未提供读服务）。

tendb 在 master 故障时，需要第三方机制完成主备切换，成功将 slave 提升为 master 后，需要修改 Tdbctl 里面的路由。
下次运行 playbook 时，由于某个分配 master 、 slave 角色发生变化，如果 inventory 中的信息与 Tdbctl routes 中不一致，则会退出。

当原 master 修复并启动之后，可以运行 `change_master.yml` 来恢复主备关系，成为新的 slave 。

## 启动集群
启动集群所有实例（tendb, tspider, Tdbctl）
```
ansible-playbook -i hosts.tendbcluster start_tendbcluster.yml
```

或者重启整个集群：
```
ansible-playbook -i hosts.tendbcluster restart_tendbcluster.yml
```

## 停止集群
停止集群所有实例（tendb, tspider, Tdbctl）
```
ansible-playbook -i hosts.tendbcluster stop_tendbcluster.yml
```
## 销毁集群
在实例停止状态，才可以销毁集群，并且会提示提供管理密码才能继续
```
ansible-playbook -i hosts.tendbcluster destroy_tendbcluster.yml
```