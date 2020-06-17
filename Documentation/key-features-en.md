# TenDB Cluster Key Features

## Transparent Database/Table Sharding  
By default, TenDB Cluster will distribute data horizontally to each TenDB node, and the logic of database and table sharding is completely transparent to the application layer. In addition, TenDB Cluster supports sharding algorithms such as hash, range, and list, and users can customize data sharding strategies to improve performance.   

## Spports the MySQL Protocol
The proxy layer TSpider of TenDB Cluster, as a distributed MySQL storage engine, supports any type of SQL in principle; and the application layer can access TenDB Cluster through MySQL API.


## Online Scaling
TenDB Cluster is capable of online scaling up and down without affecting cluster services. The topics that follow describe how to do online scaling for three components.

### **TSpider**
The proxy layer TSpider does not store data and is a stateless node. Therefore, if scale-up is needed, the only thing to do is to add more TSpider nodes with the same permissions and schema to the name service. Similarly, if scale-down is needed, you only need to remove some TSpider nodes from the name service.

### **TenDB**
The scaling of storage layer TenDB requires vertical scaling. First, use MySQL hot backup plan to migrate data from TenDB instances to more powerful machines. Second, synchronize data between old instances and new ones. Finally, after data synchronization is complete, refresh the routing information of Tdbctl. In this way, we can do scaling without affecting online services.

### **Tdbctl**
Tdbctl usually has no scaling requirements. If required, add nodes to the MGR cluster built by Tdbctl.


## High-availability
TenDB Cluster is a stable and reliable MySQL distributed database solution, so each node has redundancy and mutual backup to effectively ensure uninterrupted data processing when hardware or software fails. The following describes the high availability of three components.

### **TSpider**
When the software and hardware of a certain TSpider node fails, the node can be removed from the name service so that the business cannot access the node.
When the TSpider node was failed because of software or hardware , the node can be removed from the name service so that the business cannot access the node.


### **TenDB**
If the TenDB node uses master-slave deployment, wa can refresh the sharding rules information in Tdbctl to route the request to the slave TenDB node when a master TenDB node fails
If the TenDB node is deployed using MGR,  it will be automatically removed from the cluster when a TenDB node fails.

### **Tdbctl**
Tdbctl is an MGR cluster, and a node failure will be automatically eliminated from the cluster.   