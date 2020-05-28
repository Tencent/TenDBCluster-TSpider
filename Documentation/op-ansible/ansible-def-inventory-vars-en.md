# Ansible Playbook Instructions

This document contains instructions for Ansible host inventory and the `group_vars` variable.

See also: [TenDB Cluster Ansible Playbook Repository](https://git.code.oa.com/tenstack/TenDBCluster-Ansible)

## Host Inventory Instructions

As an example, we define a topology of TenDB Cluster as:
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

In this topology a small grid represents a TenDB/TSpider/Tdbctl instance and a large grid represents a host. From it we can see there are two TenDB instances on one host.

Accordingly, *hosts.tendbcluster* is defined as:
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

Note:

1. All `inventory_hostname`s must be distinct.

2. Hostname-Port (`ansible_host` : `mysql_port`) pairs of Tdbctl, TSpider and TenDB nodes must be distinct. 
  The `server_id` under the same shard of TenDB is auto generated; therefore, in a GTID master-slave environment, `server_id`s are guaranteed distinct. 
  Please note that if some TenDB nodes are added or deleted, or the order of hosts are changed, the `server_id` generated based on `groups[mysql_shard].index(inventory_hostname)` will change accordingly; in this case, if TenDB nodes are not restarted, it may lead to duplicate `server_id`s. 
  The safest way to approach this is to set the `server_id` variable for every host.

3. The `mysql_shard` variable of hosts under the same shard of TenDB must be the same, and has to follow a format of *`SPT`+sequence_number*.

4. Each host of TenDB must have a role configured to either a master or slave. 
  If a master/slave switch takes place later, the configured roles have to be updated in time, otherwise it may cause problems for the entire cluster.

5. For TenDB, we suggest a slave use the same port as its master's. 
  A master and a slave should be deployed on different hosts.

6. In TSpider, the `autoinc_value` variable auto increases every time a new node joins. 
  If not in TSpider, it is maintained based on indexes. Therefore, it is not recommended to change the indexes. 
  Similarly, the `server_id` in TSpider is auto generated and auto increases as such.

7. If there are at least 3 Tdbctl nodes, Tdbctl will be automatically configured to MGR mode during installation. 
  In production, we suggest Tdbctl be in MGR mode, otherwise DDL operations will fail if Tdbctl ever undergoes a breakdown.

8. Global variables:
    - `tendbcluster_shard_num`
    Number of shards, i.e., number of TenDB instances.
    Each shard must be given a precise definition (eg. `SPT0`, `SPT1`, `SPT2`, `SPT3` as defined above).

    - `tendbcluster_name`
    Name of the current TenDBCluster, used to identify a cluster during monitoring.

    - `node_status` (to be implemented)
    Status of a node (set to `1` by default).
        - `1`: Working normally or expected to work normally.
        - `2`: Expected to work normally but confirmed to have broken down.
        - `3`: Waiting for retiring and switching offline.
        - `4`: Currently offline, can be safely cleared out of the inventory.

    - `tendbcluster:vars`
    Key variables that have great influence in cluster performance, such as `mysql_home_dir` (directory path for MySQL data storage) and `innodb_buffer_pool_size_mb` (size of InnoDB buffer pool).
        - `mysql_home_dir`, `mysql_data_dir`
        When multiple instances are deployed on a same host, `mysql_port` is used to separate data storage directories. If your hosts have multiple high-performance disks, you can directly set the `mysql_data_dir` variable for each host.
        - `innodb_buffer_pool_size_mb`
        By default, the size of buffer pool of each instance is calculated based on the number of instances on a host inferred from the inventory. The `innodb_buffer_pool_size_pct_total` variable in *group_vars* is set at `0.8` by default, making 80% of total memory being used. If you want to set a different buffer pool for a specific instance, you can overwrite the global configuration by setting the value of this variable for your host (unit: `MB`).

## group_vars Variable Instructions

group_vars/all:

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

Note:

- `fileserver`
The http url for downloading packages for installation.

- `mysql_version`, `mysql_pkg`, `mysql_pkg_md5`
TenDB package's version, name and MD5 checksum. Similarly, TSpider and Tdbctl packages are specified as such.

- `innodb_buffer_pool_size_pct_total`
  Percentage of TenDB's memory usage to total memory on a host. 
  For example, say a host has a total memory of `16384 MB`, the size of buffer pool is then set as `int(16384 * 0.8) = 13107 MB`. If 4 shards are deployed on this host, then each instance's `innodb_buffer_pool_size` is `int(13107 / 4) = 3276 MB`. 
  You can also manually set a specific size in the host inventory; in this case, the result of auto calculation will be overwritten.

- `mycnf_mysqld`
  Use it to customize the `mysqld` section in the *my.cnf* file.
  If you want to have different configurations for TenDB and TSpider, modify `group_vars/tendb` and `group_vars/tspider` respectively. 
  Note that the generation of *my.cnf* file does not completely depend on `mycnf_mysqld`; there are some built-in configurations in the *roles/tendb/templates/my.cnf.tendb.j2* file.

- `backup_dir`
Directory for MySQL backups. It is mainly used to store backups from `mysqldump` during "build slave".

- `tendbcluster_user_admin`, `tendbcluster_user_repl`
User accounts that have to be created for the cluster to run. They are automatically created during the deployment of cluster and are used for maintaining the cluster.
    - `tendbcluster_user_admin` Name of the admin user. The access IP is restricted during the creation of the admin user. Tdbctl will use this user account to create tables in each node.
    - `tendbcluster_user_admin_pass` Password of the admin user. We suggest using `ansible-vault` for password encryption.
    - `tendbcluster_user_repl` Name of the MySQL replication user. Replication privileges are granted to this user from any access IP (using `%`).
    - `tendbcluster_user_repl_pass` Password of the MySQL replication user.
    - `tendbcluster_user_root_pass` Password of the MySQL root account. Its value is the same as `tendbcluster_user_admin_pass`'s by default. Root account is not needed after deployment.