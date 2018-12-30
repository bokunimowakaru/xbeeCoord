# MicroPython XBee3 ZigBee
# coding: utf-8
'''
内蔵温度センサと電源電圧の値を読みとってブロードキャスト送信する
                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time

TEMP_OFFSET = 14.0                          # 内部温度上昇
dev_bc = '\x00\x00\x00\x00\x00\x00\xFF\xFF' # XBee ブロードキャストアドレス

def getTemp():                              # getTemp関数を定義する
    temp = xbee.atcmd('TP') - TEMP_OFFSET   # XBeeモジュールから温度値を取得する
    volt = xbee.atcmd('%V') / 1000          # XBeeモジュールから電圧値を取得する
    return {'temp':temp, 'volt':volt}       # 取得結果を応答する

while True:
    status = xbee.atcmd('AI')               # ネットワーク参加状態を確認する
    print('.',end='')
    if status == 0x00:                      # 参加状態の時にループを抜ける
        break
    time.sleep_ms(500)                      # 500msの待ち時間処理
print('\nJoined')

while True:
    sensor = getTemp()                      # センサ値を取得
    payload = str(sensor['temp']) + ',' + str(sensor['volt'])
    print('payload:',payload)
    xbee.transmit(dev_bc, payload)          # 取得値をXBeeで送信する
    time.sleep_ms(3000)                     # 3秒間の待ち時間処理
