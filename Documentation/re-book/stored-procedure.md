# 存储过程
存储过程（Stored Procedure）是一种在数据库中存储复杂程序，以便外部程序调用的一种数据库对象。TenDB Cluster兼容MySQL存储过程语法。


<font color="#dd0000">注意：</font> 
> 不建议在TenDB Cluster中使用复杂的存储过程，触发器，函数。   
在对数据一致性高要求的场景中使用存储过程或者触发器，建议配置分布式事务；否则TenDB Cluster只能支持单机事务。 



## 创建存储过程

```
MariaDB [tendb_test]> DELIMITER // 
MariaDB [tendb_test]> CREATE PROCEDURE delete_matches(IN p_playerno INTEGER)
    -> BEGIN
    -> DELETE FROM PLAYERS
    -> WHERE playerno = p_playerno;
    -> END 
    -> // 
Query OK, 0 rows affected (0.02 sec)
```

## 调用存储过程
```
MariaDB [tendb_test]> create table PLAYERS(id int primary key,playerno int);
MariaDB [tendb_test]> show  create table PLAYERS;

| PLAYERS | CREATE TABLE `PLAYERS` (
  `id` int(11) NOT NULL,
  `playerno` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`id`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "PLAYERS", server "SPT0"' ENGINE = SPIDER,
PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "PLAYERS", server "SPT1"' ENGINE = SPIDER,
PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "PLAYERS", server "SPT2"' ENGINE = SPIDER,
PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "PLAYERS", server "SPT3"' ENGINE = SPIDER)


MariaDB [tendb_test]> insert into  PLAYERS(id,playerno) values(1,1);
Query OK, 1 row affected (0.02 sec)

MariaDB [tendb_test]> select * from PLAYERS;
+----+----------+
| id | playerno |
+----+----------+
|  1 |        1 |
+----+----------+
1 row in set (0.01 sec)
```

```
MariaDB [tendb_test]> call delete_matches(1);
Query OK, 1 row affected (0.00 sec)

MariaDB [tendb_test]> select * from PLAYERS where playerno=1;
Empty set (0.00 sec)
```

## 删除存储过程
```
MariaDB [tendb_test]> drop PROCEDURE delete_matches;
Query OK, 0 rows affected (0.02 sec)
```