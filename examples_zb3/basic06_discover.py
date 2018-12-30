# MicroPython XBee3 ZigBee
# coding: utf-8
'''
discover命令を使ってネットワーク上のデバイスを発見したときに表示する
                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time
import time
import binascii

next_time_ms = 0
dev_types = ['coordinator', 'router', 'end device']
devs=[]

def discover():
    if time.ticks_ms() > next_time_ms:
        disc_devs = xbee.discover()
        for disc_dev in disc_devs:
            dev_addr = str(binascii.hexlify(disc_dev['sender_eui64']).decode('utf-8'))
            dev_type = dev_types[ disc_dev['node_type'] ]
            if dev_addr not in devs:
                devs.append(dev_addr)
                print('found',dev_addr, dev_type)   # 発見したデバイスを表示する
        next_time = time.ticks_ms() + 5000

while True:
    status = xbee.atcmd('AI')               # ネットワーク参加状態を確認する
    print('.',end='')
    if status == 0x00:                      # 参加状態の時にループを抜ける
        break
    time.sleep_ms(500)                      # 500msの待ち時間処理
print('\nJoined')

while True:
    discover()
    packet = xbee.receive()                 # パケットの受信を行う
    if packet:                              # 受信データがある時
        dev_rx = str(binascii.hexlify(packet['sender_eui64']).decode('utf-8'))
        dev_rx = dev_rx[:8] + ' ' + dev_rx[8:]              # アドレス(8+8文字)を保持
        payload = str(packet['payload'].decode('utf-8'))    # 受信データを保持
        print(dev_rx + ', ' + payload)      # アドレスと受信データを表示する
