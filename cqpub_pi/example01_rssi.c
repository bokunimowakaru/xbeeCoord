/***************************************************************************************
XBeeのLEDを点滅させてみる：Raspberry Piに接続した親機XBeeのRSSI LEDを点滅

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"               // XBeeライブラリのインポート

int main(int argc,char **argv){

    byte com=0xB0;                      // シリアル(USB)、拡張IOコネクタの場合は0xA0
    if(argc==2) com += atoi(argv[1]);   // 引数があれば変数comに値を加算する
    
    xbee_init( com );                   // XBee用COMポートの初期化(引数はポート番号)
    
    while(1){                           // 繰り返し処理
        xbee_at("ATP005");              // ローカルATコマンドATP0(DIO10設定)=05(出力'H')
        delay( 1000 );                  // 約1000ms(1秒間)の待ち
        xbee_at("ATP004");              // ローカルATコマンドATP0(DIO10設定)=04(出力'L')
        delay( 1000 );                  // 約1000ms(1秒間)の待ち
    }
}
