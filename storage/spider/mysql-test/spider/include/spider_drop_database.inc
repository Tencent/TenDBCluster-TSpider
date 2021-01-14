--echo
--echo deinit
--disable_warnings
--connection master_1
DROP DATABASE IF EXISTS auto_test_local;

if ($USE_CHILD_GROUP2)
{
  --connection child2_1
  DROP DATABASE IF EXISTS auto_test_remote;
  --connection child2_2
  DROP DATABASE IF EXISTS auto_test_remote_2;
}

--disable_query_log
--disable_result_log
--source ../t/test_deinit.inc
--enable_result_log
--enable_query_log
--enable_warnings
--echo
--echo end of test
