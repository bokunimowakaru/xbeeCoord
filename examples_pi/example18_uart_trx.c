/***************************************************************************************
親機と子機とのUARTをつかったシリアル送受信

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include <ctype.h>
#include "../libs/xbee.c"
#include "../libs/kbhit.c"

int main(int argc,char **argv){

    char c;                                         // 文字入力用
    char s[32];                                     // 送信データ用
    byte len=0;                                     // 文字長
    byte com=0xB0;                                  // 拡張IOコネクタの場合は0xA0
    byte dev[]={0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};
                                                    // 宛先XBeeアドレス
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);               // 引数があれば変数comに代入する
    xbee_init( com );                               // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                              // 子機XBeeデバイスを常に参加許可
    printf("Waiting for XBee Commissoning\n");      // コミッショニング待ち受け中の表示
    s[0]='\0';                                      // 文字列の初期化

    while(1){
        
        /* データ送信 */
        if( kbhit() ){
            c=getchar();                            // キーボードからの文字入力
            if( isprint( (int)c ) ){                // 表示可能な文字が入力された時
                s[len]=c;                           // 文字列変数sに入力文字を代入する
                len++;                              // 文字長を一つ増やす
                s[len]='\0';                        // 文字列の終了を表す\0を代入する
            }
            if( c == '\n' || len >= 31 ){           // 改行もしくは文字長が31文字の時
                xbee_uart( dev , s );               // 変数sの文字を送信
                xbee_uart( dev,"\r");               // 子機に改行を送信
                len=0;                              // 文字長を0にリセットする
                s[0]='\0';                          // 文字列の初期化
                printf("TX-> ");                    // 待ち受け中の表示
            }
        }
        
        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );               // XBee子機からのデータを受信
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_UART:                         // 子機XBeeからのテキスト受信
                printf("\n");                       // 待ち受け中文字「TX」の行を改行
                printf("RX<- ");                    // 受信を識別するための表示
                printf("%s\n", xbee_result.DATA );  // 受信結果(テキスト)を表示
                printf("TX-> %s",s );               // 文字入力欄と入力中の文字の表示
                break;
            case MODE_IDNT:                         // 新しいデバイスを発見
                printf("\n");                       // 待ち受け中文字「TX」の行を改行
                printf("Found a New Device\n");     // XBee子機デバイスの発見表示
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピー
                xbee_atnj(0);                       // 子機XBeeの受け入れ制限を設定
                xbee_ratnj(dev,0);                  // 子機に対して孫機の受け入れ制限
                xbee_ratd_myaddress( dev );         // 子機に親機のアドレス設定を行う
                printf("TX-> %s",s );               // 文字入力欄と入力中の文字の表示
                break;
        }
    }
}
