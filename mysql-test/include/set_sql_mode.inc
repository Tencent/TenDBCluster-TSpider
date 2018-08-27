#
# suite.pm will make sure that all tests including this file
# will be skipped unless innodb is enabled
#
-- disable_query_log
set sql_mode='STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION,NO_AUTO_CREATE_USER';
-- enable_query_log
