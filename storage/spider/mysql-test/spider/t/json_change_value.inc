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
INSERT INTO tbl VALUES(1, '{"a": 1, "b": [2, 3], "c": "[true, false]"}');
SELECT * FROM tbl WHERE j = JSON_INSERT('{ "a": 1, "b": [2, 3]}', '$.c', '[true, false]');
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES(1, '[1, 2, true, false]');
SELECT * FROM tbl WHERE j = JSON_MERGE('[1, 2]', '[true, false]');
UPDATE tbl SET j = JSON_MERGE('[2, 1]', '[true, true]');
SELECT * FROM tbl WHERE j = JSON_MERGE('[1, 2]', '[true, false]');
SELECT * FROM tbl;
UPDATE tbl SET j = JSON_MERGE('[8, 8]', '[false, false]') WHERE j = JSON_MERGE('[2, 1]', '[true, true]');
SELECT * FROM tbl;
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES(1, '["a", "d"]');
SELECT * FROM tbl WHERE j = JSON_REMOVE('["a", ["b", "c"], "d"]', '$[1]');
UPDATE tbl SET j = JSON_REMOVE(j, '$[0]');
SELECT * FROM tbl;
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES (1, '{ "a": 1, "b": [2, 3]}');
UPDATE tbl SET j = JSON_REPLACE(j, '$.a', 10, '$.c', '[true, false]');
SELECT * FROM tbl;
UPDATE tbl SET j = JSON_REPLACE(j, '$.b[1]', 4);
SELECT * FROM tbl;
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES(1, '{ "a": 1, "b": [2, 3]}');
SELECT * FROM tbl;
UPDATE tbl SET j = JSON_SET(j, '$.a', 10, '$.c', '[true, false]');
SELECT * FROM tbl;
SELECT * FROM tbl WHERE j = JSON_SET('{ "a": 1, "b": [2, 3]}', '$.a', 10, '$.c', '[true, false]');
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES(1, '"abc"');
SELECT JSON_UNQUOTE(j) FROM tbl;
SELECT * FROM tbl WHERE JSON_UNQUOTE(j) = 'abc';
SELECT * FROM tbl WHERE JSON_UNQUOTE(j) = 'abcd';
