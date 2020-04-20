# Tdbctl参数说明

中控节点Tdbctl是在一个精简TenDB上实现的，因此Tdbctl参数的配置方法和MySQL一致。通过SET指令或者改my.cnf配置文件重启mysqld可以修改配置。   
下面介绍Tdbctl作为中控节点的几个参数：   

**`TC_ADMIN`**  
作用域: SESSION|GLOBAL   
默认值: OFF     
值为OFF时中控节点会临时成为一个普通MySQL；值为ON时中控节点会将接受到的DDL分发到集群中TSpider节点和TenDB节点。
TSpider往中控节点分发DDL时会执行 *set tc_admin=on*

**`TC_FORCE_EXECUTE`**   
作用域: SESSION|GLOBAL   
默认值: ON   
值为ON表示中控节点在对TenDB节点分发DDL出错后，继续对TSpider分发DDL；否则在TenDB节点执行DDL出错后， 不再继续对TSpider节点分发DDL


**`TC_CHECK_REPAIR_ROUTING`**   
作用域: GLOBAL   
默认值: ON   
值为ON表示中控节点会自动检查TSpider节点中的路由配置和中控节点是否一致，如果不一致就修正到一致； 否则不检查路由配置一致性   

**`TC_CHECK_REPAIR_ROUTING_INTERVAL`**   
作用域: GLOBAL   
默认值: 300   
值为300表示检查集群中各TSpider节点路由信息一致性的时间间隔为300秒

**`TC_MYSQL_WRAPPER_PREFIX`**   
作用域: GLOBAL   
默认值: SPT   
中控节点在构建TSpider节点上执行的建表语句时，需要按一定分片顺序进行。本参数约束集群中存储节点的路由信息对应的Server_name的前缀。如果前缀为SPT，则存储节点的Server_name写法必须是SPT0、SPT1...SPTn，数字部分为从0开始的连续整数。

**`TC_SPIDER_WRAPPER_PREFIX`**  
作用域: GLOBAL   
默认值: SPIDER   
本参数约束集群中TSpider节点的路由信息对应的Server_name的前缀。


**`tc_set_changed_node_read_only`**   
作用域: GLOBAL   
默认值: OFF   
值为ON时表示在进行更新集群中的存储节点时，将存储节点设置为只读；否则，不设置为只读
