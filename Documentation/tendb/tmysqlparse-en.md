# SQL Syntax Analysis Tool
Based on the syntax of MySQL 5.7, TenDB Cluster developed a SQL grammatical analysis tool called *tmysqlparse*.  
It is used for grammatical analysis for SQL statements of TenDB Cluster to determine the correctness of syntax and raise high-risk alarms if needed.

## Introduction
*tmysqlparse* is an independent component that is compatible with MySQL input (terminal / file, delimiter sentence breaking). It is compatible with MySQL5.7, TenDB3 syntaxs and with reserved words of multiple MySQL versions. It is able to extract statement types, including custom types (`CREATE_TABLE_WITHOUT_INDEX`), and can extract information such as libraries, tables, and indexes.

### Variables
Some variables are listed below:
```bash 
[mysql@GCS-DEV2 ~/GCS/assistant_tools/jm/lib/tmysqlparse_v5.7]$  ./tmysqlparse.sh --help
tmysqlparse Ver 2.0
Usage: /data/home/mysql/GCS/assistant_tools/jm/lib/tmysqlparse_v5.7/tmysqlparse
  -?, --help          Display this help and exit.
  -f, --file=name     assign the file name of audit result.
  -d, --all-dml-output=name
                      output the result if all sql is DML, 0 or 1.
  -S, --split-sql-path=name
                      set the path to store the split sql file

  -W, --show-create-path=name
                      convert the create table sql into SHOW CREATE TABLE.
  -V, --version       Output version information and exit.
  -F, --force         Continue even if we get an SQL error.
  -v, --set-version=name
                      choose a version to parse sql, like, "5.0" "5.1" "5.5"
                      "tmysql-1.0"  "tmysql-1.1" "tmysql-1.2" "tmysql-1.3"
                      "tmysql-1.4" "tmysql-1.5" "tmysql-1.6" "5.6" "tmysql-2.0"
                      "tmysql-2.1"default value is "5.5"
  -c, --set-charset=name
                      set the charset of db.
  -T, --get-only-ntables
                      sqlparse output table counts only.
  -s, --split-sql     split the sql into create.sql alter.sql and dml.sql.
  -w, --show-create   convert the create table sql into SHOW CREATE TABLE.
  -b, --base64        convert the sql inputed in base64 format
```
Some common variables are：  
>`-f`:  specifies the output directory;  
`-v`:  specifies the version `-v tmysql-5.7` is recommended to use;  
`-c`:  specifies the character set;  
`-w`:  analyze creating table statements according to TSpider's rules  
`-W`:  specifies the output file, should be used together with `-w`




### Input SQL
There are two ways to input into *tmysqlparse*:


#### Input from the Terminal
```bash 
[mysql@GCS-DEV2 ~/GCS/assistant_tools/jm/lib/tmysqlparse_v5.7]$  ./tmysqlparse.sh -c gbk  -v tmysql-5.7    test
Welcome to the TMySQLParse monitor.  Commands end with ; or \g.
Reading history-file /data/home/mysql/.mysql_history
Type 'help;' or '\h' for help. Type '\c' to clear the current input statement.

tmysqlparse> create table t(c1 int primary key,c2 int,unique key a(c2));
tmysqlparse> \q
<?xml version="1.0" encoding="ISO-8859-1"?>
<result>
        <info>
                <non_ascii>0</non_ascii>
                <is_all_dml></is_all_dml>
        </info>
</result>
<?xml version="1.0" encoding="ISO-8859-1"?>
<is_all_dml>TRUE</is_all_dml>
Writing history-file /data/home/mysql/.mysql_history
Bye
```
注：
>The `test` in `./tmysqlparse.sh -c gbk  -v tmysql-5.7  test` specifies the name of the database;

#### Input from Files
```bash
[mysql@GCS-DEV2 ~/GCS/assistant_tools/jm/lib/tmysqlparse_v5.7]$ cat a.sql
create table t(c1 int primary key,c2 int,unique key a(c2));


[mysql@GCS-DEV2 ~/GCS/assistant_tools/jm/lib/tmysqlparse_v5.7]$  ./tmysqlparse.sh -c gbk  -v tmysql-5.7    test < ./a.sql
<?xml version="1.0" encoding="ISO-8859-1"?>
<result>
        <info>
                <non_ascii>0</non_ascii>
                <is_all_dml></is_all_dml>
        </info>
</result>
<?xml version="1.0" encoding="ISO-8859-1"?>
<is_all_dml>TRUE</is_all_dml>

```

### Results Ouput

*tmysqlparse* outputs the result of the SQL check in the form of xml, which can be output to the terminal or to a file.

The output format is as follows:
```html
<result></result> includes all result from tmysqlparse; 
<syntax_failed></syntax_failed> covers all synatx errors;
<failed_info></failed_info> specifies an error statement, and its details are covered in <sql>、<error_code>、<error_msg> and <line>
<risk_warnings></risk_warnings> records all high-risk statements, this statement is grammaly correct, so they are not in the <syntax_failed></syntax_failed> field.
<warning_info></warning_info> covers the information about high-risk SQL, and its details are covered in 
<type>、<name>、<text> and <line>
<info></info> stores some other information
```
Among them, the <type> field contains the type of alarm, some common types are:  
>`DROP_DB`  drop databases  
`DROP_TABLE`  drop tables  
`DROP_VIEW`  delete views  
`TRUNCATE`  clear tables  
`DELETE_WITHOUT_WHERE`  deletion without `where` condition  
`UPDATE_WITHOUT_WHERE`  update without `where` condition  
`CREATE_TABLE_WITH_MUCH_BLOB`  there are more than 10 blob/text columns when creating the table  
`ALTER_TABLE_ADD_MUCH_BLOB` there are more than 10 blob/text columns after altering the table  
`CREATE_TABLE_NOT_INNODB`  the table under creation uses a storage engine other than InnoDB  


A practical example is as follows:  
`tmysqlparse` input:
```sql 
create table t1(c1 int,c2 int);
delete * from t1;
delete  from t1;
```

tmysqlparse will check the statement and detect a syntax(lexical) error:
`delete * from t1;` where `*` is not needed.  

And two high-risk SQL are detected：  
`create table t1(c1 int,c2 int);` create a table without an explicit index.  
`delete  from t1;` deletion without `where` condition.  

结果如下：
```bash 
[mysql@GCS-DEV2 ~/GCS/assistant_tools/jm/lib/tmysqlparse_v5.7]$  ./tmysqlparse.sh -c gbk  -v tmysql-5.7    test  -f  test.result  < ./a.sql
<?xml version="1.0" encoding="ISO-8859-1"?>
<is_all_dml>FALSE</is_all_dml>
[mysql@GCS-DEV2 ~/GCS/assistant_tools/jm/lib/tmysqlparse_v5.7]$ cat test.result
<?xml version="1.0" encoding="ISO-8859-1"?>
<result>
        <syntax_fail>
                <failed_info>
                        <sql>delete * from t1</sql>
                        <error_code>1064</error_code>
                        <error_msg>You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '* from t1' at
line 1</error_msg>
                        <line>2</line>
                </failed_info>
        </syntax_fail>
        <risk_warnings>
                <warning_info>
                        <type>CREATE_TABLE_NO_INDEX</type>
                        <name>test.t1</name>
                        <text>create table t1(c1 int,c2 int)</text>
                        <line>1</line>
                </warning_info>
                <warning_info>
                        <type>DELETE_WITHOUT_WHERE</type>
                        <name>test.t1</name>
                        <text>delete  from t1</text>
                        <line>3</line>
                </warning_info>
        </risk_warnings>
        <info>
                <non_ascii>0</non_ascii>
                <is_all_dml></is_all_dml>
        </info>
</result>
```



### Some Suggestions

#### Check Syntax fro TenDB:
```bash 
./tmysqlparse.sh -c gbk  -v tmysql-5.7    test  -f  test.result  < ./a.sql
```

#### Check Syntax for TSpider
Since TSpider's DDL syntax has special requirements for the partition key, there will be some syntax errors that are different from MySQL, so you need to specify `-w -W` to perform SQL analysis on TSpider.

```bash 
./tmysqlparse.sh -c gbk  -v tmysql-5.7    test  -w -W   test.result  < ./a.sql
```



For example, as for the following SQL:
```sql 
create table test.t(c1 int primary key,c2 int,unique key a(c2));
```

tmysqlparse will parse this SQL, determine whether it is grammaly valid, and output the result:
```html
[mysql@GCS-DEV2 ~/GCS/assistant_tools/jm/lib/tmysqlparse_v5.7]$  ./tmysqlparse.sh -c gbk  -v tmysql-5.7    test  -w -W   test.result  < ./a.sql
<?xml version="1.0" encoding="ISO-8859-1"?>
<result>
        <info>
                <non_ascii>0</non_ascii>
                <is_all_dml></is_all_dml>
        </info>
</result>
<?xml version="1.0" encoding="ISO-8859-1"?>
<is_all_dml>TRUE</is_all_dml>

[mysql@GCS-DEV2 ~/GCS/assistant_tools/jm/lib/tmysqlparse_v5.7]$ cat test.result
<?xml version="1.0" encoding="ISO-8859-1"?>
<result>
        <sql>
                <convert_sql>create table test.t(c1 int primary key,c2 int,unique key a(c2))</convert_sql>
                <sql_type>STMT_CREATE_TABLE</sql_type>
                <db_name>test</db_name>
                <table_name>t</table_name>
                <shard_key></shard_key>
                <is_with_shard_key_comment>FALSE</is_with_shard_key_comment>
                <is_with_autoincrement>FALSE</is_with_autoincrement>
                <is_with_unique_key>TRUE</is_with_unique_key>
                <parse_result>ERROR: too more unique key with the different pre key</parse_result>
        </sql>
</result>
```

`parse_result` stores the result of analysis. 

Some common errors are listed below:

>ERROR: %s as TSpider key, but not exist  
The specifed shard key does not exist.

>ERROR: %s as TSpider key, but not in some unique key  
The specified shard key is not a unique key.

>ERROR: too more unique key with the different pre key  
There are more than one unique key in the creating table statement.
