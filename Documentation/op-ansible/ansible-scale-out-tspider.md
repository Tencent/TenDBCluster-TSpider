# Ansible 接入层扩缩容

## 1 TSpider扩容

TSpider 接入层节点可水平扩展，下面演示添加一个 TSpider node `tspider-node-03`:
更新好 inventory
```
#### SPIDER ####
[tspider]
tspider-node-01 ansible_host=192.168.1.106 mysql_port=25000
tspider-node-02 ansible_host=192.168.1.26 mysql_port=25000
tspider-node-03 ansible_host=192.168.1.2 mysql_port=25000
```

运行 playbook
```
ansible-playbook -i hosts.tendbcluster -l tspider-node-03 add_tspider.yml
```

_add_tspider.yml_ 主要内容：
- TSpider node 安装、配置、权限、启动
- 从 `tspider` 组的第一个 TSpider node 同步表结构
- 写入 Tdbctl 的路由表`mysql.servers`，刷新路由


## 2 TSpider缩容
删除 TSpider 节点相对简单：
- 从 Tdbctl 中删除路由记录，刷新路由
- 停止 TSpider 节点

演示删除 `tspider-node-03` :
```
ansible-playbook -i hosts.tendbcluster -l tspider-node-03 del_tspider.yml
```