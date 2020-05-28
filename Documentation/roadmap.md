# TenDB Cluster Roadmap

## 功能
- 全局唯一递增ID
- 多唯一键支持
- 支持存储实例使用KV存储
- 读写分离

## 性能
- TSpider性能优化--内存管理优化
- TSpider性能优化--连接分配优化
- TSpider性能优化--SQL改写优化

## 集群运营

### 利用Kubernetes Operator管理集群
实现TenDB Cluster Operator，旨在提供在主流云基础设施（Kubernetes）上部署管理 TenDB Cluster集群的能力。利用Kubernetes的容器编排能力和TenDB Cluster的集群管理能力，集成了一键部署、一键扩缩、故障切换、故障自愈等能力。

### 集成Prometheus监控系统
利用Prometheus对集群的数据存储、性能、异常进行监控。