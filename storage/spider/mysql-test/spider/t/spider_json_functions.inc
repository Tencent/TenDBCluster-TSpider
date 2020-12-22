--let $OUTPUT_CHILD_GROUP2_BACKUP= $OUTPUT_CHILD_GROUP2
--let $OUTPUT_CHILD_GROUP2= 1

--echo
--echo drop and create databases
--connection master_1
DROP DATABASE IF EXISTS auto_test_local;

CREATE DATABASE auto_test_local;
USE auto_test_local;
if ($USE_CHILD_GROUP2)
{
  --connection child2_1
  DROP DATABASE IF EXISTS auto_test_remote;
  CREATE DATABASE auto_test_remote;
  USE auto_test_remote;

  --connection child2_2
  DROP DATABASE IF EXISTS auto_test_remote_2;
  CREATE DATABASE auto_test_remote_2;
  USE auto_test_remote_2;
}
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
