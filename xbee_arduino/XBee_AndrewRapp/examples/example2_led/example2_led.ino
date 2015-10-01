/***************************************************************************************
LEDをリモート制御する：リモート子機のDIO4(XBee pin 11)のLEDを点滅。

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/
#include <XBee.h>
XBee xbee = XBee();

// お手持ちのXBeeモジュール子機のIEEEアドレスに変更する↓
XBeeAddress64 dev = XBeeAddress64(0x0013a200, 0x4030C16F);

void setup() {
    Serial.begin(9600);                                     // XBee用シリアルの開始
    xbee.begin(Serial);                                     // XBeeの開始
    delay(5000);                                            // XBeeの起動待ち
}

void loop() {
    
    /* xbee_gpo(dev, 4, 1); に相当する処理 */
    uint8_t at_d4[] = {'D','4'};                            // 使用するATコマンドATD4
    uint8_t val_d405[] = {0x05};                            // その値＝0x05
    RemoteAtCommandRequest remoteAtRequest;                 // リモートATコマンドOBJ定義
    remoteAtRequest                           // 宛先,ATコマンドの設定,引数,引数長を設定
        =RemoteAtCommandRequest(dev,at_d4,val_d405,sizeof(val_d405));
    xbee.send(remoteAtRequest);                             // リモートATコマンド送信
    delay(1000);

    /* xbee_gpo(dev, 4, 0); に相当する処理 */
    uint8_t val_d404[] = {0x04};                            // その値＝0x04
    remoteAtRequest.setCommandValue(val_d404);
    remoteAtRequest.setCommandValueLength(sizeof(val_d404));
    xbee.send(remoteAtRequest);
    delay(1000);
}
