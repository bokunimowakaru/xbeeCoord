# MicroPython XBee3 ZigBee
# coding: utf-8
'''
内蔵温度センサと電源電圧の値を読み取る
                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time
TEMP_OFFSET=14.0                            # 内部温度上昇

def getTemp():                              # getTemp関数を定義する
    temp = xbee.atcmd('TP') - TEMP_OFFSET   # XBeeモジュールから温度値を取得する
    volt = xbee.atcmd('%V') / 1000          # XBeeモジュールから電圧値を取得する
    return {'temp':temp, 'volt':volt}       # 取得結果を応答する

while True:
    print(getTemp())                        # センサ値を取得
    time.sleep_ms(3000)                     # 3秒間の待ち時間処理
