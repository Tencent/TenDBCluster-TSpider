# perl mysql-test-run.pl binlog_encryption.rpl_typeconv 'row'    blob compressed
# perl mysql-test-run.pl innodb.innodb_uninstall 'innodb'        base skip
# perl mysql-test-run.pl innodb.restart                          base fail 
# perl mysql-test-run.pl innodb.innodb-wl5522 'innodb'           base fail 
# perl mysql-test-run.pl perfschema_stress.read                  base fail
# perl mysql-test-run.pl perfschema_stress.setup                 base fail 
# perl mysql-test-run.pl rpl.rpl_typeconv 'row'           blob compressed        
