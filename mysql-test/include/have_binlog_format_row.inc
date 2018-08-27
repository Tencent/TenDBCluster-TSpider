--source include/have_log_bin.inc
--source include/set_sql_mode.inc

if (`SELECT @@binlog_format != 'ROW'`)
{
  --skip Not ROW format
}
