rm build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/drivers/uart/echo_bot
mkdir -p builds
cp build/zephyr/zephyr.bin builds/zephyr-echo_bot-rx_interrupt.bin

