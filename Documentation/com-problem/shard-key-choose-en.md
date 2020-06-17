## Choose The Right Shard Key For TABLE
TSpider Node use the Shard Key to distributed the data to every TenDB Node, It's very important to choose the right Shard Key, you can follow some rules and suggestions here.

## The Rules of Choosing Shard Key
+ If the TABLE just has one unique key, and you don't specify the Shard Key, TSpider Node will use the first column of in unique key as Shard Key.
+ If the TABLE has several unique keys, the specified Shard Key should be the common column of these unique keys, otherwise you can not create table.
+ If the TABLE has multiple unique keys, without specify Shard Key, the first column of the unique key will be used as the Shard Key by default. But it is necessary to ensure that the Shard Key is the first column of each unique key, otherwise the table can not be created.
+ If the TABLE just has one key/index, and you don't specify the Shard Key, TSpider Node will use the first column of the key/index as Shard Key.
+ If the TABLE has several keys/indexes, you must specify one Shard Key.
+ If you specify the Shard Key, it should be part of key/Index of the TABLE.

Among them, the constraints of the relationship between the ordinary index and the shard_key are the limitations when creating tables through the  node Tdbctl. Without relying on the central control node, it will not be constrained when building tables directly on TSpider.  However, it is generally recommended that developers access storage instances based on indexes.

More Rules of choosing Shard Key can founded at [TenDB Cluster DDL](../re-book/ddl-syntax-en.md) 

## The Suggestions of Choosing Shard Key
+  The Shard Key column is best used as conditions for frequent queries, which can improve query efficiency.
+  Use the Column as the Shard Key, the data distribution should be average, to avoid the data concentrated on one or partial Shards.

## Note
1. You can open the general log to analyze the request mode of each TABLE to adjust the Shard Key to minimize Cross-Shard scanning.
2. In addition, you can also set log_sql_use_mutil_partition to true to print the SQL which scans across multiple Shards to the slow query log for analysis.
3. You can find Cross-Shard scanning information on TSpider Node by viewing the QUERY_RESPONSE_TIME table.

	```
	mysql>show QUERY_RESPONSE_TIME; 
	mysql>SELECT * FROM information_schema.QUERY_RESPONSE_TIME;
	```