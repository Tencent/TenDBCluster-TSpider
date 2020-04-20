# Cluster Privileges 
This chapter introduces TenDB Cluster's privileges.   
According to the privilege use case scenarios, the cluster privilege can be divided into cluster internal privilege and cluster external privilege.   
The internal privileges of the cluster are mainly used for internal management of the cluster and are very important for maintaining the normal work of the cluster. If the privileges are abnormal, which may cause the cluster service to be unavailable or abnormal.  
The external privileges of the cluster is mainly used for the access control of applications requested. Before the application requests the cluster, it need to apply for the appropriate privileges on the cluster. If the privileges is abnormal, which may cause the application service to be refused and abnormal.  

## Cluster Internal Privilege 
|Request node | 	Grant Node| Privileges|Context|
| :--- | :----|:----|:----|
|TSpider|Tdbctl|ALL PRIVILEGES WITH GRANT OPTION|TSpider node used to forward DDL to Tdbctl node|
|TSpider|TenDB|SELECT, INSERT, UPDATE, TRUNCATE|TSpider node used to access TenDB node do DML
|Tdbctl|TSpider, TenDB|ALL PRIVILEGES WITH GRANT OPTION|Tdbctl used to access TSpider, TenDB node do DDL|
|TenDB|N/A|N/A|N/A|


## Cluster External Privilege 
The privileges required by application to access TenDB Cluster.  
We can use administrator account to connect anyone TSpider node, and do GRANT according to application needs.  
There is no difference with the single MySQL in account management, more information refer to [Cluster Account Management](grant-operator-en.md) 
