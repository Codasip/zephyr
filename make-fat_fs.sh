rm build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/codasip/fat_fs -- "-DEXTRA_CONF_FILE=boards/codasip_l31helium.conf"
cp build/zephyr/zephyr.bin builds/zephyr-fat_fs.bin

