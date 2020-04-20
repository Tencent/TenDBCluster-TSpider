# TenDB Cluster Environment Configuration 
As an open source distributed relational database, TenDB Cluster can be well deployed and run on Intel architecture server environment and mainstream virtualization environment, and supports most mainstream hardware networks. As a high-performance database system, TenDB Cluster supports the mainstream Linux operating system environment.

## Operating System Requirements 

| Operating System | 	Architecture 
| :--- | :----
| RHEL, CentOS,Ubuntu |glibc 2.12+, x86_64, x86_32
| macOS 10.13+    | x86_64

## System Requirements
### Minimum Hardware Requirements
| Node | 	CPU|Memory|Disk|Instance Number
| :--- | :----|:----|:----|:----|:----
|TSpider|1 Core|2GB RAM|Disk I/O subsystem applicable to a write-intensive database|1
|Tdbctl|1 Core|1GB RAM|Disk I/O subsystem applicable to a write-intensive database|1
|TenDB|2 Cores|2GB RAM|Disk I/O subsystem applicable to a write-intensive database|1

### Recommended Hardware Requirements
| Node | 	CPU|Memory|Disk|Network|Instance Number
| :--- | :----|:----|:----|:----|:----
|TSpider|4 Cores or more|4GB or more|SAS|Gigabit Ethernet|2 or more
|Tdbctl|2 Cores or more|2GB or more|SAS|Gigabit Ethernet|3 or more
|TenDB|4 Cores or more|8GB or more|SSD, RAID10 or RAID 0+1|Gigabit Ethernet|12 or more

### Network Requirements
| Node| 	Default Port|Description
| :--- | :----|:----|
|TSpider|25000|TSpider listening port
|Tdbctl|26000|Tdbctl listening port
|TenDB|20000|TenDB listening port

