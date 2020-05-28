# 使用 docker-compose 快速搭建 TenDB Cluster

> docker-compose 只适合在单机上一键部署 TenDB Cluster 测试集群，方便用户快速体验 TenDB Cluster。不要用在生成环境。


## 准备环境

- Docker: 版本17.x及以上
- docker-compose: 下载地址 https://github.com/docker/compose/releases
- helm: 下载地址 https://github.com/helm/helm/releases
- 网络能够正常拉取 `tendb`, `tspider`, `tdbctl` 的镜像

仓库下 docker-compose.yml 默认是部署 4 个分片的 TenDB Cluster，如果想部署其它分片数，才需要使用 helm 根据模板来生成 docker-compose.yml。

```
docker: ce-19.03.1
helm: 2.16.7
docker-compose: 1.25.5
```

## 快速部署

1. **克隆仓库，拉取镜像**
```
$ cd tendbcluster-docker-compose && docker-compose pull
```

2. **一键启动**
启动前确认下 docker-compose.yml 里面的默认用户名、密码、数据存放目录
- ADMIN 用户只授权本地登录
- OPERATOR 用户用于集群内部操作，比如 Tdbctl 去其它节点做 DDL 操作
- MySQL 数据目录默认 `/data/mysqldata/<port>`

```
// 启动
$ docker-compose up -d
```
大约1分钟后，通过 `docker ps` 查看 TenDB, TSpider, Tdbctl 三个组件的容器是否是 healthy .

3. **体验**
使用 mysql 客户端连接上任意 TSpider 节点。比如我在 Docker host 本地：
```
// connect to the first TSpider node
mysql -utendbcluster -ptendbclusterpass -h127.0.0.1 -P25000
> select * from mysql.servers;
> create database dbtest; use dbtest;
> create table t(id int auto_increment primary key, c1 int);
> show create table t\G
> insert into t(c1) values(1),(1),(1),(1),(1),(1),(1),(1);

// connect to the second TSpider node, and check the schema auto-created
> mysql -utendbcluster -ptendbclusterpass -h127.0.0.1 -P25001
> show create table dbtest.t\G

// connect to the first Tendb node, and check the sharded data
mysql -utendbcluster -ptendbclusterpass -h127.0.0.1 -P20000
> select * from dbtest_0.t;
```

如果需要停止或者销毁：
```
$ cd tendbcluster-docker-compose
// 停止
$ docker-compose stop

// 重新启动
$ docker-compose  start

// 销毁
$ docker-compose down
// 手动删除数据目录
```

## 自定义部署
以上默认部署的是 4节点 TenDB，2节点 TSpider，1节点 Tdbctl，如果想体验其它数量分片如12节点 TenDB，不建议直接修改 docker-compose.yml，而是使用 helm template 来生成 `docker-compose.yml` 和 `tdbctl-routing-flush.sql`。

例：
```
tendb:
  image: hub.oa.com/scc-dba/tendb:3.1.5-v0.3
  size: 12
  portStart: 20000
  dataDir: /data/mysqldata
  innodbBufferPoolSize: 1024M
```

### 修改 values.yaml，生成 docker-compose.yml

包括如果想修改数据目录、镜像路径、镜像版本，都通过修改 `compose/values.yaml` 后再 helm 来生成，避免出错。

```
// 生成 generated-docker-compose.yml
helm template compose -x templates/docker-compose.yml > generated-docker-compose.yml

// 生成 Tdbctl 的路由信息，会挂载到 tdbctl-0 并且自动执行
helm template compose -x templates/tdbctl-routing-flush.sql.tpl > tdbctl-routing-flush.sql
```

## 一键启动
```
$ docker-compose -f generated-docker-compose.yml up -d
```