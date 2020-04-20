# SQL审核工具
TenDB Cluster基于MySQL 5.7的语法，开发SQL审核工具（tmysqlparse）。用于对TenDB Cluster的SQL语句进行语法分析，判定语法正确性及检测高危告警。


## 使用介绍
tmysqlparse 是一个独立组件，能够兼容 MySQL 的输入（终端/文件，delimiter 断句)。完全的支持MySQL语法（MySQL 5.7, TenDB 3），并兼容多个 MySQL 版本的保留字。能够提取语句类型，包括自定义类型（`CREATE_TABLE_WITHOUT_INDEX`），能够提取库、表、索引等信息。
### 常用参数介绍
SQL 解析组件有如下参数：
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
常用的参数有：  
>-f 指定输出目录  
`-v` 指定版本，建议使用 `-v tmysql-5.7`  
`-c` 指定字符集  
`-w` 表示对建表语句按照TSpider的建表规则解析  
`-W` 指定输出文件，与`-w`配合使用  




### SQL输入
SQL审核工具tmysqlparse提供两种输入方式：


#### 终端输入
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
>`./tmysqlparse.sh -c gbk  -v tmysql-5.7  test`中的`test`为指定的DB名

#### 文本输入
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

### 结果输出

tmysqlparse以xml的形式输出SQL检查的结果，可以输出到终端，也可以输出到文件。

输出结果通过如下定义：
```html
<result></result> 中包含tmysqlparse分析后的所有结果。 
<syntax_failed></syntax_failed> 包含所有语法出错的信息。
<failed_info></failed_info> 包含一条出错语句，里面再分 <sql>、<error_code>、<error_msg> 和 <line> 四部分来输出出错SQL语句的信息。
<risk_warnings></risk_warnings> 包含所有的高危告警信息，产生告警的前提是语法正确，与 <syntax_failed></syntax_failed> 互相独立，不存在交集。
<warning_info></warning_info> 包含一条产生高危告警的SQL语句信息。
<type>、<name>、<text> 和 <line> 四部分给出告警SQL语句信息。
<info></info> 则存储额外的信息
```
其中type中包含是产生告警的类型，常见的type有：  
>`DROP_DB`  删除数据库操作  
`DROP_TABLE`  删除表操作  
`DROP_VIEW` 删除视图操作  
`TRUNCATE` 清空表操作  
`DELETE_WITHOUT_WHERE` 删除操作不带where条件  
`UPDATE_WITHOUT_WHERE` 更新操作不带where条件  
`CREATE_TABLE_WITH_MUCH_BLOB` 创建表时blob/text字段数大于10  
`ALTER_TABLE_ADD_MUCH_BLOB` 更改表增加的blob/text字段数大于10  
`CREATE_TABLE_NOT_INNODB` 建表指定了非innodb引擎  


使用示例：  
tmysqlparse输入文件内容如下：
```sql
create table t1(c1 int,c2 int);
delete * from t1;
delete  from t1;
```

tmysqlparse 会审核这每条SQL语句，我们会发现其中1个语法错误:  
`delete * from t1;`这条SQL多一个*  

2个高危SQL：  
`create table t1(c1 int,c2 int);`建表没有索引  
`delete  from t1;`没有where条件限制  

结果如下：
```sql
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



### 使用建议

#### TenDB变更SQL语法检查
```bash
./tmysqlparse.sh -c gbk  -v tmysql-5.7    test  -f  test.result  < ./a.sql
```

#### TSpider变更SQL语法检查
由于TSpider的DDL语法对于分区键有特殊的要求，会有一些不同于MySQL的语法报错，所以需要指定-w -W，来对TSpider进行SQL解析

```bash
./tmysqlparse.sh -c gbk  -v tmysql-5.7    test  -w -W   test.result  < ./a.sql
```



例如对于sql:
```sql 
create table test.t(c1 int primary key,c2 int,unique key a(c2));
```

tmysqlparse 会解析到这个SQL在TSpider上建表是不合法的，输出结果如下：
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

其中parse_result为解析结果，常见的错误有：

>ERROR: %s as TSpider key, but not exist  
指定shard key不存在

>ERROR: %s as TSpider key, but not in some unique key  
shard key并不存在唯一键字段列表中

>ERROR: too more unique key with the different pre key  
建表SQL中存在多个唯一键
