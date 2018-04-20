################################################################################
# t/partition_blocked_sql_funcs.inc                                            #
#                                                                              #
# Purpose:                                                                     #
#  Tests around sql functions                                                  #
#                                                                              #
#                                                                              #
#------------------------------------------------------------------------------#
# Original Author: HH                                                          #
# Original Date: 2006-11-22                                                    #
# Change Author:                                                               #
# Change Date:                                                                 #
# Change:                                                                      #
################################################################################
--echo -------------------------------------------------------------------------
--echo ---  $sqlfunc  in partition with coltype  $coltype
--echo -------------------------------------------------------------------------
--echo must all fail!
--disable_warnings
drop table if exists t1 ;
drop table if exists t2 ;
drop table if exists t3 ;
drop table if exists t4 ;
drop table if exists t5 ;
drop table if exists t6 ;
--enable_warnings

--disable_abort_on_error
--disable_warnings
drop table if exists t11 ;
drop table if exists t22 ;
drop table if exists t33 ;
drop table if exists t44 ;
drop table if exists t55 ;
drop table if exists t66 ;
--enable_warnings

eval create table t11 (col1 $coltype) engine=$engine ;

eval create table t22 (col1 $coltype) engine=$engine ;

eval create table t33 (col1 $coltype) engine=$engine ;

eval create table t44 (colint int, col1 $coltype) engine=$engine ;

eval create table t55 (colint int, col1 $coltype) engine=$engine ;

eval create table t66 (colint int, col1 $coltype) engine=$engine ;

--disable_warnings
drop table if exists t1 ;
drop table if exists t2 ;
drop table if exists t3 ;
drop table if exists t4 ;
drop table if exists t5 ;
drop table if exists t6 ;
drop table if exists t11 ;
drop table if exists t22 ;
drop table if exists t33 ;
drop table if exists t44 ;
drop table if exists t55 ;
drop table if exists t66 ;
--enable_warnings

