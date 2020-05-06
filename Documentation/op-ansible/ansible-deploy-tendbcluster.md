# Ansible快速部署TenDBCluster

## 1. 准备工作

关于 host inventory 和 group_vars 的定义，参考 [Ansible playbook说明](ansible-def-inventory-vars.md)。

### 1.1 介质包

Ansible playbook 通过 http get 方式下载安装包，如果能访问github，可以直接指定 `fileserver` 为github地址。如果在私有网络，需要搭建一个 http file server。

介质包：
```
mysql-5.7.20-linux-x86_64-tmysql-3.1.5-gcs.tar.gz
mariadb-10.3.7-linux-x86_64-tspider-3.4.5-gcs.tar.gz
tdbctl-1.4-linux-x86_64.tar.gz
```

### 1.2 Ansible中控机
下载并安装 ansible-2.6.13-1.el6.noarch.rpm 。
ansible 版本要求大于 2.4 。

### 1.3 配置ssh互信或密码
目前我们是基于 root 用户来安装、维护 TenDBCluster 的。

#### 生成秘钥对
```
ssh-keygen -t rsa -b 2048 -f tendbcluster_rsa
```
在当前目录下生成了私钥 *tendbcluster_rsa* 和公钥 *tendbcluster_rsa.pub* 

你可以手动分发 ssh key 设置互信，跳过本节即可，也可使用 playbook *add_sshkey.yml* 在提供root密码情况下，添加 public key 到root。

#### 分发公钥
在 ansible inventory 中设置 `ansible_ssh_public_key_file`：（这个是我们自定义变量，跟ansible内置变量无关）
```
ansible_ssh_user=root
ansible_ssh_port=22
# ansible_ssh_private_key_file=/root/.ssh/tendbcluster_rsa

ansible_ssh_public_key_file=/root/.ssh/tendbcluster_rsa.pub
```

通过root分发公钥，这里最好所有机器共用一个root密码，方便操作。
（如果不嫌麻烦，也可以在每个 host 后面加上 `ansible_ssh_pass` 变量）
```
ansible-playbook -i hosts.tendbcluster add_sshkey.yml -u root -k
```
`-k` 提示输入 root 密码，`ansible_ssh_private_key_file`需要注释，避免ansible优先使用它来登录


#### 秘钥分发成功后
1. 可以修改root密码
2. 设置 `ansible_ssh_private_key_file` 变量开启，后续的操作通过这个 priviate key 进行，不需要root密码

```
ansible_ssh_user=root
ansible_ssh_port=22
ansible_ssh_private_key_file=/root/.ssh/tendbcluster_rsa

ansible_ssh_public_key_file=/root/.ssh/tendbcluster_rsa.pub
```

### 1.4 MySQL密码加密
考虑大安全因素，使用 ansible-vault 对 MySQL 的 admin/replication 用户加密。
设置管理密码，这个密码用于加解密 mysql 用户的密码：
```
echo 'VaultPass123' > password_file
```

生成密文：
```
# 生成 mysql root/admin 密码
ansible-vault encrypt_string --vault-id mysql@password_file xxxxxx --name tendbcluster_user_admin_pass

# 生成 mysql replication 密码
ansible-vault encrypt_string --vault-id mysql@password_file xxxxxx --name tendbcluster_user_repl_pass
```

将上面生成的密码，分别设置到 *group_vars/all* 里面的 `tendbcluster_user_admin_pass` 和 `tendbcluster_user_repl_pass` 变量

后续运行playbook时，如果遇到错误 *Attempting to decrypt but no vault secrets found* ，均需要使用 `--vault-id password_file` 来解密密码。

如果图方便，也可以直接将 `tendbcluster_user_admin_pass`、`tendbcluster_user_repl_pass` 设置为明文，那么在运行playbook时便不需要每次指定 `vault-id`

## 2 部署TenDBCluster

### 2.1 初始化环境
```
ansible-playbook -i hosts.tendbcluster init_common.yml
```

任何新机器加入，都需要对它初始化。

初始化的内容包括：
- 修改 limits.conf nofile
- 关闭 selinux
- 关闭 hugepage
- 关闭 iptables
- 修改 vm.swappiness=1
- 其它如 ip_local_port_range, tcp_tw_recycle 等

另外还会：
- 创建 mysql 用户
- 获取真实机器内存（使用 ansible 自带的 facts 在Docker富容器下可能获取的内存不对 ）

### 2.2 部署TenDB
```
ansible-playbook -i hosts.tendbcluster install_tendb.yml
ansible-playbook -i hosts.tendbcluster build_slave.yml

# if you set mysql password encrpyted:
ansible-playbook -i hosts.tendbcluster install_tendb.yml --vault-id password_file
ansible-playbook -i hosts.tendbcluster build_slave.yml --vault-id password_file
```
_install_tendb.yml_ 主要完成:
- TenDB master/slave 的安装，启动 TenDB node
- my.cnf 配置生成，包括 server_id 和 buffer pool 计算
- 给集群内访问来源授权用户权限。根据 inventory host 里面的 ip 授权 admin, replication 用户

_build_slave.yml_ 主要根据 role 和 master ，创建slave:
- 在 master 上通过 `mysqldump --single-transaction` 备份表和数据
- 在中控机上通过 `scp -3` 将备份拷贝到 slave
- 在 slave 上通过 mysql 恢复数据
- 使用 GTID 的方式 `change master`，建立主从关系

用户可以根据自己的喜好和实际情况，实现自己的slave重建逻辑编写 playbook，比如从冷备中心拉取物理备份，恢复出新 slave ，维护好 inventory host 里面的信息正确即可。

### 2.3 部署TSpider
```
ansible-playbook -i hosts.tendbcluster install_tspider.yml
```

_install_tspider.yml_ 主要完成：
- TSpider node 安装、启动
- my.cnf 配置生成，包括自动生成 autoinc_value 
- 给集群内访问来源授权用户权限

### 2.4 部署Tdbctl
```
ansible-playbook -i hosts.tendbcluster install_tdbctl.yml
```

_install_tdbctl.yml_ 主要完成：
- Tdbctl node 安装、启动、授权 TSpider node 访问
- my.cnf 配置生成，如果节点数 >=3 则启用 MGR
- 根据 inventory 的内容，将路由信息写入 Tdbctl `mysql.servers`，执行 `tdbctl flush routing`

此时可以登录 Tdbctl 看下各路由是否正常、MGR 工作是否正常：
```
MySQL [(none)]> select * from mysql.servers;
+-------------+----------------+----+--------------+--------------+-------+--------+---------+-------+
| Server_name | Host           | Db | Username     | Password     | Port  | Socket | Wrapper | Owner |
+-------------+----------------+----+--------------+--------------+-------+--------+---------+-------+
| SPIDER0     | 192.168.1.106  |    | tendbcluster | tendbcluster | 25000 |        | SPIDER  |       |
| SPIDER1     | 192.168.1.26   |    | tendbcluster | tendbcluster | 25000 |        | SPIDER  |       |
| SPIDER2     | 192.168.1.2    |    | tendbcluster | tendbcluster | 25000 |        | SPIDER  |       |
| SPT0        | 192.168.1.95   |    | tendbcluster | tendbcluster | 20000 |        | mysql   |       |
| SPT1        | 192.168.1.95   |    | tendbcluster | tendbcluster | 20001 |        | mysql   |       |
| SPT2        | 192.168.1.2    |    | tendbcluster | tendbcluster | 20002 |        | mysql   |       |
| SPT3        | 192.168.1.2    |    | tendbcluster | tendbcluster | 20003 |        | mysql   |       |
| TDBCTL0     | 192.168.1.26   |    | tendbcluster | tendbcluster | 26000 |        | TDBCTL  |       |
| TDBCTL1     | 192.168.1.2    |    | tendbcluster | tendbcluster | 26000 |        | TDBCTL  |       |
| TDBCTL2     | 192.168.1.106  |    | tendbcluster | tendbcluster | 26000 |        | TDBCTL  |       |
+-------------+----------------+----+--------------+--------------+-------+--------+---------+-------+
10 rows in set (0.00 sec)

MySQL [(none)]> select * from performance_schema.replication_group_members;
+---------------------------+--------------------------------------+----------------+-------------+--------------+
| CHANNEL_NAME              | MEMBER_ID                            | MEMBER_HOST    | MEMBER_PORT | MEMBER_STATE |
+---------------------------+--------------------------------------+----------------+-------------+--------------+
| group_replication_applier | 4b3fecb6-8547-11ea-8533-1405094fa102 | 192.168.1.2    |       26000 | ONLINE       |
| group_replication_applier | 4b7e4a68-8547-11ea-901a-14050942d51a | 192.168.1.26   |       26000 | ONLINE       |
| group_replication_applier | 4b80af29-8547-11ea-b347-140564753c6a | 192.168.1.106  |       26000 | ONLINE       |
+---------------------------+--------------------------------------+----------------+-------------+--------------+
3 rows in set (0.00 sec)
```

### 2.5 部署监控-暂缺

```
ansible-playbook -i hosts.tendbcluster deploy_monitor.yml
```

可以登录任意一台 TSpider，创建库表验证一下。


## 3 一键部署
一键部署将上面的步骤串在 `deploy_tendbcluster.yml` 一个 playbook 里
```
ansible-playbook -i hosts.tendbcluster deploy_tendbcluster.yml

# if you set mysql password encrypted
ansible-playbook -i hosts.tendbcluster deploy_tendbcluster.yml --vault-id --vault-id password_file
```