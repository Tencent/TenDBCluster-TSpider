# TenDB Cluster手工部署
本章节描述如何在一个单机上部署TenDB Cluster集群
下文将会部署一个2个TSpider节点，4个TenDB节点，3个Tdbctl节点(MGR方案)的集群
<a id="manual-install"></a>
## 部署TenDB

<a id="jump1"></a>

部署4个后端节点，端口号依次为20000~20003
下文以20000端口为例说明，其他3个实例更改相应端口号即可

### 创建配置文件
> touch /home/mysql/my.cnf.20000
```sql
[client]
port=20000
socket=/home/mysql/mysqldata/20000/mysql.sock
[mysqld]
default-storage-engine=innodb
skip-name-resolve
datadir=/home/mysql/mysqldata/20000/data
character-set-server=utf8
innodb_data_home_dir=/home/mysql/mysqldata/20000/innodb/data
innodb_file_per_table=1
innodb_flush_log_at_trx_commit=0
innodb_lock_wait_timeout=50
innodb_log_buffer_size=2048M
innodb_log_file_size=256M
innodb_log_files_in_group=4
innodb_log_group_home_dir=/home/mysql/mysqldata/20000/innodb/log
slow_query_log_file=/home/mysql/mysqllog/20000/slow-query.log
max_binlog_size=256M
port=20000
relay-log=/home/mysql/mysqldata/20000/relay-log/relay-log.bin
replicate-ignore-db=mysql
replicate-wild-ignore-table=mysql.%
server_id=241
skip-external-locking
socket=/home/mysql/mysqldata/20000/mysql.sock
tmpdir=/home/mysql/mysqldata/20000/tmp
wait_timeout=86400
[mysql]
default-character-set=utf8
port=20000
socket=/home/mysql/mysqldata/20000/mysql.sock                                          
```

### 创建目录
```bash
#创建TenDB初始化时需要的目录,mysql用户
mkdir /home/mysql/mysqldata/20000/data
mkdir -p /home/mysql/mysqldata/20000/innodb/data
mkdir -p /home/mysql/mysqldata/20000/innodb/log
mkdir -p /home/mysql/mysqldata/20000/relay-log
mkdir -p /home/mysql/mysqldata/20000/tmp
```
### 安装TenDB

下载TenDB介质包到/usr/local
```bash
cd /usr/local
#解压介质
tar xzvf  mysql-5.7.20-linux-x86_64-tmysql-3.1.5-gcs.tar.gz
ln -s mysql-5.7.20-linux-x86_64-tmysql-3.1.5-gcs mysql
chown -R mysql mysql mysql-5.7.20-linux-x86_64-tmysql-3.1.5-gcs
# 初始化mysql
cd /usr/local/mysql && ./bin/mysqld --defaults-file=/home/mysql/my.cnf.20000 --initialize-insecure --user=mysql
# 启动mysql
./bin/mysqld_safe --defaults-file=/home/mysql/my.cnf.20000 --user=mysql &
```
### 部署另外3个实例
重复步骤1,2,3部署其他20001~20003实例，部署前需要替换my.cnf文件中的端口号等

## 部署TSpider
部署2个接入层节点TSpider，端口号依次为25000, 25001
下文以25000端口为例说明，25001实例更改相应端口号即可

### 创建配置文件
```bash
touch /home/mysql/my.cnf.25000
```
配置文件内容如下
```sql
[client]
port=25000
socket=/home/mysql/mysqldata/25000/mysql.sock
[mysqld]
ddl_execute_by_ctl=on
default-storage-engine=innodb
skip-name-resolve
performance_schema=OFF
log_slow_admin_statements=ON
alter_query_log=ON
query_response_time_stats=ON
innodb_flush_method=O_DIRECT
slow_query_log=1
core-file
datadir=/home/mysql/mysqldata/25000/data
character-set-server=utf8mb4
innodb_buffer_pool_size=120M
innodb_data_file_path=ibdata1:1G:autoextend
innodb_data_home_dir=/home/mysql/mysqldata/25000/innodb/data
innodb_file_per_table=1
innodb_flush_log_at_trx_commit=0
innodb_log_files_in_group=4
innodb_log_group_home_dir=/home/mysql/mysqldata/25000/innodb/log
log_bin_trust_function_creators=1
slow_query_log_file=/home/mysql/mysqldata/25000/slow-query.log
port=25000
replicate-wild-ignore-table=mysql.%
skip-external-locking
skip-symbolic-links
socket=/home/mysql/mysqldata/25000/mysql.sock
spider_bgs_mode=1
spider_get_sts_or_crd=1
spider_index_hint_pushdown=1
tmpdir=/home/mysql/mysqldata/25000/tmp
sql_mode=''
spider_auto_increment_mode_switch=1
spider_auto_increment_mode_value=1
spider_auto_increment_step=37
[mysql]
default-character-set=utf8mb4
no-auto-rehash
port=25000
socket=/home/mysql/mysqldata/25000/mysql.sock                                        
```
#### 重要参数说明
- ddl_execute_by_ctl  
集群支持DDL变更下，此参数必须在TSpider节点开启，确保在此节点执行的DDL语句会转发给Tdbctl节点，由tdbctl节点同步DDL语句到集群的各个TSpider节点和TenDB节点
- spider_auto_increment_mode_switch  
是否启用全局非唯一自增键功能
- spider_auto_increment_mode_value  
集群全局非唯一自增起始值，集群各TSpider节点配置不能重复
- spider_auto_increment_step  
集群全局非唯一自增步长，集群各TSpider节点配置相同


### 创建目录
```
#mysql用户
mkdir -p /home/mysql/mysqldata/25000/data
mkdir -p /home/mysql/mysqldata/25000/innodb/data
mkdir -p /home/mysql/mysqldata/25000/innodb/log
mkdir -p /home/mysql/mysqldata/25000/tmp
```

### 安装TSpider

<a id="jump2"></a>

下载TSpider介质包到/usr/local

```bash
cd /usr/local
#解压软链介质
tar xzvf  mariadb-10.3.7-linux-x86_64-tspider-3.4.5-gcs.tar.gz
ln -s mariadb-10.3.7-linux-x86_64-tspider-3.4.5-gcs tspider
chown -R mysql tspider mariadb-10.3.7-linux-x86_64-tspider-3.4.5-gcs
# 初始化TSpider
cd /usr/local/tspider && ./scripts/mysql_install_db --defaults-file=/home/mysql/my.cnf.25000
# 启动TSpider
./bin/mysqld_safe --defaults-file=/home/mysql/my.cnf.25000 --user=mysql &
```

### 部署其他节点
重复步骤1,2,3部署25001实例，部署前需要替换my.cnf文件中的端口号，同时修改spider_auto_increment_mode_value=2

## 部署Tdbctl
采用MGR复制方案部署3个中控节点Tdbctl，端口号分别为26000, 26001, 26002，其中26000为主节点，26000、26001为只读从节点。
关于MGR复制方案的细节，可以参考[组复制](https://dev.mysql.com/doc/refman/5.7/en/group-replication.html)  
下面以26000为例说明
### 创建配置文件

<a id="mgr-cnf"></a>

```bash
touch /home/mysql/my.cnf.26000
```

配置内容如下
```sql
[client]
port=26000
socket=/home/mysql/mysqldata/26000/mysql.sock
[mysqld]
datadir=/home/mysql/mysqldata/26000/data
socket=/home/mysql/mysqldata/26000/mysql.sock
port=26000
[mysql]
port=26000
socket=/home/mysql/mysqldata/26000/mysql.sock
log_bin=/home/mysql/mysqldata/26000/binlog/binlog26000.bin
relay-log=/home/mysql/mysqldata/26000/relay-log/relay-log.bin
#MGR配置如下#############
#server_id不同实例不能相同
server_id=1
gtid_mode=ON
enforce_gtid_consistency=ON
master_info_repository=TABLE
relay_log_info_repository=TABLE
binlog_checksum=NONE
log_slave_updates=ON
binlog_format=ROW
report_host=127.0.0.1
transaction_write_set_extraction=XXHASH64
loose-group_replication_group_name="aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa"
loose-group_replication_start_on_boot=off
loose-group_replication_local_address= "127.0.0.1:46000"
loose-group_replication_group_seeds= "127.0.0.1:46000,127.0.0.1:46001,127.0.0.1:46002"
loose-group_replication_bootstrap_group=off
#loose-group_replication_single_primary_mode=off
loose-group_replication_ip_whitelist='127.0.0.1'
```
#### MGR重要参数说明
- group_replication_single_primary_mode  
在单主模式下，需要设定为on, 默认为on
- group_replication_local_address  
MGR通信的端口，不能与实例运行的socket端口冲突，例如本实例服务端口为26000，MGR通信端口设定为46000

### 创建目录
```bash
#mysql用户
mkdir -p /home/mysql/mysqldata/26000/data
mkdir -p /home/mysql/mysqldata/26000/binlog
mkdir -p /home/mysql/mysqldata/26000/relay-log
```

### 安装Tdbctl
下载Tdbctl介质包到/usr/local

```bash
cd /usr/local
#解压软链介质
tar xzvf  tdbctl-1.4-linux-x86_64.tar.gz
ln -s tdbctl-1.4-linux-x86_64 tdbctl
chown -R mysql tdbctl tdbctl-1.4-linux-x86_64
# 初始化Tdbctl
cd /usr/local/tdbctl && ./bin/mysqld --defaults-file=/home/mysql/my.cnf.26000 --initialize-insecure --user=mysql
# 启动Tdbctl
./bin/mysqld_safe --defaults-file=/home/mysql/my.cnf.26000 --user=mysql &
```

### 配置MGR复制方案
在3个节点启动后，分别连接节点执行如下操作
```sql
SET SQL_LOG_BIN=0;grant REPLICATION SLAVE ON *.* TO 'repl'@'%' identified by 'repl';
SET SQL_LOG_BIN=1;
reset master;
change master to master_user='repl',master_password='repl' for channel 'group_replication_recovery';
INSTALL PLUGIN group_replication SONAME 'group_replication.so';
```
- 启动主节点
```sql
SET GLOBAL group_replication_bootstrap_group=ON;
START GROUP_REPLICATION;
SET GLOBAL group_replication_bootstrap_group=OFF;
```

- 启动从节点
```sql
START GROUP_REPLICATION;
```



## 配置集群
<a id="cluster-privilege"></a>

示例权限仅供参考，实际权限控制需结合应用访问安全考虑

### 配置TenDB权限
<a id="jump3"></a>

分别连接TenDB 20000~20003授权，下面以20000端口为例，其他实例替换相应端口操作即可
> mysql -uroot --socket=/home/mysql/mysqldata/20000/mysql.sock
```sql
create user mysql@'127.0.0.1' identified by 'mysql';
grant all privileges on *.* to mysql@'127.0.0.1' with grant option;
```

### 配置Tdbctl权限
>mysql -uroot --socket=/home/mysql/mysqldata/26000/mysql.sock
```sql
create user mysql@'127.0.0.1' identified by 'mysql';
grant all privileges on *.* to mysql@'127.0.0.1' with grant option;
```

### 配置TSpider权限
分别连接TSpider 25000, 25001授权
下面以25000端口为例,25001替换相应端口即可
>mysql -uroot --socket=/home/mysql/mysqldata/25000/mysql.sock
```sql
# 设定off，确保此操作不会发送给中控节点
set ddl_execute_by_ctl=off;
create user mysql@'127.0.0.1' identified by 'mysql';
grant all privileges on *.* to mysql@'127.0.0.1' with grant option;
```

### 配置集群
连接Tdbctl节点，配置mysql.servers路由表
>mysql -umysql -pmysql -h127.0.0.1 -P26000
```sql
#插入路由信息
insert into mysql.servers values('SPT0','127.0.0.1','','mysql','mysql',20000,'','mysql','');
insert into mysql.servers values('SPT1','127.0.0.1','','mysql','mysql',20001,'','mysql','');
insert into mysql.servers values('SPT2','127.0.0.1','','mysql','mysql',20002,'','mysql','');
insert into mysql.servers values('SPT3','127.0.0.1','','mysql','mysql',20003,'','mysql','');
insert into mysql.servers values('SPIDER0','127.0.0.1','','mysql','mysql',25000,'','SPIDER','');
insert into mysql.servers values('SPIDER1','127.0.0.1','','mysql','mysql',25001,'','SPIDER','');
insert into mysql.servers values('TDBCTL0','127.0.0.1','','mysql','mysql',26000,'','TDBCTL','');
insert into mysql.servers values('TDBCTL1','127.0.0.1','','mysql','mysql',26001,'','TDBCTL','');
insert into mysql.servers values('TDBCTL2','127.0.0.1','','mysql','mysql',26002,'','TDBCTL','');
#刷新路由，同步路由到TSpider节点
tdbctl flush routing;
```

### 集群验证
#### 连接集群
>mysql -umysql -pmysql -h127.0.0.1 -P25000

#### 创建库表
配置完成后，可以像操作单机MySQL一样操作集群，具体集群语法限制、扩展参见语法文档
```sql
create database test1;
use test1;
# 创建表需要指定唯一键，否则会报错
create table t1(a int primary key, b int);
# 查看分片情况
show create table t1;
#TSpider节点表结构如下，
#在原表结构基础上，多了分片信息，可以看到库名在各个TenDB分片是不同的，以_0~N结尾
CREATE TABLE `t1` (
  `a` int(11) NOT NULL,
  `b` int(11) DEFAULT NULL,
  PRIMARY KEY (`a`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8mb4
 PARTITION BY LIST (crc32(`a`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "test1_0", table "t1", server "SPT0"' ENGINE = SPIDER,
 PARTITION `pt1` VALUES IN (1) COMMENT = 'database "test1_1", table "t1", server "SPT1"' ENGINE = SPIDER,
 PARTITION `pt2` VALUES IN (2) COMMENT = 'database "test1_2", table "t1", server "SPT2"' ENGINE = SPIDER,
 PARTITION `pt3` VALUES IN (3) COMMENT = 'database "test1_3", table "t1", server "SPT3"' ENGINE = SPIDER)
```
连接任意一个TenDB节点查看表结构
> mysql -umysql -pmysql -h127.0.0.1 -P20000
```sql
#在TenDB节点，库名会跟TSpider多一个后缀_N,
use test1_0;
show create table t1;
#一个正常的InnoDB表，字段定义与TSpider相同
CREATE TABLE `t1` (
  `a` int(11) NOT NULL,
  `b` int(11) DEFAULT NULL,
  PRIMARY KEY (`a`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
```

#### DDL & DML 操作
通过连接TSpider节点操作整个集群，sql语法与单机MySQL大致相同
>mysql -umysql -pmysql -h127.0.0.1 -P25000
```sql
use test1;
#crc32(a)%4,插入如下数据，确保每个分片都存在一条
insert into t1 values(1,2);
insert into t1 values(2,3);
insert into t1 values(4,5);
insert into t1 values(7,8);
select * from t1;
+---+------+
| a | b    |
+---+------+
| 4 |    5 |
| 2 |    3 |
| 7 |    8 |
| 1 |    2 |
+---+------+
```
连接20000端口的TenDB查看
>mysql -umysql -pmysql -h127.0.0.1 -P20000
```sql
use test1_0;
select * from t1;
#存在一条数据crc(4)%4为0
+---+------+
| a | b    |
+---+------+
| 4 |    5 |
+---+------+
```
连接Tspider25000端口清理库
>mysql -umysql -pmysql -h127.0.0.1 -P25000
```sql
drop database test1;
```

## 其他说明
部署多个TSpider接入层节点的集群，为了简化应用侧使用配置，同时因为故障、扩缩容等场景导致节点数变化而频繁修改连接配置，建议配合域名系统使用。  
将同一集群的各接入层节点接入域名系统，分配一个相同的域名，应用侧使用域名来访问集群。由于域名解析结果只能获取节点ip，因此在部署时尽量让同一集群分部在不同主机的TSpider节点使用相同端口号，能降低系统管理、使用的复杂度