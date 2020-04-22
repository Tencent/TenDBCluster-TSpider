# TenDB Cluster
TenDB Cluster is a MySQL distributed relational database solution developed and maintained by Tencent Game DBA team. It consists of three core components: TSpider, TenDB and Tdbctl.   

TSpider is the proxy layer of TenDB Cluster. It is a customized version developed by Tencent Game DBA based on MariaDB 10.3.7. It mainly improves and customizes the distributed MySQL storage engine of spider; TSpider [github page](https://github.com/TencentDBA/TendbCluster3)

TenDB is the data layer of TenDB Cluster. It is a customized MySQL branch developed by Tencent Game DBA based on Percona 5.7.20. It provides some greate features internally for business operations; TenDB [github page](https://github.com/TencentDBA/TenDB3)

Tdbctl is the control layer of TenDB Cluster, providing cluster routing management, cluster DDL operating, cluster monitoring and some other cluster capabilities. Tdbctl [github page](https://github.com/TencentDBA/Tdbctl)

TenDB Cluster Operator is capable of deploying and managing TenDB Cluster on mainstream cloud infrastructure (Kubernetes). TenDB Cluster Operator [github page]()


## Introduction to TenDB Cluster
TenDB Cluster is a MySQL distributed relational database solution developed and maintained by Tencent Game DBA team. The main features include: transparent database/table sharding, high-availability, online scaling. With these features, developers can focus on the development and operation of their product without caring about data sharding logic. Also, in the case of massive concurrency, there is no need to care about the load pressure of the DB storage layer.

## Quick start
> docker-compose

## [Documentation](Documentation/SUMMARY-en.md)
For details, see [Reference Manual](Documentation/SUMMARY-en.md)

## Contact us
For any questions or discussions about TenDB Cluster, please feel free to give us feedback via issues:     
TSpider:  https://github.com/TencentDBA/TendbCluster3/issues   
TenDB:    https://github.com/TencentDBA/TenDB3/issues   
Tdbctl:   https://github.com/TencentDBA/Tdbctl/issues 