/***************************************************************************************
アナログ電圧をリモート取得する③特定子機の同期取得

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
    unsigned int  value;                        // リモート子機からの入力値
    byte dev[8];                                // XBee子機デバイスのアドレス

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに代入する
    xbee_init( com );                           // XBee用COMポートの初期化
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示
    if(xbee_atnj(30) == 0){                     // デバイスの参加受け入れを開始
        printf("no Devices\n");                 // 子機が見つからなかった
        exit(-1);                               // 異常終了
    }
    printf("Found a Device\n");                 // XBee子機デバイスの発見表示
    xbee_from( dev );                           // 見つけた子機のアドレスを変数devへ
    xbee_ratnj(dev,0);                          // 子機に対して孫機の受け入れ制限を設定
    
    while(1){                                   // 繰り返し処理
        /* XBee通信 */
        value = xbee_adc(dev,1);                // 子機のポート1からアナログ値を入力
        printf("Value =%d\n",value);            // 変数valueの値を表示する
        delay( 1000 );                          // 1000ms(1秒間)の待ち
    }
}
