# 多唯一键调整
基于TSpider分区键[选取规则](../re-book/ddl-syntax.md#shard_key)，如果建表是出现多唯一键，则需要进行调整。本文提供一些多唯一键调整经验。

## 同时存在主键与唯一键
若表结构中包含有两个及以上的唯一键（主键是唯一键的一种），且无共同组成部分。要求只保留一个唯一键，其它唯一键变更为普通索引，<font color="#dd0000">应用层来保证其它字段的唯一性</font>  


原表：
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


修改后：<font color="#dd0000">需要应用层来保证 GID的唯一性</font>  
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





若auto_increment列为主键，而唯一键为其他字段，可以将auto_increment列修改为普通index。自增列作为普通Key, 也能保证唯一。

原表:
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
修改后，将自增列作为普通Key, 也能保证唯一。
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




<font color="#dd0000">注意</font>   
>业务中不允许表的唯一键超过一个，除非各个唯一键有共同部分，且指定该共同部分为分区键





