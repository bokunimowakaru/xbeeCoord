# MicroPython XBee3 ZigBee
# coding: utf-8
'''
XBeeのLEDを点滅させてみる：IOポート4に接続したLEDを点滅

                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time
pinout={1:'D1', 2:'D2', 3:'D3', 4:'D4', 10:'P0', 11:'P1', 12:'P1'}

def gpo(port,val):                          # getTemp関数を定義する
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

while True:                                 # 繰り返し処理
    gpo(4,1)                                # IO4をHigh出力に設定する
    time.sleep_ms(1000)                     # 1秒間の待ち時間処理
    gpo(4,0)                                # IO4をLow出力に設定する
    time.sleep_ms(1000)                     # 1秒間の待ち時間処理
