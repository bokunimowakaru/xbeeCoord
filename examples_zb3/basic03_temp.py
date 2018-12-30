# MicroPython XBee3 ZigBee
# coding: utf-8
'''
内蔵温度センサの値を読み取る
                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time
TEMP_OFFSET=15.0			# 内部温度上昇

def temp():
	ret=xbee.atcmd('TP') - TEMP_OFFSET
	return ret

while True:
	temp()
	time.sleep_ms(1000)
