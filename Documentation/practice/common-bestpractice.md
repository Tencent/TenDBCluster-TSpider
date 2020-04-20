# 普通应用

基于TenDB Cluster兼容MySQL协议，透明分库分表、动态扩缩容的特点， TenDB Cluster适合使用MySQL但是对数据规模大或者请求量大希望做Sharding的绝大多数业务场景。

本文档用于总结在使用TenDB Cluster的一些最佳实践。从建表、SQL请求、实例管理等几方面来阐述。

## **建库建表**
对TenDB Cluster集群建库建表，有如下建议：
1. TSpider建表时，列的字符集必须与表的一致。
2. TSpider建表时，默认使用主键/唯一键的第一个字段作为shard_key；如果没有主键/唯一键，需要显式指定shard_key。
3. TSpider建表指定shard_key时，数据按照shard_key hash取模后需要是分布均匀的。如果数据分布不均匀会造成部分分片负载过高成为瓶颈。
4. TSpider建表不能使用外键约束。
5. 使用自增列时，auto_increment的列类型使用bigint，同时推荐由TSpider来进行自增列维护。<font color="#dd0000">要注意TSpider只保证自增字段的唯一性，不保证有序和递增</font> 。
6. 如果存在主键与唯一键 若主键为auto_increment列，而唯一键是业务/应用特征字段，推荐将auto_increment列修改为普通index。
7. Blob大字段类型，推荐使用压缩功能。
8. 如果有数据过期或者清理需求，TenDB存储实例推荐使用分区表。
9. 表设计时，建议将冷热数据分开独立存储。
10. 状态类与日志类数据推荐分开存放，比如独立db或者独立集群部署。


## **SQL请求**
对于访问TenDB Cluster的请求，有如下建议：
1. 查询、更新条件尽量带shard_key作为等值条件；如果不能则需要注意添加索引，且控制请求频率，防止大量的跨分区扫描。
2. 避免进行大批量的数据delete操作。
3. 避免在TSpider节点拉取大量数据，以免造成TSpider内存节点内存突增，导致节点OOM掉。
4. 避免执行高频次的聚集操作(group by等)。
5. 若使用join，推荐通过straight_join来指定驱动表。
6. 避免使用order by x limit m,n的方式获取数据。
7. 如果有高频访问的配置信息等，建议在前端增加缓存以减轻DB负担。
8. 如果业务有存在随机limit n这种请求，可以开启SPIDER_RONE_SHARD特性，select SPIDER_RONE_SHARD * from t limit n这类语句会随机分发到后端一个分片执行。

## **TSpider实例管理**
对于TSpider实例的管理，有如下建议：
1. 常见参数设置，参考下节:[TSpider常用参数设置说明](../re-book/tspider-parameter.md)
2. 如果应用有拉取大量数据以及类似合服类的需求，建议增加临时TSpider节点来支持。


## **TSpider常用参数设置说明**
### spider_quick_mode


> 此参数用来控制从后端获取数据的时候缓存到RemoteDB还是spider的本地buffer中。
> 目前上架时此配置默认值是1。如果有合服之类的需求，建议将此参数设置为0。
> 个别业务上设置为3，用来应对拉取大量数据而导致spider进程被oom kill的情况。有个副作用，在拉取大量数据时会造成spider临时空间耗用问题。


### spider_max_connections

>此参数用来控制从Spider到RemoteDB的最大连接数。从实际运营经验来看，200这个值能满足绝大多数业务的需要。此值设置过大，会导致RemoteDB上连接数过多。
另外，spider_max_connections * <TSpider数量> 需要小于TenDB存储节点上max_connections的设置值。

### spider_bgs_mode

>此参数用于控制是否打开后端并行查询的功能。建议设置为1，打开后端并行查询的功能。

### spider_index_hint_pushdown

>此参数用以控制是否允许index hints的下放，比如force_index。建议设置为1。

### spider_get_sts_or_crd

>此参数用以控制show table status、show index语句能否在RemoteDB上执行来搜集统计信息。建议设置为1。

### spider_ignore_autocommit

>此参数用来控制set autocommit=0的语句能否分发到RemoteDB上执行。

### spider_rone_shard_switch

>TSpider增加了SPIDER_RONE_SHARD选项，实现select SPIDER_RONE_SHARD * from t limit 10这类语句会随机分发到后端一个分片执行。如果应用只是需要随机返回几行数据，可以打开此特性。

更多的TSpider初始化参数设置，可以参考 [TSpider参数说明](./../re-book/tspider-parameter.md) 。


## **存储实例管理**
对于TenDB存储实例管理，有如下建议：
1. 推荐使用SSD/NVME磁盘来减少I/O瓶颈。
2. 推荐使用ROW模式的Binlog。
3. 推荐使用物理备份的方式进行数据备份。Binlog建议每5分钟进行一次切换并上报备份系统。
4. 对数据清理（drop partition）操作可使用硬链的方式防抖动。

## **集群分片数**

TenDB Cluster集群如果想调整分片数，目前需要对全部数据进行重导，运营成本很高。因此提前规划好集群分片数尤为重要，通常可以按需求的1.5倍到2倍来评估分片数。 为了便于存储实例扩缩，要选择公约数较多的数作为初始分片数，比如12,16,24分片。