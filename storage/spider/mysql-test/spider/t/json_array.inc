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
SELECT Json_Array(56, 3.1416, 'My name is "Foo"', NULL);
INSERT INTO tbl VALUES (1, JSON_ARRAY(56, 3.1416, 'My name is "Foo"', NULL));
SELECT * FROM tbl;
SELECT * FROM tbl WHERE j = JSON_ARRAY(56, 3.1416, 'My name is "Foo"', NULL);
UPDATE tbl SET j = JSON_ARRAY(56, 3.1416, 'My name is "Foo"', 1234) WHERE id = 1;
SELECT * FROM tbl;
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES (1, '["a", ["b", "c"], "d"]');
SET @j = '["a", ["b", "c"], "d"]';
INSERT INTO tbl VALUES (2, JSON_ARRAY_APPEND(@j, '$[1]', 1));
SELECT * FROM tbl ORDER BY id;
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES(1, JSON_ARRAY_INSERT(@j, '$[1]', 'x'));
SELECT * FROM tbl ORDER BY id;
SET GLOBAL print_bool_as_literal = OFF;
