# Trigger
A trigger is a named database object that is associated with a table, and that activates when a particular event occurs for the table. TenDB Cluster is compatible with MySQL's trigger grammar.


<font color="#dd0000">Note：</font> 
> It is not recommended to use complex stored procedures, triggers, functions in TenDB Cluster.  
When using stored procedures or triggers in the scenarios that require high data consistency, it is recommended to configure distributed transactions; otherwise, TenDB Cluster can only support local transactions.

## Create Trigger
```
delimiter $$
    CREATE TRIGGER upd_check BEFORE UPDATE ON account
    FOR EACH ROW
    BEGIN
    IF NEW.amount < 0 THEN
    SET NEW.amount = 0;
    ELSEIF NEW.amount > 100 THEN
    SET NEW.amount = 100;
    END IF;
    END$$
delimiter ;
```

## Call Trigger
```
MariaDB [tendb_test]> CREATE TABLE account (acct_num INT primary key, amount DECIMAL(10,2));
MariaDB [tendb_test]> show create table account;

| account | CREATE TABLE `account` (
  `acct_num` int(11) NOT NULL,
  `amount` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`acct_num`)
) ENGINE=SPIDER DEFAULT CHARSET=utf8
 PARTITION BY LIST (crc32(`acct_num`) MOD 4)
(PARTITION `pt0` VALUES IN (0) COMMENT = 'database "tendb_test_0", table "account", server "SPT0"' ENGINE = SPIDER,
PARTITION `pt1` VALUES IN (1) COMMENT = 'database "tendb_test_1", table "account", server "SPT1"' ENGINE = SPIDER,
PARTITION `pt2` VALUES IN (2) COMMENT = 'database "tendb_test_2", table "account", server "SPT2"' ENGINE = SPIDER,
PARTITION `pt3` VALUES IN (3) COMMENT = 'database "tendb_test_3", table "account", server "SPT3"' ENGINE = SPIDER)


MariaDB [tendb_test]> INSERT INTO account VALUES(137,14.98),(141,1937.50),(97,-100.00);
MariaDB [tendb_test]> select * from account;
+----------+---------+
| acct_num | amount  |
+----------+---------+
|       97 | -100.00 |
|      137 |   14.98 |
|      141 | 1937.50 |
+----------+---------+
3 rows in set (0.00 sec)
```

```
MariaDB [tendb_test]> update account set amount=-10 where acct_num=137;
MariaDB [tendb_test]> select * from account;

+----------+---------+
| acct_num | amount  |
+----------+---------+
|       97 | -100.00 |
|      137 |    0.00 |
|      141 | 1937.50 |
+----------+---------+
```

```
MariaDB [tendb_test]> update account set amount=-10 where acct_num=137;
MariaDB [tendb_test]> select * from account;

+----------+---------+
| acct_num | amount  |
+----------+---------+
|       97 | -100.00 |
|      137 |    0.00 |
|      141 | 1937.50 |
+----------+---------+
```

## Drop Trigger
```
MariaDB [tendb_test]> DROP TRIGGER upd_check;
Query OK, 0 rows affected (0.04 sec)
```
