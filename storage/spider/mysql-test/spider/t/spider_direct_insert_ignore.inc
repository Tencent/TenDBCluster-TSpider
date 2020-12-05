--connection master_1
eval SET spider_direct_dup_insert = $VAL_DIRECT_DUP_INSERT;
eval SET spider_direct_insert_ignore = $VAL_DIRECT_INSERT_IGNORE;

TRUNCATE TABLE tbl_a;
INSERT INTO tbl_a VALUES (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6);
--eval $TEST_SQL
--echo
--echo select for master
--eval SELECT * FROM tbl_a
if ($USE_GENERAL_LOG)
{
    --eval SELECT argument FROM mysql.general_log WHERE argument LIKE '%mysql% $TEST_SQL_TYPE %'
}
if ($USE_CHILD_GROUP2)
{
    --connection child2_1
    if ($USE_GENERAL_LOG)
    {
        --eval SELECT argument FROM mysql.general_log WHERE argument LIKE '%$TEST_SQL_TYPE %'
    }
    --connection child2_2
    if ($USE_GENERAL_LOG)
    {
        --eval SELECT argument FROM mysql.general_log WHERE argument LIKE '%$TEST_SQL_TYPE %'
    }
}