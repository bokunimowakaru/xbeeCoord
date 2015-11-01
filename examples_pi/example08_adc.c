/***************************************************************************************
アナログ電圧をリモート取得する①同期取得

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

// お手持ちのXBeeモジュール子機のIEEEアドレスに変更する↓
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){
    byte com=0xB0;                      // シリアル(USB)、拡張IOコネクタの場合は0xA0
    unsigned int  value;                // リモート子機からの入力値
    
    if(argc==2) com += atoi(argv[1]);   // 引数があれば変数comに代入する
    xbee_init( com );                   // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                  // 親機XBeeを常にジョイン許可状態にする

    while(1){                           // 繰り返し処理
        /* XBee通信 */
        value = xbee_adc(dev,1);        // リモート子機のポート1からアナログ値を入力
        printf("Value =%d\n",value);    // 変数valueの値を表示する
        delay( 1000 );                  // 1000ms(1秒間)の待ち
    }
}
