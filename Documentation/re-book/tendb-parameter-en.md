# TenDB Variables Reference

TenDB Cluster's data layer TenDB is a branch of MySQL, which fully compatible, enhanced and open source.  
The main parameters and configuration can refer to the MySQL official manual, this chapter mainly introduce the new variables in TenDB.



**`LOG_BIN_COMPRESS`**

Scope: GLOBAL  
Default Value: OFF  
If on, binary log event will be compressed before logging. 


**`LOG_BIN_COMPRESS_MIN_LEN`**

Scope: GLOBAL  
Default Value: 256  
The minimum length in bytes for binary log event satisfy compress. 




**`RELAY_LOG_UNCOMPRESS`**

Scope: GLOBAL  
Default Value: ON  
If on, when Master enable `log_bin_compress` which compressed event will log to binary log, Slave's IO_THREAD will uncompress binary log's event before write to relay-log file.  
If off, Slave's IO_THREAD works as usual, compressed event will be extract by SQL_THREAD.



**`DATETIME_PRECISION_USE_V1`**  

Scope: GLOBAL  
Default Value: OFF  
If on, when CREATE TABLE, DATETIME/TIMESTAMP/TIME field whill user old format to compatibility MySQL 5.5 version. 



**`BLOB_COMPRESSED`**  

Scope: GLOBAL  
Default Value: OFF  
If on, enable blob column's data compress




**`READ_BINLOG_SPEED_LIMIT`**  

Scope: GLOBAL  
Default Value: 0  
Slave's IO_THREAD read binary log rate in kilobyte from Master.  
If 0, disable limit.
In some scenarios, if the read rate is high, it will cause abnormal traffic.


**`MAX_XA_COMMIT_LOGS`**

Scope: GLOBAL  
Default Value: 1000000  
The maximum record numbers for mysql.xa_commit_log. If exceed, will overwrite.



**`sort_when_partition_prefix_order`**

Scope: GLOBAL  
Default Value: TRUE  
If statement request a parition table, which use prefix index, limit and order by, result maybe incorrect on MySQL.  
If true, not affected by this bug. 

