rm build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/userspace/hello_world_user
cp build/zephyr/zephyr.bin builds/zephyr-hello_world_user.bin

