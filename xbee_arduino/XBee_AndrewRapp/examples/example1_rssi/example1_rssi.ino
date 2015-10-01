/***************************************************************************************
XBeeのLEDを点滅させてみる：Arduinoに接続した親機XBeeのRSSI LEDを点滅

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/
#include <XBee.h>
XBee xbee = XBee();

void setup() { 
    Serial.begin(9600);                                     // XBee用シリアルの開始
    xbee.begin(Serial);                                     // XBeeの開始
    delay(5000);                                            // XBeeの起動待ち
}

void loop() {
    
    /* xbee_at("ATP005"); に相当する処理 */
    uint8_t at_p0[] = {'P','0'};                            // 使用するATコマンドATP0
    uint8_t val_p005[] = {0x05};                            // その値＝0x05
    AtCommandRequest atRequest;                             // ATコマンドOBJ定義
    atRequest.setCommand(at_p0);                            // ATコマンドの設定
    atRequest.setCommandValue(val_p005);                    // 引数のポインタ渡し
    atRequest.setCommandValueLength(sizeof(val_p005));      // 引数のバイト数の設定
    xbee.send(atRequest);                                   // ATコマンド送信
    delay(1000);

    /* xbee_at("ATP004"); に相当する処理 */
    uint8_t val_p004[] = {0x04};                            // その値＝0x04
    atRequest.setCommandValue(val_p004);
    atRequest.setCommandValueLength(sizeof(val_p004));
    xbee.send(atRequest);
    delay(1000);
}
