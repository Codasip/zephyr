rm -f build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/codasip/hello_world_interrupt
mkdir -p builds
cp build/zephyr/zephyr.bin builds/zephyr-hello_world_interrupt.bin

