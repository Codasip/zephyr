rm -f build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/philosophers
mkdir -p builds
cp build/zephyr/zephyr.bin builds/zephyr-philosophers.bin

