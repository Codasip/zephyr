rm build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/subsys/console/getchar
cp build/zephyr/zephyr.bin builds/zephyr-getchar.bin

