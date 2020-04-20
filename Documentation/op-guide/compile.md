# 源码编译
本章节介绍如何使用源码来编译TenDB, TSpier, Tdbctl节点

## TSpider编译
下载最新版本TSpider源码  

### 环境要求
- 主流Linux系统，CentOS等
- gcc 3.4.6+
- cmake 3.75+
- bison
- zlib-dev
- jemalloc

#### 编译
下面以3.4.4版本示例
```bash
#cd进入源码仓库，解压子仓库
unzip libmariadb.zip
#创建编译目录
mkdir bld && cd bld
cmake .. -DMYSQL_SERVER_SUFFIX=-tspider-3.4.4 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr/local/mysql -DBUILD_CONFIG=mysql_release -DWITH_EMBEDDED_SERVER=OFF -DWITH_JEMALLOC=no -DWITH_SSL=no -DWITHOUT_MROONGA_STORAGE_ENGINE=1 -DWITHOUT_TOKUDB_STORAGE_ENGINE=1 -DWITHOUT_MARIA_STORAGE_ENGINE=1 -DWITH_ARIA_STORAGE_ENGINE=0 -DWITH_NUMA=no -DWITH_WSREP=0
#编译打包
nohup make -j > mk.log 2>&1  && make package > mp.log 2>&1 &
```

cmake相关参数说明
- MYSQL_SERVER_SUFFIX
TSider版本后缀信息
- CMAKE_INSTALL_PREFIX
编译包安装位置

## TenDB编译
下载最新TenDB版本

### 环境要求
- 主流Linux系统，具体见[操作系统要求](https://www.mysql.com/support/supportedplatforms/database.html)
- CMAKE 2.8.2+
- GCC 4.4+
- bison
- boost 1.59
- jemalloc

### 编译
```bash
# 进入源码目录
mkdir bld
cd bld
export debug_flag=" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_CONFIG=mysql_release "
export gccdir=/usr/local/gcc-4.7.3
# configure
cmake .. -DDOWNLOAD_BOOST=1 -DWITHOUT_TOKUDB_STORAGE_ENGINE=1 -DMYSQL_SERVER_SUFFIX=$suffix $debug_flag -DFEATURE_SET=community -DWITH_EMBEDDED_SERVER=OFF -DCMAKE_C_COMPILER=$gccdir/bin/gcc -DCMAKE_CXX_COMPILER=$gccdir/bin/g++ -DCMAKE_INSTALL_PREFIX=/usr/local/mysql -DCMAKE_CXX_FLAGS="-static-libgcc -static-libstdc++" -DCMAKE_C_FLAGS="-static-libgcc" -DWITH_QUERY_RESPONSE_TIME=on
#编译，打包
nohup make -j > mk.log 2>&1  && make package > mp.log 2>&1 &
```

## Tdbctl编译
下载最新Tdbctl版本

### 环境要求
- 主流Linux系统，具体见[操作系统要求](https://www.mysql.com/support/supportedplatforms/database.html)
- CMAKE 2.8.2+
- GCC 4.9.2+
- bison
- boost 1.59
- jemalloc

### 编译
下面以tdbctl-1.4版本，gcc 5.5版本为例
```bash
# 进入源码目录
mkdir bld
cd bld
export gccdir=/usr/local/gcc-5.5.0
# configure
cmake .. -DDOWNLOAD_BOOST=1 -DMYSQL_SERVER_SUFFIX=1.4 -DCMAKE_INSTALL_PREFIX=/usr/local/mysql  -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_CONFIG=mysql_release -DCMAKE_C_COMPILER=$gccdir/bin/gcc -DCMAKE_CXX_COMPILER=$gccdir/bin/g++ -DCMAKE_CXX_FLAGS="-static-libgcc -static-libstdc++" -DCMAKE_C_FLAGS="-static-libgcc" -DFEATURE_SET=community -DWITH_EMBEDDED_SERVER=OFF  -DWITH_QUERY_RESPONSE_TIME=on
#编译，打包
nohup make -j > mk.log 2>&1  && make package > mp.log 2>&1 &
```