/***************************************************************************************
親機と子機とのUARTをつかったシリアル送受信

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include <ctype.h>
#include "../libs/xbee.c"
#include "../libs/kbhit.c"

int main(int argc,char **argv){
    byte com=0xB0;                                  // 拡張IOコネクタの場合は0xA0
    byte dev[]={0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};
                                                    // 宛先XBeeアドレス
    char s[32];                                     // 送信データ用
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);               // 引数があれば変数comに値を加算する
    xbee_init( com );                               // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                              // 子機XBeeデバイスを常に参加許可
    printf("Waiting for XBee Commissoning\n");      // コミッショニング待ち受け中の表示

    while(1){
        /* データ送信 */
        if( kbhit() ){
            fgets(s, 32, stdin);                    // キーボードからの文字入力
            xbee_uart( dev , s );                   // 変数sの文字を送信
            printf("TX-> ");                        // 待ち受け中の表示
        }
        
        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );               // XBee子機からのデータを受信
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_UART:                         // 子機XBeeからのテキスト受信
                printf("\n");                       // 待ち受け中文字「TX」の行を改行
                printf("RX<- ");                    // 受信を識別するための表示
                printf("%s\n", xbee_result.DATA );  // 受信結果(テキスト)を表示
                printf("TX-> ");                    // 文字入力欄を表示
                break;
            case MODE_IDNT:                         // 新しいデバイスを発見
                printf("\n");                       // 待ち受け中文字「TX」の行を改行
                printf("Found a New Device\n");     // XBee子機デバイスの発見表示
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピー
                xbee_atnj(0);                       // 子機XBeeの受け入れ制限を設定
                xbee_ratnj(dev,0);                  // 子機に対して孫機の受け入れ制限
                xbee_ratd_myaddress( dev );         // 子機に親機のアドレス設定を行う
                printf("TX-> ");                    // 文字入力欄を表示
                break;
        }
    }
}
