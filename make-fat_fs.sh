rm -f build/zephyr/zephyr.bin
# west build -p always -b codasip_l31helium samples/codasip/fat_fs -- "-DEXTRA_CONF_FILE=boards/codasip_l31helium.conf"
west build -p always -b codasip_l31helium samples/codasip/fat_fs
mkdir -p builds
cp build/zephyr/zephyr.bin builds/zephyr-fat_fs.bin

