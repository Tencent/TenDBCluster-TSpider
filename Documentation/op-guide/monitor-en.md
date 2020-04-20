# TenDB Cluster Monitor
TenDB Cluster involves multiple components. This document mainly describes the items that each group of components needs to pay attention to in daily operations.

## TSpider
TSpider is the access layer of TenDB Cluster. After accessing the application request, TSpider will rewrite the SQL through data routing rules and distribute it to the corresponding storage node TenDB for execution.

The main monitoring items of TSpider are as follows:
1. Whether the TSpider instance port can be connected
2. TSpider connection number check: max_connect_errors occurs when the number of connections exceeds the max_connections limit
3. Can the TSpider instance connect to the backend TenDB instance or not.
4. TSpider mysqldata directory size check: TSpider does not retain application data, the mysqldata directory is generally not too large, if it is too large, you need to confirm whether it is normal. The experience value is generally not more than 10G.
    The following situations may cause the mysqldata directory to be too large:
    *    Relay log space consumption during data migration
    *    Temporary space occupied by pulling data on TSpider
    *    If there are too many tables on TSpider, the table statistics space is consumed
5. Abnormality connection check on TSpider is mainly related to abnormal state check related to lock status, such as "Waiting for table flush".
6. Specific error code check in TSpider.
    About TSpider error code,can refer to [Error-code](./../re-book/errorno-en.md)
7. The OS level mainly focuses on the following indicators:
    *  CPU utilization
    *  Memory utilization
    *  Disk space and IO utilization
    *  Network traffic


## TenDB
TenDB is the data storage layer of TenDB Cluster. A cluster backend generally has multiple groups of TenDB instances. Each group of TenDB can use the master-standby deployment or MGR to ensure the availability of the storage layer.

The main monitoring items of TenDB are as follows:
1. Whether the TenDB instance port can be connected
2. TenDB connection number check: max_connect_errors appears when the number of connections exceeds the max_connections limit
3. Check the abnormal status of the connection on TenDB, mainly related to the abnormal status check related to the lock status
4. Check the specific error report in the error log of TenDB instance. Keywords include ERROR, restarted, hanging, locked, etc
5. Slow queries: if it exceeds the threshold, intervene in the process.
6. DB concurrent threads monitoring
7. Slave status check, mainly related to:
    *  IO thread check
    *  SQL thread check
    *  Slave Last Errno check
    *  Slave delay status check 
6. checksum execution result check 
    *  Recommended to use pt-table-checksum to check the consistency of the active and standby data.
    *  Check the results of checksum regularly. Handle this exception if there is inconsistency.
8. The OS level mainly focuses on the following indicators:
    *  CPU utilization
    *  Memory utilization
    *  Disk space and IO utilization
    *  Network traffic
    
Note: Please refer to other chapters for performance/log collection and display.
