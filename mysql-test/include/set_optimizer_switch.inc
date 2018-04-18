#
# suite.pm will make sure that all tests including this file
# will be skipped unless innodb is enabled
#
-- disable_query_log
set optimizer_switch='index_merge=on,index_merge_union=on,index_merge_sort_union=on,index_merge_intersection=on,index_merge_sort_intersection=on,index_condition_pushdown=on';
-- enable_query_log
