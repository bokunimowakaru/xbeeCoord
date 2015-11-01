/***************************************************************************************
XBee Wi-FiのRSSI LEDとDIO4に接続したLEDを点滅

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"          // XBeeライブラリのインポート
#include "../libs/kbhit.c"

// お手持ちのXBeeモジュールのIPアドレスに変更する(区切りはカンマ)
byte dev[] = {192,168,0,135};

int main(void){
    xbee_init( 0 );                     // XBeeの初期化
    printf("Example 66 RSSI (Any key to Exit)\n");
    while( xbee_ping(dev)==00 ){        // 繰り返し処理
        xbee_rat(dev,"ATP005");         // リモートATコマンドATP0(DIO10設定)=05(出力'H')
        xbee_rat(dev,"ATD405");         // リモートATコマンドATD4(DIO 4設定)=05(出力'H')
        delay( 1000 );                  // 約1000ms(1秒間)の待ち
        xbee_rat(dev,"ATP004");         // リモートATコマンドATP0(DIO10設定)=04(出力'L')
        xbee_rat(dev,"ATD404");         // リモートATコマンドATD4(DIO 4設定)=04(出力'L')
        delay( 1000 );                  // 約1000ms(1秒間)の待ち
        if( kbhit() ) break;            // キーが押されていた時にwhileループを抜ける
    }
    printf("\ndone\n");
    return(0);
}
