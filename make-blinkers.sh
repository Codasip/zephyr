rm build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/codasip/blinkers
cp build/zephyr/zephyr.bin builds/zephyr-blinkers.bin

