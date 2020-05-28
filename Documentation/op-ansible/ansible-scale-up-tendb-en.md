# Data Layer Scaling Using Ansible

# 1. TenDB Scaling Up
On the basis that TenDB has a constant number of shards, the purpose of scaling up TenDB is to migrate instances to hosts with larger capacities. Here is a demonstration of migrating a master-slave pair of SPT1 to new hosts.

Initial state:
```
+-----------------+
|   tendb-spt1    |
+-----------------+
         ^  Master
         |        
         |        
+-----------------+
|   tendb-spt1-1  |
+-----------------+
```

### 1.1 Add New Hosts to the Shard

```
#### SPT1 ####
[SPT1]
tendb-spt1 ansible_host=192.168.1.95 mysql_shard=SPT1 mysql_port=20001 role=master
tendb-spt1-1 ansible_host=192.168.1.147 mysql_shard=SPT1 mysql_port=20001 role=slave master=tendb-spt1

# the new tendb pair machines that have higher performance
tendb-spt1-2 ansible_host=192.168.1.6 mysql_shard=SPT1 role=slave master=tendb-spt1 innodb_buffer_pool_size_mb=4096 mysql_data_dir=/data1/mysqldata/{{mysql_port}}
tendb-spt1-3 ansible_host=192.168.1.210 mysql_shard=SPT1 role=slave master=tendb-spt1-2 innodb_buffer_pool_size_mb=4096 mysql_data_dir=/data1/mysqldata/{{mysql_port}}
```

Here we manually specify `innodb_buffer_pool_size_mb` and `mysql_data_dir`. If you expect the buffer pool size to be automatically calculated, the other instances that will be installed on the target host should be added in the inventory file, since auto calculation is based on the `tendb` group's `ansible_host` in the inventory.

Also note that, when joining the shard, the two new hosts have `role=slave` but point to different masters. Therefore, the master/slave switch between `tendb-spt1` and `tendb-spt1-2` later can be achieved in one step.

```
+-----------------+            +-----------------+
|   tendb-spt1    | <--------- |   tendb-spt1-2  |
+-----------------+            +-----------------+
         ^  Master                      ^         
         |                              |         
         |                              |         
+-----------------+            +-----------------+
|   tendb-spt1-1  |            |   tendb-spt1-3  |
+-----------------+            +-----------------+
```

### 1.2 Chaining Slaves

```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2,tendb-spt1-3 init_common.yml
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2,tendb-spt1-3 build_slave.yml
```

The `-l` option is to specify the new TenDB nodes for the operation.

### 1.3 Master/slave Switch

```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1,tendb-spt1-2 -e "master_tgt=tendb-spt1-2" switch_master_slave.yml
```

Use the `-l` to specify the two TenDB nodes to switch and set the `master_tgt` value to specify who's the master.

```
+-----------------+            +-----------------+         
|   tendb-spt1    | ---------> |   tendb-spt1-2  |         
+-----------------+            +-----------------+         
         ^                     Master   ^                  
         |                              |                  
         |                              |                  
         |                              |                  
+-----------------+            +-----------------+         
|   tendb-spt1-1  |            |   tendb-spt1-3  |         
+-----------------+            +-----------------+         
                                                    
```

After switching, the routing information in Tdbctl will be flushed at the same time.

### 1.4 Stop Old Instances

```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1,tendb-spt1-1 stop_tendbcluster.yml
```

If everything is working fine, stop the master/slave instances on the old hosts.

```
+-----------------+
|   tendb-spt1-2  |
+-----------------+
Master   ^         
         |         
         |         
+-----------------+
|   tendb-spt1-3  |
+-----------------+
```

> Important: In the inventory, delete information of the old instances and update `role` and `master`.

```
#### SPT1 ####
[SPT1]
tendb-spt1-2 ansible_host=192.168.1.6 mysql_shard=SPT1 role=master innodb_buffer_pool_size_mb=4096 mysql_data_dir=/data1/mysqldata/{{mysql_port}}
tendb-spt1-3 ansible_host=192.168.1.210 mysql_shard=SPT1 role=slave master=tendb-spt1-2 innodb_buffer_pool_size_mb=4096 mysql_data_dir=/data1/mysqldata/{{mysql_port}}
```

### 1.5 Modify Remaining Shards' Configuration (Optional)

When scaling up, we suggest the other shards on the original hosts be migrated to the corresponding new hosts, and therefore the old hosts can be "discarded". The advantage of doing so is that you **need not** modify the buffer pool configuration of the other shards to achieve scaling up.

If you want to keep the old hosts, then the remaining shards can be set to have larger buffer pools. Run the `update_config_tendb.yml` playbook to modify the remaining shards' buffer pools of SPT0:

```
ansible-playbook -i hosts.tendbcluster -l SPT0 update_config_tendb.yml

# By default, modifying configuration does not reboot instances; slaves will reboot if "restart=true" is specified.
ansible-playbook -i hosts.tendbcluster -l SPT0 update_config_tendb.yml -e "restart=true"
```

### TenDB Scaling Up in One Step

> We do not recommend using it in production.

We put the slave chaining and master/slave switch in one playbook so that scaling up can be achieved within one step. However, please note that the configuration in the inventory should be set up correctly beforehand, and that old hosts should be deleted and role/master should be updated.

```
ansible-playbook -i hosts.tendbcluster -l SPT1 -e "master_tgt=tendb-spt1-2" tendb_migrate.yml
```

## 2 TenDB Scaling Down

The procedures for scaling down TenDB are identical to the procedures of scaling up, except that the target hosts have a lower capacity and performance, and that evaluation is carried out to check whether disk space is enough for multiple instances to run on one host.