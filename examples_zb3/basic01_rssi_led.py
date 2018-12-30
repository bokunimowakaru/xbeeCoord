# MicroPython XBee3 ZigBee
# coding: utf-8
'''
XBeeのLEDを点滅させてみる：RSSI LED（Port 10）を5回、点滅して終了する
（実行の途中で止めないでください。）

                                                  Copyright (c) 2018-2019 Wataru KUNINO
'''
import xbee
import sys, time

for i in range(1,5):                        # 繰り返し（5回）
    xbee.atcmd('P0',0x05)                   # RSSI用 LEDを点灯する
    time.sleep_ms(1000)                     # 1秒の待ち時間処理
    xbee.atcmd('P0',0x01)                   # RSSI用 LEDをRSSI表示に戻す(消灯)
    time.sleep_ms(1000)                     # 1秒間の待ち時間処理
