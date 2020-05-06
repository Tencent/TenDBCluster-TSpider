#!/bin/sh

usage(){
    echo -e "Usage: $0 [OPTIONS]"
    echo -e "compile MySQL Server source code"
    echo -e ""
    echo -e " -H --help        display help info ."
    echo -e " -t --tar         make package to tar.gz file"
    echo -e " -v --version     mysql server version"
    echo -e " -i --install     do   install"
    echo -e " --test           do mysql-test-run"
    echo -e " --debug          compile with debug info"
    echo -e "--------------------------------------------------------"
    echo -e "  version         default 3.5"
    echo -e "  debug           default to false"
    echo -e ""

    exit 1
}

do_install=0
debug=0
do_tar=0
version=3.5
do_test=0
debug_flag=" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_CONFIG=mysql_release "
bld_dir="bld"
static_flag=" -DCMAKE_CXX_FLAGS=-static-libstdc++ -DCMAKE_C_FLAGS=-static-libgcc " 
install_dir=/usr/local/mysql

TEMP=`getopt -o b:d:hitv: --long debug,test,help,install,tar,version:,verion: \
        -n "Try $0 --help for more information" -- "$@"`

if [ $? != 0 ]
then
    echo "script abnormal exit"
    exit 1
fi

if [ $# -eq 0 ]
then
    usage
fi

eval set -- "$TEMP"

while true
do
    case $1 in
	-h|--help)	usage; shift;;
	-i|--install)	do_install=1; shift;;
	-t|--tar)	do_tar=1; shift;;
	-v|--version)	version=$2; shift 2;;
	--debug)	debug=1; shift;;
	--test)		do_test=1; shift;;
	--) shift ; break;;
	*) usage;
	esac
done

suffix="-tspider-$version"

if [ $debug == 1 ]
then
    suffix="$suffix-debug"
    debug_flag=" -DCMAKE_BUILD_TYPE=Debug -DWITH_DEBUG=ON "
    bld_dir="bld_debug"
    #static_flag=""
fi


mkdir -p $bld_dir
cd $bld_dir
rm -f CMakeCache.txt

cmd="cmake .. -DMYSQL_SERVER_SUFFIX=$suffix -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr/local/mysql $debug_flag -DWITH_EMBEDDED_SERVER=OFF -DWITH_JEMALLOC=no -DWITH_SSL=no -DWITHOUT_MROONGA_STORAGE_ENGINE=1 -DWITHOUT_TOKUDB_STORAGE_ENGINE=1 -DWITHOUT_MARIA_STORAGE_ENGINE=1 -DWITH_ARIA_STORAGE_ENGINE=0 -DWITH_NUMA=no -DWITH_WSREP=0"
echo "compile args:"
echo "$cmd"
$cmd
make VERBOSE=1 -j `grep -c '^processor' /proc/cpuinfo`

if [ $do_install == 1 ]
then
    make install
fi

if [ $do_tar == 1 ]
then
    make package
fi

if [ $do_test == 1 ]
then
    cd mysql-test
    perl mysql-test-run.pl --max-test-fail=0 --force --parallel=24 &>test.log
    cd ..
fi

cd ..
