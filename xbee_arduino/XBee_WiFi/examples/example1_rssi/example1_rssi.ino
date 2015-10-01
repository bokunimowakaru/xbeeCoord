/***************************************************************************************
XBee Wi-FiのRSSI LEDとDIO4に接続したLEDを点滅

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <xbee_wifi.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// お手持ちのArduino EthernetのMACアドレスに変更する(区切りはカンマ)
byte mac[] = {0x90,0xA2,0xDA,0x00,0x00,0x00};

// お手持ちのXBeeモジュールのIPアドレスに変更する(区切りはカンマ)
byte dev[] = {192,168,0,135};

byte dev_my[4];                         // 親機ArduinoのIPアドレス代入用

void setup(){
    lcd.begin(16, 2);
    lcd.print("Example 1 RSSI ");       // タイトル文字を表示
    xbee_init( 0x00 );                  // XBee用Ethenet UDPポートの初期化
    xbee_myaddress(dev_my);             // 自分のアドレスを取得する
    lcd.setCursor(0,1);
    for(byte i=0;i<4;i++){
        lcd.print(dev_my[i]);           // 自分のアドレスを表示する
        if(i<3)lcd.print('.');
    }
}

void loop(){
    lcd.setCursor(0,0);
    if( xbee_ping(dev)==00 ){
        xbee_rat(dev,"ATP005");         // リモートATコマンドATP0(DIO10設定)=05(出力'H')
        xbee_rat(dev,"ATD405");         // リモートATコマンドATD4(DIO 4設定)=05(出力'H')
        lcd.print("Output High     ");
        delay( 2000 );                  // 約1000ms(1秒間)の待ち
        xbee_rat(dev,"ATP004");         // リモートATコマンドATP0(DIO10設定)=04(出力'L')
        xbee_rat(dev,"ATD404");         // リモートATコマンドATD4(DIO 4設定)=04(出力'L')
        lcd.setCursor(0,0);
        lcd.print("Output Low      ");
    }else{
        lcd.print("No Response     ");
    }
    delay( 2000 );                      // 約1000ms(1秒間)の待ち
}
