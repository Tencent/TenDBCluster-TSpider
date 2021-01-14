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