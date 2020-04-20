# TenDB Cluster 权限管理
权限的语法与普通MySQL相同，在使用上没有太大的区别。

<font color="#dd0000">注意：</font>   
>ddl_execute_by_ctl=0  
在TSpider节点执行grant语句，仅对当前节点授权，用法和普通MySQL无差别  
ddl_execute_by_ctl=1    
存在中控节点，且参数打开。在TSpider节点执行grant语句，是对所有TSpider节点的授权。TSpider会将授权语句转发到Tdbctl，再由Tdbctl对所有TSpider授权。


## FLUSH PRIVILEGES
FLUSH PRIVILEGES 语句可以重新load授权。


<font color="#dd0000">注意：</font>   
>在单机TSpider(无中控节点）维护mysql.servers时，需要flush privileges让mysql.servers信息生效 ； 



## GRANT

GRANT <privileges> to user@'ip' identified by password  
用于为 TSpider 创建用户，并分配权限。

```
MariaDB [tendb_test]> GRANT SELECT, INSERT, UPDATE, DELETE, DROP, EXECUTE on *.* to spider@'***' identified by '***';
Query OK, 0 rows affected (0.00 sec)
```


## SHOW GRANTS 

用于显示与用户关联的权限列表。

```
MariaDB [tendb_test]> show grants;

 GRANT SELECT, INSERT, UPDATE, DELETE, DROP, EXECUTE ON *.* TO 'spider'@'***' IDENTIFIED BY PASSWORD '*6CFA6C059D69E70E1E4A37045E474F9D761BB5EB' 

1 row in set (0.00 sec)
```



## SHOW PRIVILEGES 
用于显示 TSpider 中可分配权限的列表。
