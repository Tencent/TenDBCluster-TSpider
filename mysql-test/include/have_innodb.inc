#
# suite.pm will make sure that all tests including this file
# will be skipped unless innodb is enabled
#
-- disable_query_log
set optimizer_switch='index_merge=on,index_merge_union=on,index_merge_sort_union=on,index_merge_intersection=on,index_merge_sort_intersection=on,index_condition_pushdown=on';
set sql_mode='STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION,NO_AUTO_CREATE_USER';
-- enable_query_log
