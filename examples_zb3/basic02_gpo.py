# MicroPython XBee3 ZigBee
# coding: utf-8
'''
XBeeのLEDを点滅させてみる：IOポート4に接続したLEDを点滅

                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time
pinout={1:'D1', 2:'D2', 3:'D3', 4:'D4', 10:'P0', 11:'P1', 12:'P1'}

def gpo(port,val):
	ret=False
	out=0x00
	if isinstance(val, bool):
		if val == False:
			out=0x04
		else:
			out=0x05
	if type(val) is int:
		if val == 0:
			out=0x04
		else:
			out=0x05
	if out == 0x00 or port not in pinout:
		print('usage: gpo port =',pinout)
		print('            val = {0,1,False,True}')
		return 'ERROR'
	ret=xbee.atcmd(pinout[port],out)
	return {'port':port, 'out':out-4, 'return':ret }

while True:
	gpo(4,1)
	time.sleep_ms(1000)
	gpo(4,0)
	time.sleep_ms(1000)
