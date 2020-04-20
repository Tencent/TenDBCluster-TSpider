# 函数
函数是存在数据库中的一段sql集合，调用函数可以减少很多工作量。TenDB Cluster兼容MySQL函数语法。



<font color="#dd0000">注意：</font>   
>不建议在TenDB Cluster中使用复杂的存储过程，触发器，函数。   
在对数据一致性高要求的场景中使用存储过程或者触发器，建议配置分布式事务；否则TenDB Cluster只能支持单机事务。 





## 创建函数
```
MariaDB [tendb_test]> DELIMITER //
MariaDB [tendb_test]> CREATE FUNCTION GetEmployeeInformationByID(id INT)
    -> RETURNS VARCHAR(300)
    -> BEGIN
    -> RETURN(SELECT CONCAT('employee name:',employee_name,'---','salary: ',employee_salary) FROM employees WHERE employee_id=id);
    -> END//
Query OK, 0 rows affected (0.03 sec)
```

## 调用函数
```
MariaDB [tendb_test]> create table employees
    -> (
    -> employee_id int(11) primary key not null auto_increment,
    -> employee_name varchar(50) not null,
    -> employee_sex varchar(10) default '男',
    -> hire_date datetime not null default current_timestamp,
    -> employee_mgr int(11),
    -> employee_salary float default 3000,
    -> department_id int(11)
    -> );
Query OK, 0 rows affected (0.06 sec)


MariaDB [tendb_test]> insert into employees(employee_name,employee_sex,employee_mgr,employee_salary,department_id) values ('David Tian','男',10,7500,1);
Query OK, 1 row affected (0.00 sec)


MariaDB [tendb_test]> select  GetEmployeeInformationByID(1);
+-----------------------------------------+
| GetEmployeeInformationByID(1)           |
+-----------------------------------------+
| employee name:David Tian---salary: 7500 |
+-----------------------------------------+
1 row in set (0.00 sec)

```

## 删除函数
```
MariaDB [tendb_test]> DROP FUNCTION GetEmployeeInformationByID;
Query OK, 0 rows affected (0.04 sec)
```
