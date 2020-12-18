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
INSERT INTO tbl VALUES (1, '{"a": 1, "b": {"c": 30}}');
SELECT * FROM tbl WHERE JSON_KEYS(j) = JSON_KEYS('{"a": 1, "b": {"c": 30}}');
SELECT * FROM tbl WHERE JSON_KEYS(j) = JSON_ARRAY("a", "b");
TRUNCATE TABLE tbl;
INSERT INTO tbl VALUES(1, '["abc", [{"k": "10"}, "def"], {"x":"abc"}, {"y":"bcd"}]');
SELECT JSON_SEARCH(j, 'one', 'abc') FROM tbl;
SELECT JSON_SEARCH(j, 'all', 'abc') FROM tbl;
SELECT JSON_SEARCH(j, 'all', 'ghi') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '10') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '10', NULL, '$') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '10', NULL, '$[*]') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '10', NULL, '$**.k') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '10', NULL, '$[*][0].k') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '10', NULL, '$[1]') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '10', NULL, '$[1][0]') FROM tbl;
SELECT JSON_SEARCH(j, 'all', 'abc', NULL, '$[2]') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '%a%') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '%b%') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '%b%', NULL, '$[0]') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '%b%', NULL, '$[2]') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '%b%', NULL, '$[1]') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '%b%', '', '$[1]') FROM tbl;
SELECT JSON_SEARCH(j, 'all', '%b%', '', '$[3]') FROM tbl;
