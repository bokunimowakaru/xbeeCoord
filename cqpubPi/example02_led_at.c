/***************************************************************************************
LEDをリモート制御する①リモートATコマンド：リモート子機のDIO4(XBee pin 11)のLEDを点滅。

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

// お手持ちのXBeeモジュール子機のIEEEアドレスに変更する↓
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){
    
    byte com=0;                         // シリアルCOMポート番号
    
    if(argc==2) com=(byte)atoi(argv[1]);// 引数があれば変数comに代入する
    xbee_init( com );                   // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                  // 親機XBeeを常にジョイン許可状態にする

    while(1){                           // 繰り返し処理
        xbee_rat(dev,"ATD405");         // リモートATコマンドATD4(DIO4設定)=05(出力'H')
        delay( 1000 );                  // 1000ms(1秒間)の待ち
        xbee_rat(dev,"ATD404");         // リモートATコマンドATD4(DIO4設定)=04(出力'L')
        delay( 1000 );                  // 1000ms(1秒間)の待ち
    }
}
