/***************************************************************************************
LEDをリモート制御する②ライブラリ関数xbee_gpoで簡単制御

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

// お手持ちのXBeeモジュール子機のIEEEアドレスに変更する↓
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){
    
    byte com=0xB0;                      // シリアル(USB)、拡張IOコネクタの場合は0xA0
    
    if(argc==2) com += atoi(argv[1]);   // 引数があれば変数comに代入する
    xbee_init( com );                   // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                  // 親機XBeeを常にジョイン許可状態にする
    
    while(1){                           // 繰り返し処理
        xbee_gpo(dev, 4, 1);            // リモートXBeeのポート4を出力'H'に設定する
        delay( 1000 );                  // 1000ms(1秒間)の待ち
        xbee_gpo(dev, 4, 0);            // リモートXBeeのポート4を出力'L'に設定する
        delay( 1000 );                  // 1000ms(1秒間)の待ち
    }
}
