# 触发器
触发器是与表有关的数据库对象，在满足定义条件时触发，并执行触发器中定义的语句集合。TenDB Cluster兼容MySQL触发器语法。



<font color="#dd0000">注意：</font> 
>不建议在TenDB Cluster中使用复杂的存储过程，触发器，函数。   
在对数据一致性高要求的场景中使用存储过程或者触发器，建议配置分布式事务；否则TenDB Cluster只能支持单机事务。



## 创建触发器
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

## 调用触发器
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

## 删除触发器
```
MariaDB [tendb_test]> DROP TRIGGER upd_check;
Query OK, 0 rows affected (0.04 sec)
```
