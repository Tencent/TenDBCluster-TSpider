# 大字段压缩
TenDB具有对BLOB相关列提供压缩属性的能力，有利于减小存储空间，提高内存利用率，从而增加IO效率。本篇介绍TenDB在列压缩上的新功能。

## 压缩列创建以及适用范围
建表语句指定列compressed属性(**只适用于InnoDB存储引擎**)，拥有这个属性的列在插入时会对数据进行压缩存储，查询时会在底层进行解压。压缩/解压操作采用ZLIB算法，并对开发者透明。
```sql
create table t1 (
  C1 int,
  C2 blob compressed,
  C3 text character set gbk compressed,
  C4 blob
)engine=InnoDB;
```
支持compressed属性的列类型包括：TINYBLOB,BLOB, MEDIUMBLOB, LONGBLOB, TINYTEXT, TEXT, MEDIUMTEXT, LONGTEXT和JSON。  
请注意，VARCHAR和VARBINARY类型不支持COMPRESSED属性，对VARCHAR/VARBINARY的压缩利用`alter table`转换成 BLOB/TEXT COMPRESSED; 
```sql
create table t1 (
  id int,
  vc varchar(1000)
)engine=InnoDB;

insert into t1(vc) values(repeat('abc',200)),(repeat('xyz',300));

alter table t1 modify vc text compressed;
```
此时，t1中原有的varchar数据进行压缩后，写入到text compressed列中。  
也可以把一个普通的BLOB列修改成BLOB COMPRESSED列。同样的，数据会在压缩后插入到压缩列中。

## 压缩列索引
具有压缩属性的列**不能被直接索引**，但是可以被**虚列后索引**，如对BLOB类型生成前缀虚列后再索引，
```sql
CREATE TABLE t1(
  bcol BLOB COMPRESSED, 
  vcol VARCHAR(20) GENERATED ALWAYS AS (left(bcol, 10)),
  index idx(scol)
);
```
或对JSON的某一个元素虚列后再索引，
```sql
CREATE TABLE t1 (
    jcol JSON COMPRESSED, 
    vcol VARCHAR(20) GENERATED ALWAYS AS (jcol->"$.name"),
    index idx(scol)
);
```

## 压缩属性感知
压缩列属性可以通过`show create table`中的/*!99104 COMPRESSED */来查看
```sql 
mysql> show create table t1\G
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `bcol` blob /*!99104 COMPRESSED */
) ENGINE=InnoDB DEFAULT CHARSET=latin1
```

或`describe table`中EXTRA里的*COMPRESSED COLUMN*来查看
```
mysql> describe t1\G
*************************** 1. row ***************************
  Field: bcol
   Type: blob
   Null: YES
    Key: 
Default: NULL
  Extra: COMPRESSED COLUMN
```

## 压缩阈值控制
TenDB提供了InnoDB全局参数`innodb_min_blob_compress_length`，表示触发压缩存储的阈值。即插入具有COMPRESSED属性的列时，只有当数据长度超过此阈值，才会触发压缩逻辑。该参数默认值为256。
