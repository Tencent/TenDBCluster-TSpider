# TenDB Cluster Error Message Reference
This section provides a reference for the various types of error messages in TenDB Cluster. 

## Error Message Reference 
TenDB Cluster is compatible with MySQL error messages, and in most cases will return the same error codes as MySQL.   
The following lists some other error messages specific to TenDB Cluster.

|Error number|Symbol|Message|
|-|-|-|
|12500|ER_SPIDER_UNKNOWN_NUM|unknown|
|12501|ER_SPIDER_INVALID_CONNECT_INFO_NUM|The connect info '%-.64s' is invalid"|
|12502|ER_SPIDER_INVALID_CONNECT_INFO_TOO_LONG_NUM|The connect info '%-.64s' for %s is too long|
|12518|ER_SPIDER_READ_ONLY_NUM|Table '%s.%s' is read only|
|12521|ER_SPIDER_SQL_WRAPPER_IS_INVALID_NUM|Can't use wrapper '%s' for SQL connection|
|12524|ER_SPIDER_CANT_OPEN_SYS_TABLE_NUM|Can't open system table %s.%s|
|12601|ER_SPIDER_CANT_USE_BOTH_INNER_XA_AND_SNAPSHOT_NUM|Can't use both spider_use_consistent_snapshot = 1 and spider_internal_xa = 1|
|12602|ER_SPIDER_XA_LOCKED_NUM|This xid is now locked|
|12603|ER_SPIDER_XA_NOT_PREPARED_NUM|This xid is not prepared|
|12604|ER_SPIDER_XA_PREPARED_NUM|This xid is prepared|
|12605|ER_SPIDER_XA_EXISTS_NUM|This xid is already exist|
|12609|ER_SPIDER_SYS_TABLE_VERSION_NUM|System table %s is different version|
|12610|ER_SPIDER_XA_MAY_PARTIAL_COMMIT_NUM|XA transaction may partial commit|
|12701|ER_SPIDER_REMOTE_SERVER_GONE_AWAY_NUM|Remote MySQL server has gone away|
|12702|ER_SPIDER_REMOTE_TABLE_NOT_FOUND_NUM|Remote table '%s.%s' is not found|
|12710|ER_SPIDER_INVALID_REMOTE_TABLE_INFO_NUM|Invalid information from remote table '%s.%s'|
|12714|ER_SPIDER_TABLE_OPEN_TIMEOUT_NUM|Table %s.%s open timeout|
|12723|ER_SPIDER_CON_COUNT_ERROR|Too many connections between spider and remote|
|12724|ER_SPIDER_GET_SHARE_ERROR|failed get spider share|
|12725|ER_SPIDER_CONN_BE_FREE_NUM|conn may be free|
|12726|ER_SPIDER_COND_INVALID_NUM|where condition may be invalid|
|12727|ER_SPIDER_SHARE_INVALID_NUM|may duplicate error after flush with no block|
|12728|ER_SPIDER_XA_TIMEOUT_NUM|transaction execute timeout, wait some seconds and see the results|
|12729|ER_SPIDER_XA_FAILED_NUM|transaction execute failed, please retry transaction|
