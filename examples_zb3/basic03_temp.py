# MicroPython XBee3 ZigBee
# coding: utf-8
'''
内蔵温度センサと電源電圧の値を読み取る
                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time
TEMP_OFFSET=14.0			# 内部温度上昇

def temp():
	temp = xbee.atcmd('TP') - TEMP_OFFSET
	volt = xbee.atcmd('%V') / 1000
	return {'temp':temp, 'volt':volt}

while True:
	temp()
	time.sleep_ms(3000)
