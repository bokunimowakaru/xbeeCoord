/***************************************************************************************
XBee Wi-Fi搭載の照度センサから照度値を取得する

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

#define FORCE_INTERVAL  2                           // データ要求間隔(秒)

void setup(){
    lcd.begin(16, 2);
    lcd.print("Example 4 MySens");                  // タイトル文字を表示
    xbee_init( 0x00 );                              // XBee用Ethenet UDPポートの初期化
    xbee_myaddress(dev_my);                         // 自分のアドレスを取得する
    lcd.setCursor(0,1);
    lcd.print("ping...");
    while( xbee_ping(dev) ) delay(3000);            // XBee Wi-Fiから応答があるまで待機
    xbee_ratd_myaddress(dev);                       // 子機にPCのアドレスを設定する
    xbee_gpio_config( dev, 1 , AIN );               // XBee子機のポート1をアナログ入力へ
    xbee_end_device(dev,28,0,0);                    // XBee Wi-Fiモジュールを省電力へ
    lcd.clear();
    lcd.setCursor(0,1);
    for(byte i=0;i<4;i++){
        lcd.print(dev_my[i]);                       // 自分のアドレスを表示する
        if(i<3)lcd.print('.');
    }
    trig=0;
}

void loop(){
    float value;                                    // 受信データの代入用
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)
    unsigned long time=millis()/1000;               // 時間[秒]の取得(約50日まで)

    if( time >= trig && time > FORCE_INTERVAL){     // 変数trigまで時刻が進んだとき
        xbee_force(dev);                            // 状態取得指示を送信
        trig = time + FORCE_INTERVAL;               // 次回の変数trigを設定
    }
    if( time <= FORCE_INTERVAL ) trig = time + FORCE_INTERVAL;

    xbee_rx_call( &xbee_result );                   // データを受信
    if( xbee_result.MODE == MODE_RESP){             // 子機XBeeからのIOデータの受信時
        value = (float)xbee_result.ADCIN[1] * 7.4;  // 照度の測定結果をvalueに代入
        lcd.home();
        lcd.print("Time = ");
        lcd.print(time);                            // 変数timeの値を表示
        lcd.print(" [sec]  ");
        lcd.setCursor(0,1);
        lcd.print("Illum= ");
        lcd.print(value,0);                         // 変数valueの値を表示
        lcd.print(" [lux]  ");
    }
    if( lcd.readButtons()==BUTTON_DOWN ){           // 下ボタンが押された時
        xbee_end_device(dev,0,0,0);                 // XBee Wi-Fiの省電力モードを解除
        lcd.clear();
        lcd.print("done");
        while(lcd.readButtons()!=BUTTON_UP);        // 上ボタンが押されるまで停止
        xbee_end_device(dev,28,0,0);                // XBee Wi-Fiモジュールを省電力へ
    }
}
