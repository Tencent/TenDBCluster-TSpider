# Big Column Compression
TenDB provides compressed attributes for BLOB related columns. It is beneficial to reduce storage cost and improve main memory utilization, thereby increasing IO efficiency. This article introduces TenDB's new features in column compression.

## Create Compressed Column
We can specify compressed attribute to some columns when creating table (only applies when the storage engine is InnoDB). Columns with compressed attribute will compress data when it is inserted, and decompress data when querying. The compression/decompression operation uses the ZLIB algorithm and is transperant to developers.

Here is an example to create compressd columns (`C2` and `C3`):
```sql
create table t1 (
  C1 int,
  C2 blob compressed,
  C3 text character set gbk compressed,
  C4 blob
)engine=InnoDB;
```
The compressed attribute is limited to columns of the following data types: `TINYBLOB`, `BLOB`, `MEDIUMBLOB`, `LONGBLOB`, `TINYTEXT`, `TEXT`, `MEDIUMTEXT`, `LONGTEXT` and `JSON`  

Please note that, we can not add compressed attribute to `VARCHAR` and `VARBINARY`. Instead, we can use `alter table` to convert `VARCHAR/VARBINARY` columns to `BLOB/TEXT COMPRESSED` ones if we want to use compressed columns. The example is shown below: 
```sql
create table t1 (
  id int,
  vc varchar(1000)
)engine=InnoDB;

insert into t1(vc) values(repeat('abc',200)),(repeat('xyz',300));

alter table t1 modify vc text compressed;
```
When this is done, the original `varchar` data in `t1` is compressed and written to the `text compressed` column.
  
You can also modify a `BLOB` column to a `BLOB COMPRESSED` one. Similarly, the data would be inserted into the column after compression.

## Index on Compressed Columns
Columns with compressed attribute can not be indexed directly. However, it can be indexed on its generated columns. For instance, you can index on a virtual generated prefix column based on a `BLOB` column:
```sql
CREATE TABLE t1(
  bcol BLOB COMPRESSED, 
  vcol VARCHAR(20) GENERATED ALWAYS AS (left(bcol, 10)),
  index idx(scol)
);
```
Or you can index on a generated column based on one JSON element:
```sql
CREATE TABLE t1 (
    jcol JSON COMPRESSED, 
    vcol VARCHAR(20) GENERATED ALWAYS AS (jcol->"$.name"),
    index idx(scol)
);
```

## How to Know a Column is Compressed
We can use `show create table` to get whether a column is compressed or not. The compressed column is shown with `/*!99104 COMPRESSED */`:

```sql
mysql> show create table t1\G
*************************** 1. row ***************************
       Table: t1
Create Table: CREATE TABLE `t1` (
  `bcol` blob /*!99104 COMPRESSED */
) ENGINE=InnoDB DEFAULT CHARSET=latin1
```

Or use `describe table` to see if there exists *COMPRESSED COLUMN* in the `Extra` field.
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

## Compression Threshold Control
TenDB provides InnoDB global variable `innodb_min_blob_compress_length`. InnoDB only do compression when the original length of data exceeded this value. This variable is 256 by default.
