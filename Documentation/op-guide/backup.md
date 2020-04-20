# TenDB Cluster备份
由于TSpider本身不存储数据，数据都存储在后端TenDB。所以推荐在TSpider上只备份schema，在各个后端TenDB中备份数据。本文将介绍TSpider以及后端TenDB的备份。

## TSpider备份
TSpider和普通MySQL的逻辑备份并无差别，我们使用TSpider自带的mysqldump工具进行备份。但是<font color="#dd0000">不建议直接从TSpider备份全部数据。</font>如果没有中控节点，还需要备份mysql.servers数据; 
  
例如，备份 `test_spider1 test_spider2`库下所有的表结构的指令如下：
```
/home/mysql/dbbackup/mysqldump -uTSpider -pTSpider -S /data1/mysqldata/mysql.sock   --max-allowed-packet=1G  --default-character-set=utf8mb4 --single-transaction --no-autocommit=FALSE  --skip-opt --create-options  --routines  --quick --no-data --databases test_spider1 test_spider2
```

<font color="#dd0000">注意:</font>   
>需要指定--no-data,只导出数据库表结构



## TenDB备份
TenDB是TenDB Cluster集群的存储层，支持逻辑备份和物理备份。

### 逻辑备份
TenDB和普通MySQL的逻辑备份并无差别。

例如，备份 `test_spider1_0 test_spider2_0`库下所有数据的指令如下：
```
 /home/mysql/dbbackup/mysqldump -uTenDB -pTenDB -h"host" -P20000 --ignore-show-create-table-error --skip-foreign-key-check --max-allowed-packet=1G  --default-character-set=utf8 --single-transaction --no-autocommit=FALSE  --skip-opt --create-options  --routines  --master-data=2  --quick  --flush-wait-timeout=30 --databases test_spider1_0 test_spider2_0 
```


### 物理备份
TenDB采用percona的xtrabackup进行物理备份，细节可以参考[percona-xtrabackup官方文档](https://www.percona.com/doc/percona-xtrabackup/2.4/manual.html)

示例：
```
/home/mysql/dbbackup/xtrabackup --defaults-file=/etc/my.cnf.20000 --host="host" --port=20000 -uTenDB -pTenDB --target-dir=/data/dbbak/file_name --backup --binlog-info=ON --no-timestamp --compress --compress-thread=4 --ibbackup=/home/mysql/dbbackup/xtrabackup --flush-wait-timeout=30 --lazy-backup-non-innodb --wait-last-flush=2
```



## 如何提取原库表结构（非spider引擎）
那么如何原库表结构呢？
如果直接在TSpider侧提取，只能提取出spider引擎表，且带有分片信息。如果直接在后端TenDB提取，库名又会不一致。我们推荐在后端第0个分片通过mysqldump获取表结构，然后通过脚本替换库名。

下面给出单个库的提取方法，例如现在需要提取 `test_spider1`库下的表结构,需要操作如下两步：

1) 在第0个分片提取test_spider1_0库表结构
```
/home/mysql/dbbackup/mysqldump  -uTenDB -pTenDB -h"host" -P20000 --default-character-set=utf8 --skip-opt  --create-options --single-transaction -q --no-autocommit -d --database test_spider1_0 > dump.sql
```

2) 处理文本，替换库名
```
sed -i 's/test_spider1_0/test_spider1/g' dump.sql
```
