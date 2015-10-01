/***************************************************************************************
XBee Wi-Fiのスイッチ状態をリモートで取得しつつスイッチ変化通知でも取得する

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
byte dev_my[4];                                     // 親機ArduinoのIPアドレス代入用
unsigned long trig;                                 // 取得タイミング保持用

#define FORCE_INTERVAL  1                           // データ要求間隔(秒)

void setup(){
    lcd.begin(16, 2);
    lcd.print("Example 3 SW_F ");                   // タイトル文字を表示
    xbee_init( 0x00 );                              // XBee用Ethenet UDPポートの初期化
    xbee_myaddress(dev_my);                         // 自分のアドレスを取得する
    lcd.setCursor(0,1);
    lcd.print("ping...");
    while( xbee_ping(dev) ) delay(3000);            // XBee Wi-Fiから応答があるまで待機
    xbee_gpio_init(dev);                            // デバイスdevにIO設定を行う
    lcd.clear();
    lcd.setCursor(0,1);
    for(byte i=0;i<4;i++){
        lcd.print(dev_my[i]);                       // 自分のアドレスを表示する
        if(i<3)lcd.print('.');
    }
    trig=0;
}

void loop(){
    byte value;                                     // 受信値
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)
    unsigned long time=millis()/1000;               // 時間[秒]の取得(約50日まで)
    
    if( time >= trig && time > FORCE_INTERVAL){     // 変数trigまで時刻が進んだとき
        xbee_force(dev);                            // 状態取得指示を送信
        trig = time + FORCE_INTERVAL;               // 次回の変数trigを設定
    }
    if( time <= FORCE_INTERVAL ) trig = time + FORCE_INTERVAL;

    xbee_rx_call( &xbee_result );                   // データを受信
    if( xbee_result.MODE == MODE_RESP ||            // xbee_forceに対する応答
        xbee_result.MODE == MODE_GPIN){             // もしくは子機XBeeのDIO入力の時
        value = xbee_result.GPI.PORT.D1;            // D1ポートの値を変数valueに代入
        lcd.setCursor(0,0);
        lcd.print("Val=");
        lcd.print(value, BIN);                      // 変数valueの値を表示
        lcd.print(" Time=");
        lcd.print( time, DEC);                      // 変数timeの値を表示
        xbee_gpo(dev,4,value);                      // 子機XBeeのDIOポート4へ出力
    }
}
