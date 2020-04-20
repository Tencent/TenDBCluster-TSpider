# Multi-unique Key Adjustment
Based on the TSpider [Shard Key selection rules](../re-book/ddl-syntax-en.md#shard_key), if the TABLE has multiple unique keys, you need to do some adjustment. This article provides some experiences with multiple unique keys adjustments.

## More than One Unique Keys
If the TABLE has two or more unique keys (the primary key is a kind of unique key too), only one unique key is required to be kept, other unique keys should be changed to ordinary indexes, and <font color="#dd0000">the application layer should ensures the uniqueness of them</font>  


Original TABLE:

```
MariaDB [tendb_test]> CREATE TABLE `guild_user` 
    -> (  
    ->   `GID` bigint(20) NOT NULL DEFAULT '0',  
    ->   `UserId` bigint(20) NOT NULL DEFAULT '0',  
    ->    `GuildJob` tinyint(1) unsigned NOT NULL DEFAULT '0',  
    ->   `Contribution` int(10) unsigned DEFAULT '0',  
    ->   `Contribution_Total` int(10) unsigned DEFAULT '0', 
    ->   PRIMARY KEY `userid_index` (`UserId`),  
    ->   UNIQUE KEY `GID_index` (`GID`)
    -> ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
ERROR 4151 (HY000): Failed to execute ddl, Error code: 12021, Detail Error Messages: CREATE TABLE ERROR: ERROR: too more unique key with the different pre key
```


After Adjustment: <font color="#dd0000">
The application layer is required to guarantee the uniqueness of GID</font>  

```
MariaDB [tendb_test]> CREATE TABLE `guild_user` 
    -> (  
    ->   `GID` bigint(20) NOT NULL DEFAULT '0',  
    ->   `UserId` bigint(20) NOT NULL DEFAULT '0',  
    ->    `GuildJob` tinyint(1) unsigned NOT NULL DEFAULT '0',  
    ->   `Contribution` int(10) unsigned DEFAULT '0',  
    ->   `Contribution_Total` int(10) unsigned DEFAULT '0', 
    ->   PRIMARY KEY `userid_index` (`UserId`),  
    ->  KEY `GID_index` (`GID`)
    -> ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
Query OK, 0 rows affected (0.07 sec)
```


If the auto_increment column is the primary key and the unique key is another column, you can modify the auto_increment column to an ordinary index. As an ordinary key, the auto-inc column will be guaranteed to be unique by TenDB Cluster.

Original TABLE:

```
MariaDB [tendb_test]> CREATE TABLE `guild_list` 
    -> (  
    ->      `idx` bigint(20) unsigned NOT NULL AUTO_INCREMENT,  
    ->      `GID` bigint(20) NOT NULL DEFAULT '0',  
    ->      `GName` varchar(64) NOT NULL DEFAULT '',  
    ->      `CreateTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',  
    ->       `Data` blob,  
    ->       PRIMARY  KEY (`idx`),  
    ->       UNIQUE KEY `GName_index` (`GName`)
    -> ) 
    -> ;
ERROR 4151 (HY000): Failed to execute ddl, Error code: 12021, Detail Error Messages: CREATE TABLE ERROR: ERROR: too more unique key with the different pre key
```

After Adjustment, the auto-inc column `idx` will be guaranteed to be unique by TenDB Cluster.

```
MariaDB [tendb_test]> CREATE TABLE `guild_list` 
    -> (  
    ->      `idx` bigint(20) unsigned NOT NULL AUTO_INCREMENT,  
    ->      `GID` bigint(20) NOT NULL DEFAULT '0',  
    ->      `GName` varchar(64) NOT NULL DEFAULT '',  
    ->      `CreateTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',  
    ->       `Data` blob,  
    ->       KEY (`idx`),  
    ->       PRIMARY KEY `GName_index` (`GName`)
    -> ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
Query OK, 0 rows affected (0.04 sec)

```



## <font color="#dd0000">Note</font>   
TenDB Cluster do not allow more than one unique keys in a TABLE, unless each unique key has the same common part, and the common part is designated as the Shard Key



