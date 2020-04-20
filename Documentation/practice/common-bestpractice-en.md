# General Scenarios
Due to the ability to be compatible with MySQL protocol, transparent database/table sharding, online scaling, TenDB cluster is suitable for most scenarios where MySQL is applicable whereas a shared layout is more adequate to handle large amount of data or requests.

This document summarizes best practices on general scenarios when using TenDB cluster, including table creation, SQL query, and instance management.

## **Create Database or Table**
Below tips are advised when creating database or table on TenDB cluster:
1. Use the same character set for columns to the table when creating a table on TSpider.
2. When creating a table on TSpider, by default the first column of primary key/unique key will be used as shard_key; if no primary key nor unique key is present, a shard key should be designated explicitly.
3. When choosing a shard_key, data should be distributed evenly upon mod(harsh(shard_key)), otherwise, a skewed data layer could lead to hot spots or bottleneck.
4. Foreign key constraint is not applicable for TSpider tables.
5. When using auto-increment columns, use bigint as the data type for an auto_increment column, and it's suggested to let TSpider itself to manage the columns. It's important to be aware that TSpider only guarantees uniqueness of auto-increment columns, and no guarantee that they are ordered and increment globally.
6. If primary key and unique key are present, if the primary key is of auto_increment type, and the unique key is the character column of application business logic, it's suggested to modify auto_increment column to ordinary index.
7. Enabling compression is suggested for Blob columns.
8. If data are to be expired and deleted periodically, partitioned tables are suggested to be used in TenDB storage instance.
9. Store hot and cold data separately when designing a table.
10. Store log data and data subject to change in separate storage, Such as different databases or clusters.

## **SQL Requests**
Below tips are advised when make requests to TenDB cluster:
1. Use shard key on the right of == in your query, if not possible, it's suggested to create an index, and to throttle frequency of requests, massive scans that across partitions should be avoided if possible.
2. Avoid batch deletion.
3. Avoid retrieving large amounts of data from TSpider node, which may lead to memory usage surge in TSpider node or even an OOM failure.
4. Avoid aggregation queries(such as group by, etc.) of high frequency.
5. Use straight_join to designate driving table when using join.
6. Avoid to retrieve data in order by x limit m, n style.
7. If data are to be retrieved frequently, such as configuration data, it's suggested to use a cache in the front layer to reduce the workload on database.
8. If business logic makes random limit n requests, SPIDER_RONE_SHARD switch could be turned on, when enabled, requests like select SPIDER_RONE_SHARD * from t limit n will be routered randomly to one of the backend shards.

 
## **TSpider Instance Management**
Below tips are advised when manage TSpider instances:
1. Setting common parameters, see the next section: [Setting parameters for TSpider](../re-book/tspider-parameter.md)
2. When retrieving a large amount of data, or even the entire database, It's suggested to use temporary TSpider nodes to serve the request.

## **Setting Parameters for TSpider**

### spider_quick_mode
> This parameter specifies where to cache data retrieved from backends, RemoteDB or local buffer.
> The default value is 1 upon installation. If a database merge task is expected, it's suggested to be set to 0.
> In rare situations, It's set to 3, to cope with OOM killed issue when retrieving a large amount of data. The side effect is temporary storage space of spider may be exhausted when retrieving mass data.

### spider_max_connections
> This parameter is used to specify the max number of connections from TSpider to RemoteDB. According to our experience in production environments, the value of 200 is adequate for most applications. If the value is set to high, too many connections may be made to RemoteDB. 
And spider_max_connections * <TSpider count> should be less than the max_connections set on TenDB storage node.

### spider_bgs_mode

> This parameter specifies whether to enable parallel query in backends. It's suggested to be set to 1, ie., to enable parallel query in backends.

### spider_index_hint_pushdown
> This parameter specifies whether to enable index hint to be pushed to backends, such as force_index. It's suggested to be set to 1.

### spider_get_sts_or_crd
> This parameter specifies whether to allow statements such as show table status, and show index, etc. to be executed on RemoteDB to collect statistics. It's suggested to be set to 1.

### spider_ignore_autocommit
> This parameter specifies whether to allow the set autocommit=0 statement can be executed on RemoteDB.

### spider_rone_shard_switch
> TSpider introduced a new SPIDER_RONE_SHARD option, when enabled, statements such as select SPIDER_RONE_SHARD * from t limit 10 will be distributed to a random backend shard. In scenarios where only a small sample piece of the data is desired, It could be enabled.

For more suggestions on TSpider parameter setting, see [TSpider parameters](./../re-book/tspider-parameter-en.md)

## **Storage Instance Management**
Below tips are advised when manage TenDB storage instance:
1. Use SSD/NVME disk to avoid I/O bottleneck.
2. Use Binlog of ROW mode.
3. Use a physical backup manner to back your data. It's suggested to switch upload Binlog to remote backup system, in every 5 minutes.
4. When dropping a partition, use the hard link trick to avoid influence on I/O performance.

## **Sharding Number**
All data in old shards must be re-organized to change the number of shards, which could make a heavy workload to the system. So it's crucial to choose a proper sharding number. Usually it's suggested to shard your data with 1.5 times to 2 times of your expected data amount. To make it easier to scale up or scale down the cluster, It's suggested to use a sharding number with more submultiples, such as 12, 16 or 24.

