/***************************************************************************************
子機XBeeのスイッチ状態をリモートで取得する

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/
#include <XBee.h>
#include <LiquidCrystal.h>
#define FORCE_INTERVAL  250                         // データ要求間隔(10msの倍数)

XBee xbee = XBee();
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
unsigned long time_next;                            // 次回の測定時間

// お手持ちのXBeeモジュール子機のIEEEアドレスに変更する↓
XBeeAddress64 dev = XBeeAddress64(0x0013a200, 0x4030C16F);

void setup() {
    lcd.begin(16, 2);                               // LCDのサイズを16文字×2桁に設定
    lcd.clear();                                    // 画面消去
    lcd.print("Example 4 SW_F ");                   // 文字を表示
    Serial.begin(9600);
    xbee.begin(Serial);
    delay(5000);                                    // XBeeの起動待ち

    /* xbee_atnj(30)に相当する処理は省略(example53_sw_rを参照) */

    RemoteAtCommandRequest remoteAtRequest;         // リモートATコマンドOBJの定義
    
    /* xbee_gpio_config(dev,1～3,DIN)に相当する処理（子機の初期設定） */
    uint8_t at_dx[] = {'D','x'};                    // 使用するATコマンドATDn
    uint8_t val_dx[] = {0x03};                      // その値=0x03
    for( uint8_t port = (uint8_t)'1' ; port <= (uint8_t)'3' ; port++ ){
        at_dx[1] = port;
        remoteAtRequest=RemoteAtCommandRequest(dev,at_dx,val_dx,sizeof(val_dx));
        xbee.send(remoteAtRequest);
        delay(100);
    }
    time_next = millis() + 10*FORCE_INTERVAL;
    if( time_next < 10*FORCE_INTERVAL ){
        while( millis() > FORCE_INTERVAL );
        time_next += 10*FORCE_INTERVAL;
    }
}


void loop() {
    RemoteAtCommandResponse remoteAtResponse = RemoteAtCommandResponse();
    
    if( millis() > time_next){
        /* xbee_force( dev )に相当する処理（子機へデータ要求を送信） */
        RemoteAtCommandRequest remoteAtRequest;     // リモートATコマンドOBJの定義
        uint8_t at_is[] = {'I','S'};                // 使用するATコマンドATIS
        remoteAtRequest=RemoteAtCommandRequest(dev,at_is);
        xbee.send(remoteAtRequest);
        time_next += 10*FORCE_INTERVAL;
        delay(100);
    }

    xbee.readPacket();                              // XBeeパケットを受信
    if (xbee.getResponse().isAvailable()) {         // 受信データがある時
        if( xbee.getResponse().getApiId() == REMOTE_AT_COMMAND_RESPONSE){
            xbee.getResponse().getRemoteAtCommandResponse(remoteAtResponse);
            if(remoteAtResponse.getValueLength() >= 6 ){
                byte val = remoteAtResponse.getValue()[2];
                val &= remoteAtResponse.getValue()[5];
                lcd.clear();                        // 画面消去
                lcd.print("D1:");
                lcd.print(val>>1&0x01);             // 子機XBeeのポート1の状態を表示する
                lcd.print(" D2:");
                lcd.print(val>>2&0x01);             // 子機XBeeのポート2の状態を表示する
                lcd.print(" D3:");
                lcd.print(val>>3&0x01);             // 子機XBeeのポート3の状態を表示する
            } 
        }
    }
}
