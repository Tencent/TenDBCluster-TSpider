#TenDB参数说明  

集群的存储层TenDB是一个MySQL分支，在兼容MySQL的参数的基础上新增了一些参数。
其主要参数以及配置可以参照MySQL官方手册，本文将主要介绍新增的参数。



**`LOG_BIN_COMPRESS`**

作用域: GLOBAL  
默认值: OFF  
值为OFF，表示不开启Binlog压缩特性；否则表示打开。



**`LOG_BIN_COMPRESS_MIN_LEN`**

作用域: GLOBAL  
默认值: 256  
值为256表示，在开启压缩功能的前提下，大于256字节的binlog event，符合压缩条件。




**`RELAY_LOG_UNCOMPRESS`**

作用域: GLOBAL  
默认值: ON  
值为ON表示解压,Master启用binlog压缩后，slave的IO线程在拉取binlog时，可通过此参数控制是否立即解压binlog;值为OFF，会在SQL线程执行时由SQL线程解压。



**`DATETIME_PRECISION_USE_V1`**  

作用域: GLOBAL  
默认值: OFF  
值为ON，表示在5.6创表时,DATETIME/TIMESTAMP/TIME默认使用低版本格式(兼容低版本)。值为OFF，则表示不是默认使用低版本格式。




**`BLOB_COMPRESSED`**  

作用域: GLOBAL  
默认值: OFF  
值为ON，表示开启blob列压缩特性；否则表示不开启。




**`READ_BINLOG_SPEED_LIMIT`**  

作用域: GLOBAL  
默认值: 0  
值为0，表示slave拉取binlog不限速。
由于start slave后，slave会尽可能快地拉取master的binlog，可能导致master的流量较高，影响master的性能，因此需要控制拉取binlog的速度，单位是KB。  
set global read_binlog_speed_limit = 30720; //限速30M  
set global read_binlog_speed_limit = 0 ; //不限速



**`MAX_XA_COMMIT_LOGS`**

作用域: GLOBAL  
默认值: 1000000  
值为1000000表示mysql.xa_commit_log的最大行数，此表是循环使用的



**`sort_when_partition_prefix_order`**

作用域: GLOBAL  
默认值: TRUE  
值为TRUE，用于修复涉及到分区表+前缀索引+order by的bug
