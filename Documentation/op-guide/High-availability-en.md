#TenDB Cluster high availability
When the database is running, it is inevitable that it will encounter hardware, software, network and other failures, affecting the quality of service of the database. At this time, high availability of the database is very important.
The high availability of the cluster is mainly divided into fault detection and failover.
On the one hand, we must have the ability to quickly find faults when the cluster nodes are abnormal; at the same time, after sensing the faults, we can quickly achieve failover and minimize the impact of service availability


[Proxy Layer HA](TSpider-failover-en.md)

[Data Layer HA](TenDB-failover-en.md)

[Tdbctl HA](Tdbctl-failover-en.md)