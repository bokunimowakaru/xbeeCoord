# MicroPython XBee3 ZigBee
# coding: utf-8
'''
内蔵温度センサと電源電圧の値を読みとって送信する
                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time

TEMP_OFFSET=14.0							# 内部温度上昇
DEV_NAME='xbtmp_1,'							# 識別用デバイス名
dev_bc='\x00\x00\x00\x00\x00\x00\xFF\xFF'	# XBee ブロードキャストアドレス

def temp():
	temp = xbee.atcmd('TP') - TEMP_OFFSET
	volt = xbee.atcmd('%V') / 1000
	return {'temp':temp, 'volt':volt}


while True:
	sensor = temp()
	payload = DEV_NAME + str(sensor['temp']) + ',' + str(sensor['volt'])
	xbee.transmit(dev_bc, payload)
	print('send :',payload)
	time.sleep_ms(3000)
