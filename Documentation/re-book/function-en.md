# Function
The function is a collection of sql in the database, calling function can reduce a lot of work.  TenDB Cluster is compatible with MySQL's function grammar.



<font color="#dd0000">Note：</font> 
> It is not recommended to use complex stored procedures, triggers, functions in TenDB Cluster.  
When using stored procedures or triggers in the scenarios that require high data consistency, it is recommended to configure distributed transactions; otherwise, TenDB Cluster can only support local transactions.


## Create Function
```
MariaDB [tendb_test]> DELIMITER //
MariaDB [tendb_test]> CREATE FUNCTION GetEmployeeInformationByID(id INT)
    -> RETURNS VARCHAR(300)
    -> BEGIN
    -> RETURN(SELECT CONCAT('employee name:',employee_name,'---','salary: ',employee_salary) FROM employees WHERE employee_id=id);
    -> END//
Query OK, 0 rows affected (0.03 sec)
```

## Call Function
```
MariaDB [tendb_test]> create table employees
    -> (
    -> employee_id int(11) primary key not null auto_increment,
    -> employee_name varchar(50) not null,
    -> employee_sex varchar(10) default 'male',
    -> hire_date datetime not null default current_timestamp,
    -> employee_mgr int(11),
    -> employee_salary float default 3000,
    -> department_id int(11)
    -> );
Query OK, 0 rows affected (0.06 sec)


MariaDB [tendb_test]> insert into employees(employee_name,employee_sex,employee_mgr,employee_salary,department_id) values ('David Tian','male',10,7500,1);
Query OK, 1 row affected (0.00 sec)


MariaDB [tendb_test]> select  GetEmployeeInformationByID(1);
+-----------------------------------------+
| GetEmployeeInformationByID(1)           |
+-----------------------------------------+
| employee name:David Tian---salary: 7500 |
+-----------------------------------------+
1 row in set (0.00 sec)

```

## Drop Function
```
MariaDB [tendb_test]> DROP FUNCTION GetEmployeeInformationByID;
Query OK, 0 rows affected (0.04 sec)
```
