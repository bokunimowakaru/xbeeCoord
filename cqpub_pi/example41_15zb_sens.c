/***************************************************************************************
IchigoJam用 ワイヤレスセンサから情報を取得する

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#include "../libs/kbhit.c"
#include <ctype.h>                              // isprintを使うためのライブラリ

int main(int argc,char **argv){

    byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
    byte dev[8];                                // XBee子機デバイスのアドレス
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)
    int i,size;

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに値を加算する
    xbee_init( com );                           // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                          // 子機XBeeデバイスを常に参加受け入れ
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示

    while(1){
        /* データ受信(待ち受けて受信する) */
        size=xbee_rx_call( &xbee_result );          // データを受信,sizeは受信長
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_UART:                         // 子機XBeeの自動送信の受信
                printf("RX<- ");
                for(i=0; i<size; i++){                          // 繰り返し処理
                    if( isprint(xbee_result.DATA[i]) ){         // 表示可能な文字の時
                        printf("%c" , xbee_result.DATA[i] );    // 受信文字を表示
                    }
                }
                printf("\n");
                break;
            case MODE_IDNT:                         // 新しいデバイスを発見
                printf("Found a New Device\n");
                xbee_atnj(0);                       // 子機XBeeデバイスの参加を不許可へ
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピーする
                xbee_rat(dev,"ATST03E8");           // 子機スリープ実行猶予時間を1秒に
                xbee_end_device( dev, 20, 20, 0);   // 起動間隔20秒,自動測定20秒
                break;
        }
        if( kbhit() ) break;                        // キーが押された場合に終了
    }
    printf("done\n");
    return(0);
}
