# TSpider access layer high availability
The high availability of the TSpider access layer, including the rapid detection of faults and the rapid switching after abnormal services.

## Faults Detection  

<a id="jump1"></a>

There are generally two ways to find the fault. 

- Cluster Awareness

```
The abnormal trigger threshold is preset within the cluster. The cluster components or nodes continuously collect error information of statistical requests, and warn about abnormalities after triggering the threshold, triggering high availability.
Requires Tdbctl node, TSpider node has statistics collection function, currently not supported
```

- Peripheral Detection

```
Connect instances with peripheral components, periodically analyze the running information of the instance to determine availability
For example, ping operation, mysql connection to execute SQL, etc.
```

### Introduction to TenDB Cluster Detection
TenDB Cluster uses peripheral detection to detect node availability. According to the network distribution of the nodes, a co-city and a cross-city detection point will be deployed to periodically connect to the current node and perform a replace write operation. If the operation is successful, it will be reported to the central computer as normal, and if the operation fails or times out, it will be reported as abnormal; The machine periodically scans the detection results and triggers the switching behavior after detecting anomalies.

## Failover
TSpider access layer switching is relatively simple, mainly to remove the faulty TSpider node from load balancing, name service, application configuration, etc. And remove the abnormal node from the Tdbctl node. 

### Update name service
TSpider should update the name service or load balance service in time after an exception, remove the abnormal ip, and ensure that new connection requests are no longer resolved to abnormal nodes.

### Update Cluster Router

```sql
#Remove abnormal TSpider router from Tdbctl node
delete from mysql.servers where Host='$spider_ip' and Port='$spider_port';
#Refresh route by force
tdbclt flush routing force;
```
In the fault scenario, some abnormal tcp connections may sometimes affect the real-time performance of the switch before being destroyed normally. Therefore, Tdbctl provides a forced refresh route function, which can effectively avoid the interference of these abnormal connections.

## Fault reconstruction
After the failover is successful, compared with the original cluster, the load capacity of the access layer decreases due to the decrease of nodes. It is necessary to add new access layer nodes according to the current status of the cluster. For details, please refer to
 [Tspider-scale](TSpider-scale-en.md/#jump1)

## Other Instructions
When deploying a TenDB Cluster cluster, deploy multiple TSpider nodes as much as possible, and fully consider cross-machine, cross-machine room, and cross-region disaster tolerance, which can effectively avoid large batches of failures.
During the switchover, if only the last TSpider node is left in the TenDB Cluster cluster, it is necessary to carefully consider the removal operation to prevent the loss of the domain name and table structure, which makes it difficult to recover from the disaster
There are many failure scenarios that affect the access layer, such as network partitions, excessive machine load, machine hardware failure, and abnormal service of TSpider itself. Different failure scenarios may show different phenomena. Therefore, when detecting the availability of TSpider, these anomalies need to be fully considered, so that timely detection and rapid switching
In some failure scenarios, such as a network flash, after the high-availability switch is completed, there is no such abnormal node in the cluster's route (so the new DDL operation will not and cannot be synchronized); after the network flashes, it is quickly recovered , and the application connection maybe cache, reconnection, long connection mechanism  etc will cause requests to still connect to this node, resulting in data errors. However, this risk generally only occurs in the fast recovery scenario after the network is abnormal (usually in seconds, which is related to the time taken from detecting the abnormality to the completion of the switch), so this problem needs to be considered when designing the fault detection and switching. For example, after detecting an abnormal node to recover quickly, shut down its service in time.