# TenDB Cluster
TenDB Cluster is a MySQL distributed relational database solution developed and maintained by Tencent Game CROS DBA team. It consists of three core components: TSpider, TenDB and Tdbctl.   

TSpider is the proxy layer of TenDB Cluster. It is a customized version developed by Tencent Game CROS DBA based on MariaDB 10.3.7. It mainly improves and customizes the distributed MySQL storage engine of Spider; TSpider [github page](https://github.com/Tencent/TenDBCluster-TSpider)

TenDB is the data layer of TenDB Cluster. It is a customized MySQL branch developed by Tencent Game CROS DBA based on Percona Server 5.7.20. It provides some great features internally for business operations; TenDB [github page](https://github.com/Tencent/TenDBCluster-TenDB)

Tdbctl is the control layer of TenDB Cluster. It is developed by Tencent Game CROS DBA team based on TenDB (mainly reuse TenDB's own SQL parsing capabilities and MGR capabilities). It provides cluster routing management, cluster DDL operating, cluster monitoring and some other cluster capabilities. Tdbctl [github page](https://github.com/Tencent/TenDBCluster-Tdbctl)

## Introduction to TenDB Cluster
TenDB Cluster is a MySQL distributed relational database solution developed and maintained by Tencent Game DBA team. The main features include: transparent database/table sharding, high-availability, online scaling. With these features, developers can focus on the development and operation of their product without caring about data sharding logic. Also, in the case of massive concurrency, there is no need to care about the load pressure of the DB storage layer.

## Quick Start
For more detail, see Chapter: [Quick Deployment With Docker Compose](Documentation/op-guide/docker-compose-tendbcluster-en.md)


## [Documentation](http://tendbcluster.com/)
For details, see [Reference Manual](http://tendbcluster.com/)

## Flexible Deployment
It is recommended to use TSpider + TenDB + Tdbctl to deploy TenDB Cluster.   
However, developers can also flexibly deploy TenDB Cluster according to their own online situation. Currently, companies use TSpider + Tencent Cloud RDS or TSpider + AWS RDS to deploy TenDB Cluster clusters.

## Contact us
For any questions or discussions about TenDB Cluster, please feel free to give us feedback via issues:     
TSpider:  https://github.com/Tencent/TenDBCluster-TSpider/issues   
TenDB:    https://github.com/Tencent/TenDBCluster-TenDB/issues   
Tdbctl:   https://github.com/Tencent/TenDBCluster-Tdbctl/issues 


## Roadmap
For details, see [chapter Roadmap](Documentation/roadmap-en.md)


## License
TenDBCluster is licensed under the GNU General Public License Version 2, except for the third-party components listed below. Copyright and license information can be found in the file TenDBCluster-License.


## Acknowledgements
Thanks to MariaDB and Percona for their outstanding contributions to the MySQL open source ecosystem, it was on the shoulders of the predecessors that TenDB Cluster was born.    
In addition, special thanks to the outstanding developers, contributors. For details, see [chapter acknowledgements](Documentation/acknowledgements-en.md)