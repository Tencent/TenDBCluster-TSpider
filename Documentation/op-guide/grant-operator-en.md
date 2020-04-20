# TenDB Cluster GRANT
When an application access TenDB Cluster, it needs to apply the corresponding permission through the `GRATN` statement on the TSpider node. After receiving the` GRATN` statement, TSpider will directly forward it to the Tdbctl node. 
Tdbctl will rewrite and distributed to other TSpider nodes for execution in parallel, thus completing the whole Authorization operation of the cluster.
The syntax of grant permission is the same as ordinary MySQL, you can refer to stand-alone MySQL for use and management.

## GRANT example
Access to any TSpider node:
>mysql -umysql -pmysql -h127.0.0.1 -P25000
- GRANT
```sql
create user abc@'127.0.0.1' identified by 'abc' ;
grant select,insert,alter on test1.* to abc@'127.0.0.1';
```
- check permission
> mysql -uabc -pabc -h127.0.0.1 -P25000
```
#show grants:
show grants;
+------------------------------------------------------------------------------------------------------------+
| Grants for abc@127.0.0.1                                                                                   |
+------------------------------------------------------------------------------------------------------------+
| GRANT USAGE ON *.* TO 'abc'@'127.0.0.1' IDENTIFIED BY PASSWORD '*0D3CED9BEC10A777AEC23CCC353A8C08A633045E' |
| GRANT SELECT, INSERT, ALTER ON `test1`.* TO 'abc'@'127.0.0.1'                                              |
+------------------------------------------------------------------------------------------------------------+
use test1;
insert into t1 values(9,10,11);
```