rm -f build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/subsys/console/getchar
mkdir -p builds
cp build/zephyr/zephyr.bin builds/zephyr-getchar.bin

