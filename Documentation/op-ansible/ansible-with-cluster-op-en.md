# Common Operations and Maintenance Using Ansible

## Modify TenDB Configuration File

> Do not modify the *my.cnf* configuration file directly on the target TenDB host, because running playbooks will generate a new *my.cnf* file automatically and cause the modification to be overwritten.
> Dynamic modification using `set global xxx=xx` is not yet implemented

In *group_vars/tendb*, you can modify maximum connections allowed by TenDB:
```
mycnf_mysqld:
  innodb_flush_log_at_trx_commit: 0
  character-set-server: utf8mb4
  max_connections: 5000
```

Generating a new *my.cnf* file;
```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2 update_config_tendb.yml

# By default, modifying configuration does not reboot instances; slaves will reboot if "restart=true" is specified.
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2 update_config_tendb.yml -e "restart=true"
```

## Restart TenDB Nodes

```
# stop
ansible-playbook -i hosts.tendbcluster -l tendb-spt1,tendb-spt1-1 stop_tendbcluster.yml

# start
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-3 start_tendbcluster.yml

# restart
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-3 restart_tendbcluster.yml

# restart all tendb instance
ansible-playbook -i hosts.tendbcluster -l tendb restart_tendbcluster.yml
```

Also you can operate TSpider or Tdbctl nodes like above.

## Rebuild TenDB Slaves

- Rebuilding a slave requires that the target slave is not already running.
- Currently TenDB masters are backuped using `mysqldump --single-transaction`.
- Transfering backup files is run on Tdbctl nodes vir `scp -3` that means network traffic will pass through Tdbctl nodes.
- You can write your own playbook for building slaves.

```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2,tendb-spt1-3 init_common.yml
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-2,tendb-spt1-3 build_slave.yml
```

## Manually Perform TenDB Master/slave Failover

```
ansible-playbook -i hosts.tendbcluster -l tendb-spt3-2,tendb-spt3 -e "master_tgt=tendb-spt3-2" switch_master_slave.yml
```

The playbook verifies whether specified hosts belong to the same shard; if not, errors will be reported.

For now, the failover implementation of this playbook is simplified and it does not verify data integrity.

You can implement this playbook yourself to ensure data integrity. After failover is completed, routing information need be flushed (see ansible role `switch_master_slave`)

Once switching is completed, remember to manually update the role/master information in the inventory, so as to avoid routing information in Tdbctl being incoherent.

## Change Master-Slave Relationships

Eg., change the master of `tendb-spt1-3` to `tendb-spt1-2`:
```
ansible-playbook -i hosts.tendbcluster -l tendb-spt1-3 -e "master_tgt=tendb-spt1-2" tendb_change_master.yml 
```

After the change, make sure to update the role/master information in the inventory to ensure consistency.

## Synchronize TSpider Table Schema

Eg., import table schema of the first node in the `tspider` group to `tdbctl-node-03`:
```
ansible-playbook -i hosts.tendbcluster -l tdbctl-node-03 sync_tspider_schema.yml
```

## Flush Tdbctl Routing

The `mysql.servers` table in Tdbctl is updated in accordance with the inventory. This task is approached using `REPLACE INTO`. Make sure the `role` and `master` are set correctly in the inventory.
```
ansible-playbook -i hosts.tendbcluster -l tdbctl update_config_tdbctl.yml
```

## TenDB Failover

For now, a TenDB slave breakdown does not need to be particularly handled (slaves do not offer read service yet).

When a TenDB master undergoes a breakdown, a third-party mechanism is needed to perform a master/slave failover. After its slave is switched to the new master, the routing in Tdbctl need be flushed. The next time the playbook is run, since one of the master/slave roles has changed, if information in the inventory is inconsistent with the Tdbctl routes, the execution will be forced to abort.

Once the original master is fixed and started, you can run `change_master.yml` to make it as the new slave.

## Start TenDBCluster

Start all instances of TenDBCluster (TenDb, TSpider and Tdbctl):
```
ansible-playbook -i hosts.tendbcluster start_tendbcluster.yml
```

Or you can restart the whole TenDBCluster：
```
ansible-playbook -i hosts.tendbcluster restart_tendbcluster.yml
```

## Stop TenDBCluster
Stop all instances of TenDBCluster (TenDb, TSpider and Tdbctl):
```
ansible-playbook -i hosts.tendbcluster stop_tendbcluster.yml
```

## Destroy TenDBCluster
You can destroy the cluster only when it is stopped, and tendbcluster admin password will be prompted to proceed:
```
ansible-playbook -i hosts.tendbcluster destroy_tendbcluster.yml
```