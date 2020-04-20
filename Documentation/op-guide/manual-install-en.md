# TenDB Cluster Manul Install
The most common way to deploy TenDB Cluster is using multiple server instances, to provide high availability. It is also possible to deploy TenDB Cluster locally, for example for testing purposes. 
This section explains how you can deploy TenDB Cluster locally with three Proxy layer TSpider, four Data layer TenDB, three Control layer Tdbctl on one physical machine

<a id="manual-install"></a>

## Deploying TenDB Node


<a id="jump1"></a>

Install four TenDB nodes with port 20000~20003. Each TenDB instance requires a specific data directory, an initialize each one. 

The following uses 20000 port as an example to deploy instance.

### Create Configuration File 
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

### Create Data Directory 
```bash
#use mysql user
mkdir /home/mysql/mysqldata/20000/data
mkdir -p /home/mysql/mysqldata/20000/innodb/data
mkdir -p /home/mysql/mysqldata/20000/innodb/log
mkdir -p /home/mysql/mysqldata/20000/relay-log
mkdir -p /home/mysql/mysqldata/20000/tmp
```
### Install TenDB

Download TenDB generic binary and unpack to `/usr/local`
```bash
cd /usr/local
#extract
tar xzvf  mysql-5.7.20-linux-x86_64-tmysql-3.1.5-gcs.tar.gz
ln -s mysql-5.7.20-linux-x86_64-tmysql-3.1.5-gcs mysql
chown -R mysql mysql mysql-5.7.20-linux-x86_64-tmysql-3.1.5-gcs
#init TenDB 
cd /usr/local/mysql && ./bin/mysqld --defaults-file=/home/mysql/my.cnf.20000 --initialize-insecure --user=mysql
#start TenDB 
./bin/mysqld_safe --defaults-file=/home/mysql/my.cnf.20000 --user=mysql &
```
### Deploying Another Three Instances 
Another instances's deployment is essentially the same sequence of steps as 20000 instance, except for things such as the `port` in my.cnf.   

## Deploying TSpider Node
Deploying two TSpider nodes with port 25000,250001. Each TSpider instance requires a specific data directory, an initialize each one. 

The following uses 25000 port as an example to deploy instance.

### Create Configuration File 
```bash
touch /home/mysql/my.cnf.25000
```
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
#### Important Variables Reference
- ddl_execute_by_ctl  
>Whether TSpider node support do DDL. If on, all DDL received by TSpider node will be directly forwarded to Tdbctl node, TSpider node not execute DDL immediately. After Tdbclt node received DDL, it will rewrite and then parallel send to each TSpider node, TenDB node execute.
- spider_auto_increment_mode_switch  
>whether enable global non-continuous unique increment.
- spider_auto_increment_mode_value  
>Global non-continuous unique mode number, each TSpider node can't be repeated. 
- spider_auto_increment_step  
>Global non-continuous unique increment step, all TSpider nodes must be the same.


### Create Data Directory 
```bash
#use mysql user
mkdir -p /home/mysql/mysqldata/25000/data
mkdir -p /home/mysql/mysqldata/25000/innodb/data
mkdir -p /home/mysql/mysqldata/25000/innodb/log
mkdir -p /home/mysql/mysqldata/25000/tmp
```

### Install TSpider

<a id="jump2"></a>

Download TSpider generic binary and unpack to `/usr/local`

```bash
cd /usr/local
#extract
tar xzvf  mariadb-10.3.7-linux-x86_64-tspider-3.4.5-gcs.tar.gz
ln -s mariadb-10.3.7-linux-x86_64-tspider-3.4.5-gcs tspider
chown -R mysql tspider mariadb-10.3.7-linux-x86_64-tspider-3.4.5-gcs
#init TSpider node
cd /usr/local/tspider && ./scripts/mysql_install_db --defaults-file=/home/mysql/my.cnf.25000
#start TSpider node 
./bin/mysqld_safe --defaults-file=/home/mysql/my.cnf.25000 --user=mysql &
```

### Deploying Another Instance 
Another instances's deployment is essentially the same sequence of steps as 25000 instance, except for things such as the `port`, `spider_auto_increment_mode_value` in my.cnf.

## Deploying Tdbctl Node
Use [Group Replication](https://dev.mysql.com/doc/refman/5.7/en/group-replication.html) to deploy three Tdbctl nodes, ports are 26000~26002.  
We use Single-Primary mode here, 26000 instance is Primary, 26001 and 26002 are secondaries which read-only.  

The following uses 26000 port as an example to deploy instance.
### Create Configuration File 

<a id="mgr-cnf"></a>

```bash
touch /home/mysql/my.cnf.26000
```
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
#MGR Configuration#############
#each instance must be different
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
#each instance must be different
loose-group_replication_local_address= "127.0.0.1:46000"
loose-group_replication_group_seeds= "127.0.0.1:46000,127.0.0.1:46001,127.0.0.1:46002"
loose-group_replication_bootstrap_group=off
loose-group_replication_single_primary_mode=on
loose-group_replication_ip_whitelist='127.0.0.1'
```
#### Important Variables Reference
- group_replication_single_primary_mode  
>Instructs the group to automatically pick a single server to be the one that handles read/write workload. This server is the PRIMARY and all others are SECONDARIES.
- group_replication_local_address  
>The network address which the member provides for connections from other members, specified as a host:port formatted string. This address must be reachable by all members of the group.

### Create Data Directory 
```bash
#use mysql user
mkdir -p /home/mysql/mysqldata/26000/data
mkdir -p /home/mysql/mysqldata/26000/binlog
mkdir -p /home/mysql/mysqldata/26000/relay-log
```

### Install Tdbctl 
Download Tdbctl generic binary and unpack to `/usr/local`

```bash
cd /usr/local
#extract
tar xzvf  tdbctl-1.4-linux-x86_64.tar.gz
ln -s tdbctl-1.4-linux-x86_64 tdbctl
chown -R mysql tdbctl tdbctl-1.4-linux-x86_64
#init Tdbctl
cd /usr/local/tdbctl && ./bin/mysqld --defaults-file=/home/mysql/my.cnf.26000 --initialize-insecure --user=mysql
#start Tdbctl
./bin/mysqld_safe --defaults-file=/home/mysql/my.cnf.26000 --user=mysql &
```

### Configuration of Local Group Replication Members 
After three nodes stated, you need to connect each node do configure
```sql
#each nodes need to do
SET SQL_LOG_BIN=0;grant REPLICATION SLAVE ON *.* TO 'repl'@'%' identified by 'repl';
SET SQL_LOG_BIN=1;
reset master;
change master to master_user='repl',master_password='repl' for channel 'group_replication_recovery';
INSTALL PLUGIN group_replication SONAME 'group_replication.so';
```
- Start Primary Instance 
```sql
#only Primary instance need to do, here is 26000
SET GLOBAL group_replication_bootstrap_group=ON;
START GROUP_REPLICATION;
SET GLOBAL group_replication_bootstrap_group=OFF;
```

- Start Secondary Instance 
```sql
#26001, 26002 instance do this
START GROUP_REPLICATION;
```

## Configuration TenDB Cluster 
<a id="cluster-privilege"></a>
After all nodes had started and configured, we need to configure them as an TenDB Cluster.  
The privileges configure below are for reference only, the actual access control needs to be considered  with application security.
About Cluster's privileges, refrer to [Account Management](cluster-grant-en.md)


### Configuration TenDB Privileges
<a id="jump3"></a>

Connect each TenDB node and do GRANT. Below use 20000 instance as example.
> mysql -uroot --socket=/home/mysql/mysqldata/20000/mysql.sock
```sql
create user mysql@'127.0.0.1' identified by 'mysql';
grant all privileges on *.* to mysql@'127.0.0.1' with grant option;
```

### Configuration Tdbctl Privileges
>mysql -uroot --socket=/home/mysql/mysqldata/26000/mysql.sock
```sql
#only primary instance need
create user mysql@'127.0.0.1' identified by 'mysql';
grant all privileges on *.* to mysql@'127.0.0.1' with grant option;
```

### Configuration TSpider Privileges
Connect each TSpider node and do GRANT. Below use 25000 instance as example.
>mysql -uroot --socket=/home/mysql/mysqldata/25000/mysql.sock
```sql
# temporary set to off, disable the CREATE/GRANT send to Tdbctl node
set ddl_execute_by_ctl=off;
create user mysql@'127.0.0.1' identified by 'mysql';
grant all privileges on *.* to mysql@'127.0.0.1' with grant option;
```

### Configuration Cluster 
connecto to PRIMARY Tdbctl node, configure routing table mysql.servers.
>mysql -umysql -pmysql -h127.0.0.1 -P26000
```sql
#insert routing rules.
insert into mysql.servers values('SPT0','127.0.0.1','','mysql','mysql',20000,'','mysql','');
insert into mysql.servers values('SPT1','127.0.0.1','','mysql','mysql',20001,'','mysql','');
insert into mysql.servers values('SPT2','127.0.0.1','','mysql','mysql',20002,'','mysql','');
insert into mysql.servers values('SPT3','127.0.0.1','','mysql','mysql',20003,'','mysql','');
insert into mysql.servers values('SPIDER0','127.0.0.1','','mysql','mysql',25000,'','SPIDER','');
insert into mysql.servers values('SPIDER1','127.0.0.1','','mysql','mysql',25001,'','SPIDER','');
insert into mysql.servers values('TDBCTL0','127.0.0.1','','mysql','mysql',26000,'','TDBCTL','');
insert into mysql.servers values('TDBCTL1','127.0.0.1','','mysql','mysql',26001,'','TDBCTL','');
insert into mysql.servers values('TDBCTL2','127.0.0.1','','mysql','mysql',26002,'','TDBCTL','');
#flush routing
tdbctl flush routing;
```

### Cluster verification 
#### Connect Cluster 
>mysql -umysql -pmysql -h127.0.0.1 -P25000

#### Create Database/Table 
After Cluster configure completed, you can operator Cluster like single MySQL instance.
```sql
create database test1;
use test1;
create table t1(a int primary key, b int);
# view table structure and sharding information.
show create table t1;
#Table structure on TSpider node，
#Compared with the original table, there is additional sharding information. You can see that the DATABASE name is different in each TenDB, which ending with _0 ~ N
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
Connect to anyone TenDB Node to view table structure
> mysql -umysql -pmysql -h127.0.0.1 -P20000
```sql
#On TenDB node，database name is end with number suffix _N,
use test1_0;
show create table t1;
#table name is same with spider, but no sharding information
CREATE TABLE `t1` (
  `a` int(11) NOT NULL,
  `b` int(11) DEFAULT NULL,
  PRIMARY KEY (`a`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
```

#### DDL & DML 
Connect to anyone TSpider node
>mysql -umysql -pmysql -h127.0.0.1 -P25000
```sql
use test1;
#sharding algorithm is crc32(a)%4, insert blow data to enusre each TenDB node store one record
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
Connect to TenDB node 20000
>mysql -umysql -pmysql -h127.0.0.1 -P20000
```sql
use test1_0;
select * from t1;
#exist one record: crc(4)%4为0
+---+------+
| a | b    |
+---+------+
| 4 |    5 |
+---+------+
```
Connect to Tspider node 25000 and DROP DATABAE
>mysql -umysql -pmysql -h127.0.0.1 -P25000
```sql
drop database test1;
```

## Note 
If a TenDB cluster is deployed with multiple TSpider nodes, in order to simplify the configuration on the application side, and at the same time, if TSpider node scaling, faileover thus you had to modify the application's connection configuration also, it's recommended to use Name Service.