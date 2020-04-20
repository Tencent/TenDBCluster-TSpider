# TenDB Cluster Backup
Since TSpider itself does not store data, the data is stored in the backend TenDB. So it is recommended to back up only the schema on TSpider and the data in each backend TenDB. This article will introduce the backup of TSpider and backend TenDB.

## TSpider Backup
There is no difference between TSpider and ordinary MySQL logical backup, we use TSpider's own mysqldump tool for backup. But<font color="#dd0000">it is not recommended to directly back up all data from TSpider. </font> If there is no Tdbctl node
,you also need to backup mysql.servers data.
  
For example, backup all table schmea in  `test_spider1 test_spider2` database , you can execute : 
```
/home/mysql/dbbackup/mysqldump -uTSpider -pTSpider -S /data1/mysqldata/mysql.sock   --max-allowed-packet=1G  --default-character-set=utf8mb4 --single-transaction --no-autocommit=FALSE  --skip-opt --create-options  --routines  --quick --no-data --databases test_spider1 test_spider2
```

<font color="#dd0000">Attention:</font>   
>Need to specify : --no-data, only dump table schema



## TenDB Backup
TenDB is the storage layer of TenDB Cluster and supports logical backup and physical backup.

### Logical backup
There is no difference in logical backup between TenDB and ordinary MySQL.

For example, backup all data in  `test_spider1_0 test_spider2_0`database , you can execute : 
```
 /home/mysql/dbbackup/mysqldump -uTenDB -pTenDB -h"host" -P20000 --ignore-show-create-table-error --skip-foreign-key-check --max-allowed-packet=1G  --default-character-set=utf8 --single-transaction --no-autocommit=FALSE  --skip-opt --create-options  --routines  --master-data=2  --quick  --flush-wait-timeout=30 --databases test_spider1_0 test_spider2_0 
```


### Physical backup
TenDB  use percona xtrabackup for backup ,For details, please refer to [percona-xtrabackup Manual](https://www.percona.com/doc/percona-xtrabackup/2.4/manual.html)

For example：
```
/home/mysql/dbbackup/xtrabackup --defaults-file=/etc/my.cnf.20000 --host="host" --port=20000 -uTenDB -pTenDB --target-dir=/data/dbbak/file_name --backup --binlog-info=ON --no-timestamp --compress --compress-thread=4 --ibbackup=/home/mysql/dbbackup/xtrabackup --flush-wait-timeout=30 --lazy-backup-non-innodb --wait-last-flush=2
```



## How to extract the original table schema（not TSpider engine）
If it is extracted directly on the TSpider side, only the spider engine table can be extracted, with fragmentation information. If you extract directly from the backend TenDB, the database name will be inconsistent. We recommend obtaining the table schema through mysqldump on the 0th shard in the backend, and then replacing the database name with a script.

The following shows the extraction method of a single database. For example, if you need to extract the table schema under the `test_spider1` database, you need to perform the following two steps:

1) Extract the test_spider1_0 database table schema at the 0th shard
```
/home/mysql/dbbackup/mysqldump  -uTenDB -pTenDB -h"host" -P20000 --default-character-set=utf8 --skip-opt  --create-options --single-transaction -q --no-autocommit -d --database test_spider1_0 > dump.sql
```

2) Process text, replace database name
```
sed -i 's/test_spider1_0/test_spider1/g' dump.sql
```