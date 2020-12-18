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
INSERT INTO tbl VALUES (1, '{"a": 1, "b": 2, "c": {"d": 4}}');
INSERT INTO tbl VALUES (2, '{"a": 3, "b": 2, "c": {"d": 4}}');
SELECT * FROM tbl WHERE JSON_CONTAINS(j, '1', '$.a');
SELECT * FROM tbl ORDER BY id;
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES (1, '{"a": 1, "b": 2, "c": {"d": 4}}');
SELECT * FROM tbl WHERE JSON_CONTAINS_PATH(j, 'one', '$.a', '$.e') = 0;
SELECT * FROM tbl WHERE JSON_CONTAINS_PATH(j, 'one', '$.a', '$.e') = 1;
