# Summary [中文文档](SUMMARY.md)

* [Introduction](README-en.md)
* Main Concepts
    * [Architecture](architecture-en.md)
    * [Key Features](key-features-en.md)
* Operation Guide
    * [Quick Start]()
        * [Deployment with Docker Compose]
        * [Basic Operation]
    * Software and Hardware Environment
        * [Software Binaries]
        * [Source Code Compilation](op-guide/compile-en.md)
        * [Hardware Requirements](op-guide/system-en.md)
    * Deployment
        * [Cluster Installation]
        * [Cluster Installation by Manual](op-guide/manual-install-en.md)
        * [Routing Management](op-guide/route-manager-en.md)
        * [Privileges Management](op-guide/cluster-grant-en.md)
    * [Operation](op-guide/cluster-operator-en.md)
        * [Cluster DDL Operating](op-guide/alter-operator-en.md)
        * [Account Management](op-guide/grant-operator-en.md)
        * [Backup](op-guide/backup-en.md)
        * [Monitor](op-guide/monitor-en.md)
        * [Checksum](op-guide/checksum-en.md)
    * [Scaling](op-guide/scale-up-down-en.md)
        * [Proxy Layer Scaling](op-guide/TSpider-scale-en.md)
        * [Data Layer Scaling](op-guide/TenDB-scale-en.md)
    * [HA](op-guide/High-availability-en.md)
        * [Proxy Layer HA](op-guide/TSpider-failover-en.md)
        * [Data Layer HA](op-guide/TenDB-failover-en.md)
        * [Tdbctl HA](op-guide/Tdbctl-failover-en.md)
    * Data Migration
        * [From MySQL to Cluster](op-guide/Data-migrate-en.md/#jump1)
        * [From Cluster to MySQL](op-guide/Data-migrate-en.md/#jump3)
* Reference Book
    * [SQL Grammar](re-book/sql-grammar-en.md)   
      * [DDL](re-book/ddl-syntax-en.md)   
      * [DML](re-book/dml-syntax-en.md)
      * [Account](re-book/grant-en.md)
      * [Proceduer](re-book/stored-procedure-en.md)
      * [Function](re-book/function-en.md)
      * [Trigger](re-book/trigger-en.md)
      * [New Grammer](re-book/new-grammar-en.md)
    * [Auto_increment](re-book/auto-increase-en.md)
    * [Transaction](re-book/transaction-en.md)
      * [Transaction Usage](re-book/transaction-en.md#jump2)
      * [Common Transaction](re-book/transaction-en.md#jump3)
      * [Distributed Transaction](re-book/transaction-en.md#jump4)
        * [Xa Commit xid One Phase With Logs](re-book/transaction-en.md#jump41)
        * [Spider_trans_rollback](re-book/transaction-en.md#jump42)
        * [Result of Transacion](re-book/transaction-en.md#jump43)
        * [Handling Suspended Transaction](re-book/transaction-en.md#jump44)
        * [Others](re-book/transaction-en.md#jump5)
    * [Similarities and Differences with MySQL](re-book/mysql-compatibility-en.md/#jump)
        * [Features Not Supported](re-book/mysql-compatibility-en.md/#jump1)
        * [DDL](re-book/mysql-compatibility-en.md/#jump21)
        * [Storage Engine](re-book/mysql-compatibility-en.md/#jump22)
        * [Auto-incement](re-book/mysql-compatibility-en.md/#jump23)
        * [Execution Plan](re-book/mysql-compatibility-en.md#jump24)
        * [Distributed Transaction](re-book/mysql-compatibility-en.md/#jump25)
        * [Proceduer](re-book/mysql-compatibility-en.md#jump26)
    * [Cluser Variables Reference](re-book/parameter-en.md)
      * [TSpider Variables](re-book/tspider-parameter-en.md)
      * [TenDB Variables](re-book/tendb-parameter-en.md)
      * [Tdbctl Variables](re-book/tdbctl-parameter-en.md)
    * [Cluser Error Message Reference](re-book/errorno-en.md)
    * [API](re-book/api-en.md)
* [About TenDB](tendb/tendb-en.md)
    * [Instant Add Column](tendb/instant-add-column-en.md)
    * [Big Column Compression](tendb/blob-compress-en.md)
    * [Binlog Compression](tendb/binlog-compress-en.md)
    * [Binlog Speed Limit](tendb/binlog-speed-limit-en.md)
    * [XA Transaction Optimization](tendb/xafeatures-en.md)
	* [Grammar Check Tools](tendb/tmysqlparse-en.md)
* [TenDB Cluster in Kubernetes](kubernetes/cluster-in-k8s-en.md)
    * [TenDB Cluster Operator](kubernetes/cluster-operator-en.md)
    * [Deployment]
    * [Configuration]
    * [Monitoring]
    * [Operation]
    * [Upgrade]
    * [Scaling]
    * [HA]
    * [FAQ]
* Best Practices
    * [General Scenarios](practice/common-bestpractice-en.md)
    * [Big Data Scenarios](practice/bigdatapractice-en.md)
    * [Transaction Scenarios](practice/transactionapplication-en.md)
* FAQ
    * [Multi-unique Key Adjustment](com-problem/multi-unique-key-adjust-en.md)
    * [Shard Key Selection](com-problem/shard-key-choose-en.md)
* [Performance](performance-test-en.md)
* Technical Support
    * [Contact Us](contribution/concat-en.md)
* Contribution
    * [Source Code Contribution](contribution/contribute-en.md)
    * [Feedback](contribution/contribute-en.md#jump2)
* [Acknowledgements](acknowledgements-en.md)