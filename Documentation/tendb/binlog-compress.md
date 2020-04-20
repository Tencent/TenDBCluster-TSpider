# TenDB binlog压缩
为了减小binlog的存储开销。在TenDB中,基于ZLIB算法实现了binlog压缩的功能。该功能可以在运行中开启和关闭，并且适用于statement，row，mixed格式。

## 使用方法
### 压缩

开启Binlog压缩：
```sql 
set global log_bin_compress=ON
```

Binlog压缩阈值：
```sql 
set global log_bin_compress_min_len=256
```  
在开启压缩功能的前提下，binlog字段长度大于256的event，符合压缩条件。

>经过测试，在通常情况下，启用binlog压缩功能，可以达到7-9的压缩比。是以CPU为代价缓解了IO压力的良好实践。

### 解压

有两种解压的方式可以选择：
1. 让binlog在IO线程中解压：
```sql 
set global relay_log_uncompress = ON
```  

2. 让binlog在sql线程中解压：
```sql
set global relay_log_uncompress = OFF
```  

>如果slave机器，relay-log空间够，可以set global relay_log_uncompress=ON(默认配置)，在binlog在IO线程中解压。  
如果slave机器，relay-log空间不够，可以set global relay_log_uncompress=OFF，让binlog在sql线程中解压，但是这样可能会导致sql线程变慢。  
但slave都有并行同步能力，所以在sql线程解压一般不会对运行速度有很大影响。


## 兼容性

新增了4种事件类型：
`QUERY_COMPRESSED_EVENT`表示statement格式下压缩后的DML事件；
`WRITE_ROWS_COMPRESSED_EVENT`，`UPDATE_ROWS_COMPRESSED_EVENT`和`DELETE_ROWS_COMPRESSED_EVENT`分别表示row格式下插入，更新和删除在压缩后的事件。

>推荐使用TenDB的`mysqlbinlog`工具进行binlog操作。


