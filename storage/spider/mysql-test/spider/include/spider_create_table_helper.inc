# Auto-generate create table and drop table sql for childrend and the master
# Parameters needed
# 1. $CHILD2_1_DROP_TABLES   --> e.g. DROP TABLE IF EXISTS tbl;
# 2. $CHILD2_1_CREATE_TABLES --> e.g. CREATE TABLE tbl (id INT PRIMARY KEY, j JSON)$CHILD2_1_ENGINE  $CHILD2_1_CHARSET;
# 3. $CHILD2_2_DROP_TABLES   --> e.g. DROP TABLE IF EXISTS tbl;
# 4. $CHILD2_2_CREATE_TABLES --> e.g. CREATE TABLE tbl (id INT PRIMARY KEY, j JSON)$CHILD2_2_ENGINE  $CHILD2_2_CHARSET;
# 5. $MASTER_DROP_TABLES1    --> e.g. DROP TABLE IF EXISTS tbl;
# 6. $MASTER_CREATE_TABLES1  --> e.g. CREATE TABLE tbl (id INT PRIMARY KEY, j JSON)$MASTER_1_ENGINE $MASTER_1_CHARSET
#                                     PARTITION BY LIST (id % 2)
#                                     (
#                                       PARTITION `pt0` VALUES IN (0) COMMENT = 'database "auto_test_remote", table "tbl", srv "s_2_1",aim "0"',
#                                       PARTITION `pt1` VALUES IN (1) COMMENT = 'database "auto_test_remote_2", table "tbl", srv "s_2_2",aim "1"'
#                                     );
# We can conclude that the only information we need is
# 1. Table name => tbl 
# 2. Table structure body => (id INT PRIMARY KEY, j JSON)
# 3. Shard key => id
#
# So, the foreign .test file should pass the follwoing three parameters
# 1. --let $TABLE_NAME = tbl
# 2. --let $TABLE_BODY = id INT PRIMARY KEY, j JSON
# 3. --let $TABLE_SHARD_KEY = id


--let $CHILD2_1_DROP_TABLES = DROP TABLE IF EXISTS $TABLE_NAME;
--let $CHILD2_2_DROP_TABLES = $CHILD2_1_DROP_TABLES
--let $MASTER_DROP_TABLES1 = $CHILD2_1_DROP_TABLES
--let $CHILD2_1_CREATE_TABLES = CREATE TABLE $TABLE_NAME ($TABLE_BODY)$CHILD2_1_ENGINE  $CHILD2_1_CHARSET;
--let $CHILD2_2_CREATE_TABLES = CREATE TABLE $TABLE_NAME ($TABLE_BODY)$CHILD2_2_ENGINE  $CHILD2_2_CHARSET;
--let $MASTER_CREATE_TABLES1 = CREATE TABLE $TABLE_NAME ($TABLE_BODY)$MASTER_1_ENGINE $MASTER_1_CHARSET PARTITION BY LIST (CRC32($TABLE_SHARD_KEY) % 2)( PARTITION `pt0` VALUES IN (0) COMMENT = 'database "auto_test_remote", table "$TABLE_NAME", srv "s_2_1",aim "0"', PARTITION `pt1` VALUES IN (1) COMMENT = 'database "auto_test_remote_2", table "$TABLE_NAME", srv "s_2_2",aim "1"');

--let $OUTPUT_CHILD_GROUP2_BACKUP= $OUTPUT_CHILD_GROUP2
--let $OUTPUT_CHILD_GROUP2= 1

--enable_warnings

--echo
--echo create table for child
if ($USE_CHILD_GROUP2)
{
  if (!$OUTPUT_CHILD_GROUP2)
  {
    --disable_query_log
    --disable_result_log
  }
  --connection child2_1
  if ($OUTPUT_CHILD_GROUP2)
  {
    --disable_query_log
    echo CHILD2_1_DROP_TABLES;
    echo CHILD2_1_CREATE_TABLES;
  }
  --disable_warnings
  eval $CHILD2_1_DROP_TABLES;
  --enable_warnings
  eval $CHILD2_1_CREATE_TABLES;
  if ($OUTPUT_CHILD_GROUP2)
  {
    --enable_query_log
  }
  if (!$OUTPUT_CHILD_GROUP2)
  {
    --enable_query_log
    --enable_result_log
  }
}

if ($USE_CHILD_GROUP2)
{
  if (!$OUTPUT_CHILD_GROUP2)
  {
    --disable_query_log
    --disable_result_log
  }
  --connection child2_2
  if ($OUTPUT_CHILD_GROUP2)
  {
    --disable_query_log
    echo CHILD2_2_DROP_TABLES;
    echo CHILD2_2_CREATE_TABLES;
  }
  --disable_warnings
  eval $CHILD2_2_DROP_TABLES;
  --enable_warnings
  eval $CHILD2_2_CREATE_TABLES;
  if ($OUTPUT_CHILD_GROUP2)
  {
    --enable_query_log
  }
  if (!$OUTPUT_CHILD_GROUP2)
  {
    --enable_query_log
    --enable_result_log
  }
}

--echo
--echo create table for master
--connection master_1
--disable_warnings

eval $MASTER_DROP_TABLES1;

--enable_warnings
--disable_query_log
--enable_query_log

eval $MASTER_CREATE_TABLES1;
