Compilation steps:

cd TendbCluster3 && unzip libmariadb.zip
mkdir bld && cd bld
cmake .. -DMYSQL_SERVER_SUFFIX=-tspider-3.4.4 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr/local/mysql -DBUILD_CONFIG=mysql_release -DWITH_EMBEDDED_SERVER=OFF -DWITH_JEMALLOC=no -DWITH_SSL=no -DWITHOUT_MROONGA_STORAGE_ENGINE=1 -DWITHOUT_TOKUDB_STORAGE_ENGINE=1 -DWITHOUT_MARIA_STORAGE_ENGINE=1 -DWITH_ARIA_STORAGE_ENGINE=0 -DWITH_NUMA=no -DWITH_WSREP=0
nohup make -j > mk.log 2>&1  && make package > mp.log 2>&1 &
