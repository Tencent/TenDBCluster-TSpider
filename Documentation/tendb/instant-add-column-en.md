# Instant ADD COLUMN
In practice, there are many online DDL operations in addition to DML operations. Unlike DML, DDL involves the process of table rebuild due to changes in their structures, where some write operations would be blocked. This can lead to low efficency of applications. 

The processing of online DDL differs in different MySQL versions. Normaly, there are three ways of handling online DDL.  
1. COPY TABLE  
The principle of COPY TABLE is: create a temporary table with the target table strucutre, and perform a rename operation after copying the original data; the original table is readable during the entire process. but is not writable and it will cost about double storage space of the table.  
2. INPLACE  
The INPLACE solution is to update directly on the original table, and there will be no copying temporary table. Compared with COPY TABLE, the efficiency is higher, but there are more limitations. Also, the original table is not writable during the entire process.  
3. ONLINE  
Compared with the two aforementioned methods, ONLINE solution allows to write and read tables, this solution is introduced after MySQL 5.6.

If dig into the MYSQL ONLINE ADD COLUMN, we will find that ONLINE DDL has a temporary lock table. In the case of large concurrency scenarios, it is possible that DDL will eventually fail because of lock table timeout. Also, because of large mount of incremental data, the write operations will be blocked when 
rebuilding the table.

Different from ONLINE DDL's field-adding behavior, 
TenDB uses a INSTANT ADD COLUMN method, which extends the format of data dictionary and identify column-adding behaviors. The operation can be done in seconds for column-adding behaviors which apply the rules. The operation of adding columns took a long time before, and TenDB fundamentally solved this problem by introduce this new way called INSTANT ADD COLUMN.

## Instant ADD COLUMN Example
```sql
mysql> create table t(a int, b int);
Query OK, 0 rows affected (0.00 sec)

mysql> insert into t values(1,2); 
Query OK, 1 row affected (0.00 sec)

mysql> alter table t add column(c int);
Query OK, 0 rows affected (0.00 sec)
Records: 0  Duplicates: 0  Warnings: 0

#Show result
mysql> select * from t;
+------+------+------+
| a    | b    | c    |
+------+------+------+
|    1 |    2 | NULL |
+------+------+------+
1 row in set (0.00 sec)
```

## Principle of Instant ADD COLUMN
In order to meet the performance requirements of different application scenarios, it provides four row formats, namely `Redudant`, `Compact`, `Dynamic` and `Compressed`. By default, the row format is `Compact`. Instant ADD COLUMN extends the header of `Compact` and `Dynamic` row format and acheive an instant column-adding. 

### Row Format
The following uses the `Compact` format as an example to explain the implementation details of instant column-adding:  

The storage format of compact format clustered index is shown in the figure below, which is compsed of the header and the record content. Among them, the storage order of "non-null fields other than the primary key" is the same as the column definition order.

![pic](../pic/compact-cluster-index-en.png)

The header records crucial information about fields, its structure is shown in the figure below:  
![pic](../pic/compact-record-head-en.png)

-Record Extra Bytes: fixed 5 bytes, records additional information
-Nullable Bitmap: record nullable bitmap, use 1 bit to indicate whether a nullable field is NULL
-non-NULL Variable-Length Array: array of non-null variable-length fields, specifying the length of non-null variable-length fields (such as varchar, varbinary) in the row.

In order to store the information of the new field, we only need to change the record header in the Compact format.

![pic](../pic/compcat-record-head-new-en.png)

`Field Count` is a new field in header which stores the number of fields when inserting, occupying 1 to 2 bytes (if the number of fields exceeds 127, than it takes 2 bytes). 

Suppose the Field count record of a record is x and the number of table fields is y, then x <= y is always true. For the modified format, the field processing becomes:

  * If x == y, process it as it was.
  * If x < y, then the record must be in
   Generated before column-adding, then the last y-x fields must be NULL or the default value.
    -If the field is allowed to be NULL, it is NULL.
    -If the field is not allowed to be NULL, it is the default value.

The entire process is as follows

![pic](../pic/record-resolv-en.jpg)

The new `Field Count` in the row format stores the number of recorded fields. If the table did not perform the column-adding, this part would not be used. Therefore, if no column-adding are performed, the `Field Count` information does not have to be stored in the record, and can be stored in the extended data dictionary, so that before the column is added, the `Field Count` field can be used to reduced storage cost.
After the transformation of the Compact format, the problem of processing new fields in the storage format is solved, which is crucial to INSTANT ADD COLUMN.

### Data Dictionary
InnoDB's data dictionary about columns does not contain the default value information, the default value is stored in the `frm` file.  

In order to support the operation of adding columns with default values, the InnoDB storage engine layer needs to store the default value information of newly added columns. One way is to add the `def_val` attribute to the `SYS_COLUMNS` table to record the default value. But this will bring compatibility issues with the official MySQL version. Also, we only need to save the default value information of the newly added column. Therefore, add a system table `SYS_ADDED_COLS_DEFAULT`, save the default value of the newly added column, and associate with the column information of` SYS_COLUMNS` through (`tableid`, `pos`):
```sql
CREATE TEMPORARY TABLE `INNODB_SYS_ADDED_COLS_DEFAULT` (
  `TABLE_ID` bigint(21) unsigned NOT NULL DEFAULT '0',
  `POS` int(11) unsigned NOT NULL DEFAULT '0',
  `DEF_VAL` blob,
  `DEF_VAL_LEN` int(11) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8
);
```
The description of each field in the table is as follows:

| Column name | 	Description|
| :--- | :----|
|table_id|The unique identifier of the table|
|pos|Column number|
|def_val|The content of the default value is saved in binary. The maximum length of the default value is 65535, which is guaranteed by the upper frm format; for numeric types, big-endian storage is used; for characters / binaries, both are stored in byte streams.|

The default value is a new field of NULL (no non-nullable constraint), and does not need to be recorded in the `SYS_ADDED_COLS_DEFAULT` dictionary table, because the `DEF_VAL` occupied length of the `NULL` record in redundant format is 65535, and the insertion will fail with `row_size_too_big`.

For compatibility with MySQL, `SYS_ADDED_COLS_DEFAULT` creates dynamically. When TenDB starts to load the data dictionary subsystem and finds that the table does not exist, it will create it dynamically. In this way, when upgrading from MySQL to TenDB in place, there is no need to consider incompatibilities.

### Process of INSTANT ADD COLUMN
The extended row format of TenDB supports processing newly added fields without changing the original data, and the metadata can be directly modified on the original table without copying the data. Therefore, the time for adding column can be greatly shortened.

The process of INSTANT ADD COLUMN is:
1. Add the dictionary lock (SHARED_NO_WRITE) on the target table `T1`, blocking all write operations;
2. Check whether the conditions for adding fields online are met. If yes, go to step 3. Otherwise, add fields according to the method of copying data;
3. Create a temporary table `frm` file `S1.frm` with the same structure as the target table T1 (including new columns)
4. The dictionary lock of the target table `T1` is upgraded to an exclusive lock, and all read and write operations on the `T1` table are blocked
5. Modify the InnoDB internal data dictionary information of T1 table and add new fields.
6. Rename `S1.frm` to `T1.frm`
7. Release the dictionary lock of table `T1`

As can be seen from the above process, TenDB instant adding columns cancels the lock table problem caused by the data copy process that may take too long or the application of incremental data. At the same time, no new table is created, we just generate the corresponding `frm` file. In most cases, instant adding columns can return immediately, which is really instant.
In addition, based on the way instant adding columns does, TenDB has developed an online DDL operation execution framework. The instant operations supported on other storage formats can be easily extended, such as the implementation of online addition of fields to the partition table, and the instant column renaming, instant column modification, etc.

## Limitations
- `redundant` and `compressed` row format are not supported;
- One or more columns can be added by `ALTER TABLE` once, but multiple `ALTER TABLE` with manipulation types is not allowed. (such as adding/deleting indexes, deleting fields, modifying fields, etc.);  
- Does not support specifying the FIRST or AFTER keywords to indicate the definition order;
- Cannot contain any constraints except not null, including foreign key constraints, unique constraints;
- Does not support nullable fields have default value;
- The added field cannot be added automatically (auto_increment);
- Table contains full text index;
- Table contains geographic information index;
- Add Percona compressed column format is not allowed;
- Table which has performed INSTANT ADD COLUMN can not perform `flush tables xx for export`;
- Table which has performed INSTANT ADD COLUMN can not perform `alter table xx discard tablespace`

## Miscellaneous
Currently, TenDB does not support adding columns that are nullable and specify default values. In fact, it can be supported.
That is, no longer distinguish whether the column is empty, all from the data dictionary in memory, but it needs to ensure the following two points
- innobase_add_one_instant：For information with a default value of NULL, do not insert `SYS_ADDED_COLS_DEFAULT`
- dict_load_added_cols_default：for new fields that are not recorded in the `SYS_ADDED_COLS_DEFAULT` table, the default value is NULL, and at the same time ensure that dict_col_default_t must have a value and the value is NULL.