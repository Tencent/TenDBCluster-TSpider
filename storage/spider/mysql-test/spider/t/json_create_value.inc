let $CHILD2_1_DROP_TABLES=
DROP TABLE IF EXISTS tbl;
let $CHILD2_1_CREATE_TABLES=
CREATE TABLE tbl (id INT PRIMARY KEY, j JSON)$CHILD2_1_ENGINE  $CHILD2_1_CHARSET;

let $CHILD2_2_DROP_TABLES=
DROP TABLE IF EXISTS tbl;
let $CHILD2_2_CREATE_TABLES=
CREATE TABLE tbl (id INT PRIMARY KEY, j JSON)$CHILD2_2_ENGINE  $CHILD2_2_CHARSET;

let $MASTER_DROP_TABLES1=
DROP TABLE IF EXISTS tbl;

let $MASTER_CREATE_TABLES1 = 
CREATE TABLE tbl (id INT PRIMARY KEY, j JSON)$MASTER_1_ENGINE $MASTER_1_CHARSET
PARTITION BY LIST (id % 2)
(
  PARTITION `pt0` VALUES IN (0) COMMENT = 'database "auto_test_remote", table "tbl", srv "s_2_1",aim "0"',
  PARTITION `ptbl` VALUES IN (1) COMMENT = 'database "auto_test_remote_2", table "tbl", srv "s_2_2",aim "1"'
);

--source spider_json_functions.inc

--echo
--echo insert data and test
SET GLOBAL print_bool_as_literal = ON;
INSERT INTO tbl VALUES(1, 'null'),(2, '"null"'),(3, '[1,2,3]');
SELECT JSON_QUOTE(j) FROM tbl;
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES (1, json_array(1,'abc',NULL,true));
SELECT * FROM tbl WHERE (j = json_array(1,'abc',NULL,true));
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES(1, JSON_OBJECT('name', 'carrot', 'id', 87, 'flag', true));
INSERT INTO tbl VALUES(2, JSON_OBJECT('name', 'nuts', 'id', 85, 'flag', false, 'ext', JSON_OBJECT('name1', 'carrot', 'id1', 86, 'flag', true)));
INSERT INTO tbl VALUES (3, JSON_OBJECT());
INSERT INTO tbl VALUES (4, NULL);
SELECT * FROM tbl ORDER BY id;
SELECT * FROM tbl WHERE j = JSON_OBJECT('name', 'carrot', 'id', 87, 'flag', true);
SELECT * FROM tbl where j = JSON_OBJECT('name', 'nuts', 'id', 85, 'flag', false, 'ext', JSON_OBJECT('name1', 'carrot', 'id1', 86, 'flag', true));
SELECT * FROM tbl where j != JSON_OBJECT('name', 'nuts', 'id', 85, 'flag', false, 'ext', JSON_OBJECT('name1', 'carrot', 'id1', 86, 'flag', true));
SELECT * FROM tbl WHERE j IS NULL;
SELECT * FROM tbl WHERE j IS NOT NULL ORDER BY id;
TRUNCATE TABLE tbl;
SET GLOBAL print_bool_as_literal = OFF;
