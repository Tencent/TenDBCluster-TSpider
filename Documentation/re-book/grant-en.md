# TenDB Cluster Account Management 
The grammar of ccount management is the same as ordinary MySQL. There is no much difference in use.  
<font color="#dd0000">Note： </font>   
>ddl_execute_by_ctl=0  
Execute the grant statement on the TSpider node, only grant for current node,  which is no difference from ordinary MySQL.  
ddl_execute_by_ctl=1    
If Tdbctl node exists, and ddl_execute_by_ctl=1. Execute the grant statement on the TSpider node, will grant for all TSpider nodes.  TSpider will distribute the grant sql to Tdbctl, and then Tdbctl will  grant for all TSpider nodes.

## FLUSH PRIVILEGES
`FLUSH PRIVILEGES` sql can reload privileges.

<font color="#dd0000">Note：</font>   
>when maintains mysql.servers on the TSpider node without Tdbctl node,  `flush privileges` is required for the mysql.servers information to take effect .

## GRANT

GRANT <privileges> to user@'ip' identified by password  
Used to create users and assign permissions for TSpider.

```
MariaDB [tendb_test]> GRANT SELECT, INSERT, UPDATE, DELETE, DROP, EXECUTE on *.* to spider@'***' identified by '***';
Query OK, 0 rows affected (0.00 sec)
```


## SHOW GRANTS 
Used to show  the list of permissions associated with the user.
```
MariaDB [tendb_test]> show grants;

 GRANT SELECT, INSERT, UPDATE, DELETE, DROP, EXECUTE ON *.* TO 'spider'@'***' IDENTIFIED BY PASSWORD '*6CFA6C059D69E70E1E4A37045E474F9D761BB5EB' 

1 row in set (0.00 sec)
```



## SHOW PRIVILEGES 
Used to show the list of system privileges that the TSpider supports. 
