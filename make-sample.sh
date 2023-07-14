#/bin/bash -xe

if [ $# -lt 3 ]
then
	echo "$0 <core_name> <sample/dir> <app-name> [<new-name>]"
	echo E.g.: $0 codasip_l31carbon samples/codasip blinkers
	exit 1
else
	core=$1
	dir=$2
    app=$3
    name=$app
fi

if [ $# -eq 4 ]
then
    name=$4
fi

echo Building $dir/$app for core $core

rm -rf build
west build -p always -b ${core} $dir/$app
mkdir -p builds-$core
cp build/zephyr/zephyr.bin builds-$core/zephyr-$name.bin
cp build/zephyr/zephyr.elf builds-$core/zephyr-$name.elf
