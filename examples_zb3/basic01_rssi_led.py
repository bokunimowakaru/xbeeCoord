# MicroPython XBee3 ZigBee
# coding: utf-8
'''
XBeeのLEDを点滅させてみる：RSSI LED（Port 10）を5回、点滅して終了する
（実行の途中で止めないでください。）

                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time

for i in range(1,5):
	xbee.atcmd('P0',0x05)
	time.sleep_ms(1000)
	xbee.atcmd('P0',0x01)
	time.sleep_ms(1000)
