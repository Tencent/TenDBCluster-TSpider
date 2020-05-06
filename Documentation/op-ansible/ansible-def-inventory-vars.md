# Ansible playbook说明

Ansible host inventory 说明，以及 `group_vars` 变量说明。
TenDB Cluster Ansible playbook 仓库：https://git.code.oa.com/tenstack/TenDBCluster-Ansible 。

## Inventory hosts说明

定义一个如下拓扑的 TenDB Cluster
```
                          +-------------------------+   +-------------------------+
                          |   +-----------------+   |   |   +-----------------+   |
                          |   |   tendb-spt0    | <-|---|-- |   tendb-spt0-1  |   |
                          |   +-----------------+   |   |   +-----------------+   |
                          |                         |   |                         |
+-----------------+       |   +-----------------+   |   |   +-----------------+   |
| tspider-node-01 |       |   |   tendb-spt1    | <-|---|-- |   tendb-spt1-1  |   |
+-----------------+       |   +-----------------+   |   |   +-----------------+   |
                          +-------------------------+   +-------------------------+
                                                                                   
                          +-------------------------+   +-------------------------+
+-----------------+       |   +-----------------+   |   |   +-----------------+   |
| tspider-node-02 |       |   |   tendb-spt2    | <-|---|-- |   tendb-spt2-1  |   |
+-----------------+       |   +-----------------+   |   |   +-----------------+   |
                          |                         |   |                         |
                          |   +-----------------+   |   |   +-----------------+   |
                          |   |   tendb-spt3    | <-|---|-- |   tendb-spt3-1  |   |
                          |   +-----------------+   |   |   +-----------------+   |
                          +-------------------------+   +-------------------------+
                                                                                   
                                                                                   
+----------------++----------------++----------------+                             
| tdbctl-node-01 || tdbctl-node-02 || tdbctl-node-03 |                             
+----------------++----------------++----------------+                             
```

每个小框是一个 TenDB/TSpider/Tdbctl 实例
每个大框代表一台机器，图所示一台机器2个实例

对应的 ***hosts.tendbcluster*** 定义 : 
```
[all:vars]
ansible_ssh_user=root
ansible_ssh_port=36000
ansible_ssh_private_key_file=/root/.ssh/tendbcluster_rsa
# set the ssh public key to remote hosts
ansible_ssh_public_key_file=/root/.ssh/tendbcluster_rsa.pub

[tendbcluster:vars]
tendbcluster_name=test
tendbcluster_shard_num=4
node_status=1

mysql_home_dir=/home/mysql/mysqldata
mysql_log_dir=/home/mysql/mysqllog
innodb_buffer_pool_size_mb={{innodb_buffer_pool_size_mb_auto}}
mysql_data_dir="{{mysql_home_dir}}/{{mysql_port}}"

################
[tendbcluster:children]
tendb
tspider
tdbctl

[tendb:children]
SPT0
SPT1
SPT2
SPT3

################
#### SPIDER ####
[tspider]
tspider-node-01 ansible_host=192.168.1.106 mysql_port=25000
tspider-node-02 ansible_host=192.168.1.26 mysql_port=25000

################
#### TDBCTL ####
[tdbctl]
tdbctl-node-01 ansible_host=192.168.1.26 mysql_port=26000
tdbctl-node-02 ansible_host=192.168.1.2 mysql_port=26000
tdbctl-node-03 ansible_host=192.168.1.106 mysql_port=26000

################
#### SPT0 ####
[SPT0]
tendb-spt0 ansible_host=192.168.1.95 mysql_shard=SPT0 mysql_port=20000 role=master
tendb-spt0-1 ansible_host=192.168.1.147 mysql_shard=SPT0 mysql_port=20000 role=slave master=tendb-spt0

#### SPT1 ####
[SPT1]
tendb-spt1 ansible_host=192.168.1.95 mysql_shard=SPT1 mysql_port=20001 role=master
tendb-spt1-1 ansible_host=192.168.1.147 mysql_shard=SPT1 mysql_port=20001 role=slave master=tendb-spt1

#### SPT2 ####
[SPT2]
tendb-spt2 ansible_host=192.168.1.2 mysql_shard=SPT2 mysql_port=20002 role=master
tendb-spt2-1 ansible_host=192.168.1.130 mysql_shard=SPT2 mysql_port=20002 role=slave master=tendb-spt2

#### SPT3 ####
[SPT3]
tendb-spt3 ansible_host=192.168.1.2 mysql_shard=SPT3 mysql_port=20003 role=master
tendb-spt3-1 ansible_host=192.168.1.130 mysql_shard=SPT3 mysql_port=20003 role=slave master=tendb-spt3
```

**说明：**

1. 所有inventory_hostname不得相同

2. Tdbctl, TSpider, TenDB 三个组件的主机名和端口( `ansible_host`:`mysql_port` ) 不得相同
  TenDB 同一分片下的 server_id 自动生成，所以在 GTID 主备环境下，server_id 不会相同。
  但注意如果增删 TenDB 节点，或者修改了 host 顺序之后，根据 `groups[mysql_shard].index(inventory_hostname)` 生成的 server_id 会变，如果没有同步重启 TenDB，可能会导致server_id 相同。
  最安全的情况是，这里给每个 host 设置 `server_id` 变量

3. TenDB同一分片下的 host 变量 `mysql_shard` 必须相同，且必须是 _`SPT` + 序号_ 格式

4. TenDB的各分片 host role 必须正确配置是 master 还是 slave。
  包括如果后续做了主备切换，这里必须及时更新，否则可能引起整个集群问题

5. TenDB推荐同一个分片，端口相同，不同副本/主从分布于不同机器

6. TSpider的变量 `autoinc_value` 每加一个节点，保持自增。
  在不是 TSpider 时会自动按照 index 来维持，所以部署好后，不要轻易改变这里的顺序
  同理 TSpider 的 server_id 也是自动生成的。

7. Tdbctl 如果只给一个节点，则安装成单节点，如果给 3 个节点，则自动安装成 MGR 模式
  正式环境建议安装成 MGR 模式，否则 Tdbctl 故障时无法做 DDL 操作。

8. 全局变量：
  - `tendbcluster_shard_num`
    设置分片个数，即 TenDB 实例个数。
    具体的分片定义，必须准确给出，如下面的 `SPT0`, `SPT1`, `SPT2`, `SPT3`

  - `tendbcluster_name`
    TenDBCluster 的集群名，主要用于监控时标识集群

  - `node_status` - 待实现
    节点状态，默认为`1`
    - `1`: 正常工作，或者预期正常工作的节点
    - `2`: 预期正常工作，但确认已经故障的节点
    - `3`: 节点等待退役下线
    - `4`: 节点已下线，可被安全从 inventory 中清除
  - `tendbcluster:vars`
    设置影响集群性能的关键变量，比如 mysql 数据目录 `mysql_home_dir`，innodb 缓冲池大小`innodb_buffer_pool_size_mb`
    
    - `mysql_home_dir`, `mysql_data_dir`
      单机多分片（多实例）情况下，默认通过 mysql_port 来区分数据目录。
      如果机器有多个高性能磁盘，也可以直接在 host 上设置变量 `mysql_data_dir`
    - `innodb_buffer_pool_size_mb`
      默认会根据 inventory 里面配置单机实例数，自动计算每个实例的 buffer pool 大小。
      在 *group_vars* 里面设置变量 `innodb_buffer_pool_size_pct_total`，默认使用总内存的 0.8 。
      当为特定实例设置不同的 buffer pool 时，直接在 host 上设置该变量来覆盖全局配置，单位 MB。

## group_vars变量说明

group_vars/all

```
fileserver: http://9.79.12.184:6080/pkgs
ansible_python_interpreter: /usr/bin/python

iface: eth1
mysql_version: "3.1.5-gcs"
mysql_pkg: "mysql-5.7.20-linux-x86_64-tmysql-{{mysql_version}}"
mysql_pkg_md5: 3ac2d2ff8a86e45a1d3ce671991fc7da

tspider_version: 3.4.5-gcs
tspider_pkg: "mariadb-10.3.7-linux-x86_64-tspider-{{tspider_version}}"
tspider_pkg_md5: 0de1abb88c9256fd9c35d22eaa03acb3

tdbctl_version: 1.4
tdbctl_pkg: "tdbctl-{{tdbctl_version}}-linux-x86_64"
tdbctl_pkg_md5: 0e155129bb3966ad1dc48b99d15c31c3

innodb_buffer_pool_size_pct_total: 0.8

# my.cnf [mysqld]
mycnf_mysqld:
  innodb_flush_log_at_trx_commit: 0
  character-set-server: utf8mb4

# my.cnf [mysql]
mycnf_mysql:
  default-character-set: utf8mb4

backup_dir: /data/dbbak
mysql_charset: utf8mb4
mysql_basedir: /usr/local/mysql
tspider_basedir: /usr/local/tspider
tdbctl_basedir: /usr/local/tdbctl
mysql_conf_dir: /home/mysql/etc

mysql_socket: "{{mysql_data_dir}}/mysql.sock"
mysql_pid_file: "{{mysql_data_dir}}/mysqld.pid"
mysql_conf_file: "{{mysql_conf_dir}}/my.cnf.{{mysql_port}}"

# user admin
tendbcluster_user_admin: tendbcluster
tendbcluster_user_admin_pass: !vault |
          $ANSIBLE_VAULT;1.2;AES256;mysql
          36633635623633313635633163353066616566643466376437303364393038313534353363396438
          3966613066616337373435383364366136373638616236640a643338303432343566343435643238
          66333466643939663035303366333838393831343538303535313338376136353431616665383965
          6466313035303533630a346466656565363933323636353139313266626531383636613134386162
          3430
# user repl
tendbcluster_user_repl: repl_user
tendbcluster_user_repl_pass: !vault |
          $ANSIBLE_VAULT;1.2;AES256;mysql
          38396436366461326139386230346561346432613262313830623763663433643233323633616330
          3937373130346437343636326631646431316132663534330a323161656137323631333065353366
          61633938346662636365653531633735616163323936326636636535373839633264396666626535
          3038373034656439360a666434643962303666393133313539653864303135636433633632373639
          6639
tendbcluster_user_root_pass: "{{tendbcluster_user_admin_pass}}"

```

- `fileserver`
  设置安装包的下载链接地址

- `mysql_version`, `mysql_pkg`, `mysql_pkg_md5`
  设置 TenDB 的介质包版本、名称、md5
  同理 TSpider, Tdbctl 也需要配置。

- `innodb_buffer_pool_size_pct_total`
  TenDB 实例占机器总内存比例，默认 0.8 。
  比如机器内存 16384MB，则 buffer pool 大小为 `int(16384 * 0.8) = 13107`，当机器上部署4个分片时，每个实例 innodb_buffer_pool_size 为 `int(13107 / 4) = 3276` M。
  也可以直接在 host inventory 里面配置具体的大小，来覆盖自动计算。

- `mycnf_mysqld`
  自定义 my.conf 配置文件里面 `mysqld` 的配置项。
  如果要为 TenDB, TSpider 的 my.cnf 配置不同的值，在对应的 `group_vars/tendb` 和 `group_vars/tspider` 里面设置

  注意 my.cnf 的生成没有完全依赖这个 mycnf_mysqld ，在 `roles/tendb/templates/my.cnf.tendb.j2` 里面内置了一些配置。使用时以防重复

- `backup_dir`
  MySQL备份目录，主要用在自动 build slave 时，mysqldump 的存放的备份的地址

- `tendbcluster_user_admin`, `tendbcluster_user_repl`
  集群运行所必须要创建的两个用户，在部署集群时，会自动创建，在后续维护集群时，也要用到管理用户。
  - `tendbcluster_user_admin` 指定管理用户名，创建 admin 用户时会限制访问ip。后续  Tdbctl 会用这个用户在各个节点创建表
  - `tendbcluster_user_admin_pass` 指定 admin 用户密码，建议使用 ansible-vault 加密，见上文
  - `tendbcluster_user_repl` 指定 mysql replication 的用户名，授权 `%` 。
  - `tendbcluster_user_repl_pass` 指定复制用户的密码
  - `tendbcluster_user_root_pass` 指定 mysql root 的密码，默认与 `tendbcluster_user_admin_pass` 相同。后续不再需要root