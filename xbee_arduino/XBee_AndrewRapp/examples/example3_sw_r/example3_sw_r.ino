/***************************************************************************************
子機XBeeのスイッチ変化通知を受信する

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/
#include <XBee.h>
XBee xbee = XBee();
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

XBeeAddress64 dev;

void setup() {
    lcd.begin(16, 2);                               // LCDのサイズを16文字×2桁に設定
    lcd.clear();                                    // 画面消去
    lcd.print("Example 3 SW_R ");                   // 文字を表示
    Serial.begin(9600);
    xbee.begin(Serial);
    delay(5000);
    
    lcd.setCursor(0, 1);                            // LCDのカーソル位置を2行目の先頭に
    lcd.print("Waiting for XBee");                  // 待ち受け中の表示
    lcd.setCursor(0, 1);                            // LCDのカーソル位置を2行目の先頭に

    /* xbee_atnj(30)に相当する処理（デバイスの参加受け入れを開始） */
    AtCommandRequest atRequest;                             // ATコマンドOBJ定義
    uint8_t at_nj[] = {'N','J'};                            // 使用するATコマンドATNJ
    uint8_t val_nj[] = {0x1E};                              // その値=0x1E = 30秒
    atRequest.setCommand(at_nj);                            // ATコマンドの設定
    atRequest.setCommandValue(val_nj);                      // 引数のポインタ渡し
    atRequest.setCommandValueLength(sizeof(val_nj));        // 引数のバイト数の設定
    xbee.send(atRequest);                                   // ATコマンド送信
    delay(100);
    ZBRxResponse zbRxResponse = ZBRxResponse();             // XBee応答用OBJ定義
    RemoteAtCommandRequest remoteAtRequest;                 // リモートATコマンドOBJ定義
    int time;
    for(time=0;time<30;time++){
        xbee.readPacket();                                  // データを受信
        if (xbee.getResponse().isAvailable()
            && xbee.getResponse().getApiId()==ZB_IO_NODE_IDENTIFIER_RESPONSE){
            /* xbee_from( dev )に相当する処理（見つけた子機のアドレスを変数devへ） */
            xbee.getResponse().getZBRxResponse(zbRxResponse);
            dev = XBeeAddress64(zbRxResponse.getRemoteAddress64().getMsb(),
                     zbRxResponse.getRemoteAddress64().getLsb());
            lcd.print("Found a Device  ");
            delay(100);
            val_nj[0] = 0x05;                               // ATNJ値を05に変更
            /* ratnj(dev,5)に相当する処理 (子機に対して孫機の受け入れ不許可) */
            remoteAtRequest=RemoteAtCommandRequest(dev,at_nj,val_nj,sizeof(val_nj));
            xbee.send(remoteAtRequest);                     // リモートATコマンド送信
            delay(100);
            /* xbee_gpio_init( dev )に相当する処理（子機の初期設定） */
            uint8_t at_dx[] = {'D','x'};                    // 使用するATコマンドATDn
            uint8_t val_dx[] = {0x03};                      // その値=0x03
            for( uint8_t port = (uint8_t)'1' ; port <= (uint8_t)'3' ; port++ ){
                at_dx[1] = port;
                remoteAtRequest=RemoteAtCommandRequest(dev,at_dx,val_dx,sizeof(val_dx));
                xbee.send(remoteAtRequest);                 // リモートATコマンド送信
                delay(100);
            }
            uint8_t at_ic[] = {'I','C'};                    // 使用するATコマンドATIC
            uint8_t val_ic[] = {0x00,0x0E};                 // その値=0x00,0x0E
            remoteAtRequest=RemoteAtCommandRequest(dev,at_ic,val_ic,sizeof(val_ic));
            xbee.send(remoteAtRequest);                     // リモートATコマンド送信
            /* xbee_atnj(30)に相当する処理の続き(ジョイン制限の設定) */
            atRequest.setCommand(at_nj);                    // ATコマンドの設定
            atRequest.setCommandValue(val_nj);              // 引数のポインタ渡し
            atRequest.setCommandValueLength(sizeof(val_nj));    // 引数のバイト数
            xbee.send(atRequest);                           // ATコマンド送信
            delay(100);
            break;
        }
        delay(1000);
    }
    if(time==30) lcd.print("no Devices      ");             // 子機が見つからなかった
}

void loop() {
    ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

    /* xbee_rx_call( &xbee_result )に相当する処理*/
    xbee.readPacket();                              // XBeeパケットを受信
    if (xbee.getResponse().isAvailable()) {         // 受信データがある時
        /* if( xbee_result.MODE == MODE_GPIN) に相当する処理 */
        if( xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE){
            xbee.getResponse().getZBRxIoSampleResponse(ioSample);
            if (ioSample.containsDigital()) {
                lcd.clear();                        // 画面消去
                lcd.print("D1:");
                lcd.print(ioSample.isDigitalOn(1)); // 子機XBeeのポート1の状態を表示する
                lcd.print(" D2:");
                lcd.print(ioSample.isDigitalOn(2)); // 子機XBeeのポート2の状態を表示する
                lcd.print(" D3:");
                lcd.print(ioSample.isDigitalOn(3)); // 子機XBeeのポート3の状態を表示する
            } 
        }
    }
}
