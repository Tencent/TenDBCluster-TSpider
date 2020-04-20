# 事务应用

TenDB Cluster支持单机事务，分布式事务。本文提供TenDB Cluster事务使用的方法以及经验。




## 用法
使用方法可以参考[事务章节](../re-book/transaction.md/#usage)

## 注意点
使用事务过程需要注意以下几个参数:  
分布式事务参数:[SPIDER_INTERNAL_XA](../re-book/tspider-parameter.md/#SPIDER_INTERNAL_XA) 为只读参数，如果开启，需要配置到my.cnf;  
启动分布式事务需要开启:[spider_trans_rollback](../re-book/tspider-parameter.md/#spider_trans_rollback),保证事务原子性;  
防止业务框架中默认的autocommit=0（非业务预期）,可以开启:[SPIDER_IGNORE_AUTOCOMMIT](../re-book/tspider-parameter.md/#SPIDER_IGNORE_AUTOCOMMIT);  
防止性能损耗，可以关闭[SPIDER_WITH_BEGIN_COMMIT](../re-book/tspider-parameter.md/#SPIDER_WITH_BEGIN_COMMIT)，即非显式事务不分发begin/commit到remote mysql，提升性能。


## 如何处理悬挂事务
使用方法可以参考[分布式事务章节](../re-book/mysql-compatibility.md#jump253)


## 事务级切换
使用方法可以参考[SQL语法章节](../re-book/new-grammar.md#write_lock)
具体操作为:  
```
flush table with write lock;
update mysql.servers;
flush privileges;
unlock tables;
```

