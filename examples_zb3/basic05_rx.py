# MicroPython XBee3 ZigBee
# coding: utf-8
'''
内蔵温度センサと電源電圧の値を読みとってブロードキャスト送信する
                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time
import binascii

while True:
    status = xbee.atcmd('AI')               # ネットワーク参加状態を確認する
    print('.',end='')
    if status == 0x00:                      # 参加状態の時にループを抜ける
        break
    time.sleep_ms(500)                      # 500msの待ち時間処理
print('\nJoined')

while True:
    packet = xbee.receive()                 # パケットの受信を行う
    if packet:                              # 受信データがある時
        dev_rx = str(binascii.hexlify(packet['sender_eui64']).decode('utf-8'))
        dev_rx = dev_rx[:8] + ' ' + dev_rx[8:]              # アドレス(8+8文字)を保持
        payload = str(packet['payload'].decode('utf-8'))    # 受信データを保持
        print(dev_rx + ', ' + payload)      # アドレスと受信データを表示する
