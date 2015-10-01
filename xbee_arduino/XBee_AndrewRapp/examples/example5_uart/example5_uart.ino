/***************************************************************************************
子機XBeeのUARTからのシリアル情報を受信する

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/
#include <XBee.h>
#include <LiquidCrystal.h>

XBee xbee = XBee();
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

XBeeAddress64 dev = XBeeAddress64(0x00000000, 0x0000FFFF);

void setup() {
    lcd.begin(16, 2);                               // LCDのサイズを16文字×2桁に設定
    lcd.clear();                                    // 画面消去
    lcd.print("Example 5 UART ");                   // 文字を表示
    Serial.begin(9600);
    xbee.begin(Serial);
    delay(5000);                                    // XBeeの起動待ち
    lcd.setCursor(0, 1);                            // LCDのカーソル位置を2行目の先頭に
    lcd.print("Waiting for XBee");                  // 待ち受け中の表示
}

void loop() {
    ZBRxResponse zbRxResponse = ZBRxResponse();

    xbee.readPacket();                              // パケットを受信
    if (xbee.getResponse().isAvailable()) {         // 受信データがある時
        switch( xbee.getResponse().getApiId() ){    // 受信したデータの内容に応じて
            case ZB_RX_RESPONSE:                    // 子機XBeeからデータを受信した時
                xbee.getResponse().getZBRxResponse(zbRxResponse);
                lcd.clear();                        // 画面消去
                for(int i=0;i<16;i++){
                    if(zbRxResponse.getData()[i]==0x00 ||
                    i >= zbRxResponse.getDataLength() )break;
                    lcd.print( (char)zbRxResponse.getData()[i]);    // 受信結果表示
                }
                break;
            case ZB_IO_NODE_IDENTIFIER_RESPONSE:    // 子機がコミッションボタンを押した時
                lcd.clear();                        // 画面消去
                lcd.print("Found a New Dev.");
                
                /* atnj(5)を実行 (子機XBeeデバイスの参加制限に設定) */
                uint8_t at_nj[] = {'N','J'};                    // ATコマンドATNJ
                uint8_t val_nj[] = {0x05};                      // その値＝0x05
                AtCommandRequest atRequest;                     // ATコマンドOBJ定義
                atRequest.setCommand(at_nj);                    // ATコマンドの設定
                atRequest.setCommandValue(val_nj);              // 引数のポインタ渡し
                atRequest.setCommandValueLength(sizeof(val_nj));    // 引数のバイト数
                xbee.send(atRequest);                           // ATコマンド送信
                delay(100);
                
                /* 子機XBeeのアドレスを取得してdevへ登録 */
                xbee.getResponse().getZBRxResponse(zbRxResponse);
                dev = XBeeAddress64(zbRxResponse.getRemoteAddress64().getMsb(),
                         zbRxResponse.getRemoteAddress64().getLsb());

                /* ratnj(dev,0)を実行 (子機に対して孫機の受け入れ不許可) */
                RemoteAtCommandRequest remoteAtRequest; // リモートATコマンドOBJ定義
                remoteAtRequest=RemoteAtCommandRequest(dev,at_nj,val_nj,
                                                                    sizeof(val_nj));
                xbee.send(remoteAtRequest);             // リモートATコマンド送信
                delay(100);
                uint8_t text[] = {'H', 'e', 'l', 'l', 'o', '\n'};
                ZBTxRequest zbTxRequest = ZBTxRequest(dev, text, 6);
                xbee.send(zbTxRequest);             // 子機に文字を送信
                break;
        }
    }
}
