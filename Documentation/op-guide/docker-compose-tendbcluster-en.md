Quickly deploy TenDB Cluster using `docker-compose`.

> Using `docker-compose` for one-click deployment of TenDB Cluster on a single host is only suitable for test purposes, enabling you to obtain hands-on experience on TenDB Cluster without much effort. Please do not use it for production.

# Prerequisites

- `docker`: Version 17.x and later
- `docker-compose`: [download from GitHub](https://github.com/docker/compose/releases)
- `helm`: [download from GitHub](https://github.com/helm/helm/releases)
- Make sure you can successfully pull `tendb`, `tspider` and `tdbctl` docker images in your network environment.

In the repository, the *docker-compose.yml* file is a configuration of a TenDB Cluster with 4 shards. If you want to change the number of shards, use a `helm` template to generate the *docker-compose.yml* file.

```
docker: ce-19.03.1
helm: 2.16.7
docker-compose: 1.25.5
```

# Quick Deployment

1. **Clone the repo and pull the images**
```
$ cd tendbcluster-docker-compose && docker-compose pull
```

2. **Start in One Click**

Before starting, confirm your default configurations of usernames, passwords and data storage directories in the *docker-compose.yml* file.
- The ADMIN user is only granted local login.
- The OPERATOR user is for operations within the cluster, eg., performing DDL operations on other nodes using Tdbctl.
- The MySQL data directories are set as `/data/mysqldata/<port>` by default.

```
// Start TenDB Cluster
$ docker-compose up -d
```
After about a minute, you can run `docker ps` to check whether containers of TenDB, TSpider and Tdbctl all have a "healthy" status.

3. **Get Started**

Connect to any TSpider node with a MySQL client to test it out, for example:
```
// connect to the first TSpider node
mysql -utendbcluster -ptendbclusterpass -h127.0.0.1 -P25000
> select * from mysql.servers;
> create database dbtest; use dbtest;
> create table t(id int auto_increment primary key, c1 int);
> insert into t(c1) values(1),(1),(1),(1),(1),(1),(1),(1);

// connect to the second TSpider node, and check the schema auto-created
mysql -utendbcluster -ptendbclusterpass -h127.0.0.1 -P25001

// connect to the first Tendb node, and check the data
mysql -utendbcluster -ptendbclusterpass -h127.0.0.1 -P20000
> select * from dbtest_0.t;
```

If you want to stop or destroy TenDB Cluster:
```
$ cd tendbcluster-docker-compose
// Stop
$ docker-compose stop

// Restart
$ docker-compose  start

// Destroy
$ docker-compose down

// Manually remove data directories
```

# Customized Deployment

The example above demonstrates deployment of a TenDB Cluster with 4 TenDB nodes, 2 TSpider nodes and 1 Tdbctl node. If you want to try it out some other amount of shards (say 12 for example), we do not suggest directly modifying *docker-compose.yml*. Instead we suggest using `helm template` to generate *docker-compose.yml* and *tdbctl-routing-flush.sql*.

Example:
```
tendb:
  image: hub.oa.com/scc-dba/tendb:3.1.5-v0.3
  size: 12
  portStart: 20000
  dataDir: /data/mysqldata
  innodbBufferPoolSize: 1024M
```

## Modify *values.yaml* and Generate *docker-compose.yml*

If you want to modify data directories, image paths or image versions, modify *compose/values.yml* first and use `helm` to generate *docker-compose.yml*, so as to avoid possible errors.

```
// Generate "generated-docker-compose.yml"
helm template compose -x templates/docker-compose.yml > generated-docker-compose.yml

// Generate Tdbctl's routing information; it will mount on tdbctl-0 and automatically run.
helm template compose -x templates/tdbctl-routing-flush.sql.tpl > tdbctl-routing-flush.sql
```

## Start in One Click
```
$ docker-compose -f generated-docker-compose.yml up -d
```