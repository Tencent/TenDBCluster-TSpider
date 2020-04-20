# Binlog Compression
In order to reduce the storage overhead of binlog. TenDB implements a feature of binlog compression based on ZLIB algorithm. This feature can be turned on and off during operation, and can be applied to `statement`, `row` and `mixed` format.

## How to use Binlog Compression
### Compress Binlog

Turn on binlog compression:
```sql 
set global log_bin_compress=ON 
```

`log_bin_compress_min_len` specifies the compression threshold, only binlog field exceed this length will be compressed.
```sql
set global log_bin_compress_min_len=256
```  

>After testing, enabling binlog compression can achieve a compression ratio of 7 to 9. It is a good practice to reduce IO overhead at the cost of CPU.

### Decompress Binlog

There are two ways to decompress binlog for users to choose:
1. Decompress binlog in the IO thread:
```sql
set global relay_log_uncompress = ON
```  

2. Decompress binlog in the SQL thread:
```sql
set global relay_log_uncompress = OFF
```  

>If the slave has enough relay-log space, you can set global relay_log_uncompress = ON (default configuration) to let binlog decompression done in the IO thread.
If the slave has insufficient relay-log space, you can set global relay_log_uncompress = OFF and let binlog decompression done in the sql thread, but this may cause the slow sql thread down.
However, slaves have parallel synchronization capabilities, so decompression in SQL threads generally does not have a significant impact on effiency.


## Compatibility

Four new types of events are introduced.  
`QUERY_COMPRESSED_EVENT` represents the DML event after compression in statement format;
`WRITE_ROWS_COMPRESSED_EVENT`,` UPDATE_ROWS_COMPRESSED_EVENT` and `DELETE_ROWS_COMPRESSED_EVENT` respectively represent the insertion, update and delete events in row format after compression.

> It is recommended to use TenDB's `mysqlbinlog` tool for binlog operation.


