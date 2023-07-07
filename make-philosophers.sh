rm build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/philosophers
cp build/zephyr/zephyr.bin builds/zephyr-philosophers.bin

