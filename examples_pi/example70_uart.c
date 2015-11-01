/***************************************************************************************
XBee Wi-Fiを使ったUARTシリアル送受信

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"                      // XBeeライブラリのインポート
#include "../libs/kbhit.c"
#include <ctype.h>

// お手持ちのXBeeモジュールのIPアドレスに変更してください(区切りはカンマ)
byte dev[]      = {192,168,0,135};                  // 子機XBee
byte dev_my[]   = {192,168,0,255};                  // 親機パソコン

int main(void){
    char c;                                         // 文字入力用
    char s[32];                                     // 送信データ用
    byte len=0;                                     // 文字長
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    xbee_init( 0 );                                 // XBee用COMポートの初期化
    printf("Example 70 UART (ESC key to Exit)\n");
    s[0]='\0';                                      // 文字列の初期化
    printf("TX-> ");                                // 待ち受け中の表示
    if( xbee_ping(dev)==00 ){
        xbee_myaddress(dev_my);                     // PCのアドレスを設定する
        xbee_ratd_myaddress(dev);                   // 子機にPCのアドレスを設定する
        xbee_rat(dev,"ATAP00");                     // XBee APIを解除(UARTモードに設定)
        while(1){

            /* データ送信 */
            if( kbhit() ){
                c=getchar();                        // キーボードからの文字入力
                if( c == 0x1B ){                    // ESCキー押下時に
                    printf("E");                    // ESC E(改行)を実行
                    break;                          // whileを抜ける
                }
                if( isprint( (int)c ) ){            // 表示可能な文字が入力された時
                    s[len]=c;                       // 文字列変数sに入力文字を代入する
                    len++;                          // 文字長を一つ増やす
                    s[len]='\0';                    // 文字列の終了を表す\0を代入する
                }
                if( c == '\n' || len >= 31 ){       // 改行もしくは文字長が31文字の時
                    xbee_uart( dev , s );           // 変数sの文字を送信
                    xbee_uart( dev,"\r");           // 子機に改行を送信
                    len=0;                          // 文字長を0にリセットする
                    s[0]='\0';                      // 文字列の初期化
                    printf("TX-> ");                // 待ち受け中の表示
                }
            }

            /* データ受信(待ち受けて受信する) */
            xbee_rx_call( &xbee_result );           // XBee Wi-Fiからのデータを受信
            if( xbee_result.MODE == MODE_UART){     // UARTシリアルデータを受信した時
                printf("\n");                       // 待ち受け中文字「TX」の行を改行
                printf("RX<- ");                    // 受信を識別するための表示
                printf("%s\n", xbee_result.DATA );  // 受信結果(テキスト)を表示
                printf("TX-> %s",s );               // 文字入力欄と入力中の文字の表示
            }
        }
    }
    printf("done\n");
    return(0);
}
