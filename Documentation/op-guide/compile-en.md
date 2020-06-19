# Source Compile 
This chapter describes how to compile from source code.

## Compile TSpider
Download TSpider's latest version source code from GitHub.

### Build Requirements
- Mainstream linux distribution, CentOS etc.
- gcc 3.4.6+
- cmake 3.75+
- bison
- zlib-dev

#### Compile
Example to compile TSpider version 3.5
```bash
#cd source code directory
#make bld direcotry
mkdir bld && cd bld
cmake .. -DMYSQL_SERVER_SUFFIX=-tspider-3.5 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr/local/mysql -DBUILD_CONFIG=mysql_release -DWITH_EMBEDDED_SERVER=OFF -DWITH_JEMALLOC=no -DWITH_SSL=no -DWITHOUT_MROONGA_STORAGE_ENGINE=1 -DWITHOUT_TOKUDB_STORAGE_ENGINE=1 -DWITHOUT_MARIA_STORAGE_ENGINE=1 -DWITH_ARIA_STORAGE_ENGINE=0 -DWITH_NUMA=no -DWITH_WSREP=0
#compile and make package
nohup make -j > mk.log 2>&1  && make package > mp.log 2>&1 &
```

#### cmake aruguments
- MYSQL_SERVER_SUFFIX
TSider version suffix information.

- CMAKE_INSTALL_PREFIX
package to install, default to `/usr/local`.

## TenDB Compile
Download TenDB's latest version source code from GitHub.

### Build Requirements
- Mainstream linux distribution, for more information see [OS Requirements](https://www.mysql.com/support/supportedplatforms/database.html).
- CMAKE 2.8.2+
- GCC 4.4+
- bison
- boost 1.59
- jemalloc

### Compile
```bash
#cd into source code directory
mkdir bld
cd bld
export debug_flag=" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_CONFIG=mysql_release "
export gccdir=/usr/local/gcc-4.7.3
# configure
cmake .. -DDOWNLOAD_BOOST=1 -DWITHOUT_TOKUDB_STORAGE_ENGINE=1 -DMYSQL_SERVER_SUFFIX=$suffix $debug_flag -DFEATURE_SET=community -DWITH_EMBEDDED_SERVER=OFF -DCMAKE_C_COMPILER=$gccdir/bin/gcc -DCMAKE_CXX_COMPILER=$gccdir/bin/g++ -DCMAKE_INSTALL_PREFIX=/usr/local/mysql -DCMAKE_CXX_FLAGS="-static-libgcc -static-libstdc++" -DCMAKE_C_FLAGS="-static-libgcc" -DWITH_QUERY_RESPONSE_TIME=on
#compile and make package
nohup make -j > mk.log 2>&1  && make package > mp.log 2>&1 &
```

## Tdbctl Compile
Download Tdbctl's latest version source code from GitHub.

### Build Requirements
- Mainstream linux distribution, for more information see [OS Requirements](https://www.mysql.com/support/supportedplatforms/database.html).
- CMAKE 2.8.2+
- GCC 4.9.2+
- bison
- boost 1.59
- jemalloc

### Compile 
Example to compile Tdbctl 1.4 with gcc 5.5
```bash
#cd into source code directory
mkdir bld
cd bld
export gccdir=/usr/local/gcc-5.5.0
# configure
cmake .. -DDOWNLOAD_BOOST=1 -DMYSQL_SERVER_SUFFIX=1.4 -DCMAKE_INSTALL_PREFIX=/usr/local/mysql  -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_CONFIG=mysql_release -DCMAKE_C_COMPILER=$gccdir/bin/gcc -DCMAKE_CXX_COMPILER=$gccdir/bin/g++ -DCMAKE_CXX_FLAGS="-static-libgcc -static-libstdc++" -DCMAKE_C_FLAGS="-static-libgcc" -DFEATURE_SET=community -DWITH_EMBEDDED_SERVER=OFF  -DWITH_QUERY_RESPONSE_TIME=on
#compile and make package
nohup make -j > mk.log 2>&1  && make package > mp.log 2>&1 &
```