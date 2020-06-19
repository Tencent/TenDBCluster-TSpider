# Stored Procedure
Stored Procedure is a database object that stores complex procedures in the database, and can be called by external programs. TenDB Cluster is compatible with MySQL's stored procedure grammar.



<font color="#dd0000">Note：</font> 
> It is not recommended to use complex stored procedures, triggers, functions in TenDB Cluster.  
When using stored procedures or triggers in the scenarios that require high data consistency, it is recommended to configure distributed transactions; otherwise, TenDB Cluster can only support local transactions.

## Create Stored Procedure

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

## Call Stored Procedure
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

## Drop Stored Procedure
```
MariaDB [tendb_test]> drop PROCEDURE delete_matches;
Query OK, 0 rows affected (0.02 sec)
```