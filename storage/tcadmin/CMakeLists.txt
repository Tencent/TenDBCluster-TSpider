# add by will
# do nothing but package the mysql-test for tcadmin
MYSQL_ADD_PLUGIN(tcadmin_mysqltest tcadmin_mysqltest.cc STORAGE_ENGINE COMPONENT Test)
