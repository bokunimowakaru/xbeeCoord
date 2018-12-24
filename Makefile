CC = gcc -Wall -O1
PROGS = examples/sample01_led \
		examples/sample02_sw \
		examples/sample03_add \
		examples/sample04_sens \
		examples/sample05_plug \
		examples/sample06_myplug \
		examples/sample07_uart \
		examples/sample08_html \
		examples/sample09_myplug2 \
		examples/sample10_mysens \
		examples/sample12_i2c_sht \
		examples/sample13_i2c_lcd \
		examples/sample14_i2c_tag \
		examples/sample15_voice \
		examples/sample16_hum \
		examples/sample18_httpd \
		examples/sample60_ir \
		examples/PocketGeiger_xbee \
		examples/PocketGeiger_xbee_fet \
		tools/xbee_test \
		tools/xbee_at_term \
		tools/xbee_com \
		tools/xbee_atcb04 \
		tools/xbee_atee_off \
		tools/xbee_zb_mode \
		tools/xbee_aging \
		examples_lite/sample01_led \
		examples_lite/sample02_sw \
		examples_lite/sample04_sens \
		examples_wifi/sample1_led \
		examples_wifi/sample2_sw_r \
		examples_wifi/sample3_sw_f \
		examples_wifi/sample4_uart

all: $(PROGS)
		gcc -Wall -O1 -c libs/xbee.c -o libs/xbee.o
		# ========================================
		# XBeeCoord examples for PC (Cygwin)
		#                         by Wataru KUNINO
		# ========================================

clean:
		rm -f $(PROGS) libs/xbee.o
