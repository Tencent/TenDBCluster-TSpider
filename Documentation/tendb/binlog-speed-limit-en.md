# Binlog Speed Limiter
## Background  
In real application, we tend to use MySQL as one-master and multi-slave, or multi-master and multi-slave, to do disaster recovery. The most important part of these disaster recovery solutions is master-slave replication and incremental binlog synchronization.  

In most scenarios, we hope that the IO thread synchronizes the binlog to the Slave node as soon as possible when synchronization, so as to reduce the latency.

However, under some special circumstances, we hope to be able to control the speed of binlog synchronization: 
1. In one master, multi-slave scenario, with the linear growth of the slave, the master needs to synchronize binlog to different slave nodes, and the network overhead also increases linearly, which affects the speed of the corresponding request.
2. When a new slave is added (normally occurs when failover), we need to import fully prepared data, and then perform incremental synchronization. If the time difference between the backup time and the reconstruction slave is large or the binlog speed generation speed is too fast, the binlog would be too large for network to handle, which would affect the normal request of the application.

## How to Limit Speed
The variable `read_binlog_speed_limit` specifies the speed limit of reading binlog (KB/S). It can be used in session or globally. The default value is 0, meaning no speed limit.  

This variable is applied to Slave, which can be set dynamically, and it will take effect immediately after being set.

## How does it work
TenDB limit the speed by implementing the token bucket algorithm on the slave pull behavior:

The principle of the token bucket algorithm is roughly as follows:
1. Suppose there is a bucket with a capacity of `n`;
2. Every second, add `a` number of tokens;
3. When a packet arrives, it must get the same number of tokens from the bucket as the packet size before it can be released.

The speed is determined by the size of `a`, and the maximum packet size that can be released is determined by `n`. The original token bucket algorithm has a behavior of discarding packets, which is unacceptable. So we slightly modify the algorithm:

1. If an event has arrived, the overflowed tokens will not be discarded even if the bucket is full, so that data packet can always get enough tokens and then be released no matter how large it is;

2. In addition, the token bucket algorithm requires a fixed number of tokens to be added into the bucket per second, which is not easy to implement in the single-threaded model, so it is modified to add tokens before writing the relay log each time.

>There are some other MySQL branches that support binlog limit, whose principles are similar to TenDB's. However, those speed limit is applied to master. If use master speed limit, you do not need to set the speed limit function on each slave. But it is not flexible enough to solve the new slaves added scenario.