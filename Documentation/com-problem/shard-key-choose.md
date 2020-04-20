# Shard Key选取
TSpider会按照表分区键来进行数据分片,分区键的选择尤为重要。本文将提供分区键选取的规则，以及建议。

## Shard Key选取规则
选取规则如下：
*   如果只有一个唯一键（含主键),不指定shard_key, 默认会用唯一键的第一个字段作为分区key；
*   如果只有一个普通索引,不指定shard_key, 默认会用索引的第一个字段作为分区key；
*   如果指定shard_key，shard_key必须是索引的一部分；
*   如果多个唯一健（含主键),shard_key只能是其中的共同部分；否则无法建表；
*   如果只包含多个普通索引，则必须指定shard_key。
  

选取规则更详细内容可以参考[TenDB Cluster DDL](../re-book/ddl-syntax.md)


## 选取建议
*   分片的字段，最好是作为经常查询的条件，这样可以提高查询效率；  
*   使用key作为shard_key，数据分布应该均匀，避免数据集中在一个分片上;  


## Note
1. 可通过打开general log来分析应用的各个Table的请求模式，来进行Shard Key的调整，尽量减小跨分片扫描。
2. 另外也可以设置log_sql_use_mutil_partition为true将跨分区扫描的SQL打印到慢查询日志中进行分析。
3. 可以通过查看QUERY_RESPONSE_TIME table来展示TSpider上查询和跨分片扫描的信息。
    ```
     mysql> show QUERY_RESPONSE_TIME;
     mysql> SELECT * FROM information_schema.QUERY_RESPONSE_TIME;
    ```