/***************************************************************************************
子機XBee Wi-FiのUARTからのシリアル情報を受信する

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

void setup() {
    lcd.begin(16, 2);                               // LCDのサイズを16文字×2桁に設定
    lcd.print("Example 5 UART ");                   // タイトル文字を表示
    xbee_init( 0 );                                 // XBee用COMポートの初期化
    xbee_myaddress(dev_my);                         // 自分のアドレスを取得する
    lcd.setCursor(0,1);
    lcd.print("ping...");
    while( xbee_ping(dev) ) delay(3000);            // XBee Wi-Fiから応答があるまで待機
    xbee_ratd_myaddress(dev);                       // 子機にPCのアドレスを設定する
    xbee_rat(dev,"ATAP00");                         // XBee APIを解除(UARTモードに設定)
    lcd.setCursor(0,1);
    for(byte i=0;i<4;i++){
        lcd.print(dev_my[i]);                       // 自分のアドレスを表示する
        if(i<3)lcd.print('.');
    }
}

void loop() {
    byte i;
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    xbee_rx_call( &xbee_result );                   // データを受信して変数dataに代入
    if( xbee_result.MODE == MODE_UART){             // 子機XBeeからのUARTを受信時
        lcd.setCursor(0,0);
        for(i=0;i<16;i++){
            char c = (char)xbee_result.DATA[i];
            if( isprint(c) || ((byte)c >= 0xA1 && (byte)c <= 0xFC) ){	// 表示可能文字
                lcd.write(c);                       // 受信結果(テキスト)を表示
            }else{
                lcd.print(' ');                     // 受信結果(空白)を表示
            }
            if( c == '\0' ) break;
        }
        while((++i)<16) lcd.print(' ');             // 残りの表示エリアを空白で埋める
    }
}
