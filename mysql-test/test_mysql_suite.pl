#!/usr/bin/perl -w
#
# used to test all mysql test suite cases.
#

my $test_log = "test_log";

if( -e $test_log){
    print " $test_log exists, neednot make it again\n";
}else{
    mkdir $test_log;
}

#my @suites = ('main','innodb','binlog','engines','funcs_1','funcs_2','federated','jp','rpl','sys_vars','stress','parts','perfschema','perfschema_stress','manual','gcs');

my @suites = ("archive","csv","funcs_1","gcol","innodb_fts","jp","mariabackup","optimizer_unfixed_bugs","plugins","storage_engine","vcol","binlog","encryption","funcs_2","handler","innodb_gis","json","mtr","parts","roles","stress","wsrep","binlog_encryption","engines","galera","heap","innodb_undo","large_tests","mtr2","perfschema","rpl","sys_vars","compat","federated","galera_3nodes","innodb","innodb_zip","maria","multi_source","perfschema_stress","sql_sequence","unit","auth_gssapi","connect","example","heap","innobase","metadata_lock_info","myisam","myisammrg","query_response_time","sequence","sphinx","test_sql_discovery","user_variables");

#test each suite one by one,and log to the test_log/ dir as SUITE_NAME.LOG

print "first to test 'spider' \n";
`perl mysql-test-run.pl --suite=spider --big-test --force  >${test_log}/spider.test.log 2>&1`;


foreach my $st (@suites){
    print "start to test '${st}'\n";    
    if($st eq "engines"){
         `perl ./mysql-test-run.pl --suite=engines/funcs --mysqld=--default-storage-engine=innodb --force >${test_log}/${st}_funcs.test.log 2>&1`;
         `perl ./mysql-test-run.pl --suite=engines/iuds --mysqld=--default-storage-engine=innodb --force >${test_log}/${st}_iuds.test.log 2>&1`;     
        
    }else{
    
    `perl mysql-test-run.pl --suite=$st --max-test-fail=100 --force  >${test_log}/${st}.test.log 2>&1`;
    }
    print "finished test $st\n";
}

#`chmod +x ./suite/engines/rr_trx/run_stress_tx_rr.pl`;
#`./suite/engines/rr_trx/run_stress_tx_rr.pl --engine=InnoDB > run_stress_tx_rr.log 2>&1`;

#`./mtr --force --suite=extra/binlog_tests >${test_log}/extra_binlog.test.log 2>&1`;
#`./mtr --force --suite=extra/rpl_tests >${test_log}/extra_rpl_tests.test.log 2>&1`;


# `./mtr --force --suite=large_tests --big-test --suite-timeout=6360 --testcase-timeout=795 > ${test_log}/large_tests.test.log 2>&1`;
