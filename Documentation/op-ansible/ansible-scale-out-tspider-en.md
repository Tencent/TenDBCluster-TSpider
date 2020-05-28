# Proxy Layer Scaling Using Ansible

## 1. TSpider Scaling Out

The proxy layer can be horizontally expanded by adding TSpider nodes. Below is a demonstration of how to add a TSpider node `tspider-node-03`:

Update the inventory:
```
#### SPIDER ####
[tspider]
tspider-node-01 ansible_host=192.168.1.106 mysql_port=25000
tspider-node-02 ansible_host=192.168.1.26 mysql_port=25000
tspider-node-03 ansible_host=192.168.1.2 mysql_port=25000
```

Run the playbook:
```
ansible-playbook -i hosts.tendbcluster -l tspider-node-03 add_tspider.yml
```

*add_tspider.yml* mainly does:
- Install, configure, grant and boot TSpider nodes;
- Synchronize table schema from the first TSpider node in the `tspider` group;
- Write to Tdbctl's routing table `mysql.servers` and flush routing information.

## 2. TSpider Scaling In

It is easy to remove a TSpider node:
- Delete the corresponding routing record and flush routing information;
- Stop the TSpider node.

Here is a example of removing `tspider-node-03`:
```
ansible-playbook -i hosts.tendbcluster -l tspider-node-03 del_tspider.yml
```

After the scale-in finish, remember removing the host information from ansible inventory.