# TenDB Cluster Roadmap

## Features
- Globally unique incremental ID
- Multiple unique keys support
- Support storage instance using KV storage
- Separation of read and write

## Performance
- TSpider performance optimization - memory management optimization
- TSpider performance optimization - connection allocation optimization
- TSpider performance optimization - SQL rewrite optimization


## TenDB Cluster Operation
### Using Kubernetes Operator to Manage Clusters
The implementation of TenDB Cluster Operator aims to provide the ability to deploy and manage TenDB Cluster clusters on mainstream cloud infrastructure (Kubernetes). Leveraging Kubernetes' container orchestration capabilities and TenDB Cluster's cluster management capabilities, it integrates one-click deployment, one-click scaling, failover, and fault self-healing capabilities.

### Integrated Prometheus
Use Prometheus to monitor the cluster's data storage, performance, and anomalies.