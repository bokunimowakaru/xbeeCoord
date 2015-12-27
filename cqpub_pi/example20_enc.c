/***************************************************************************************
親機と子機との暗号化データの送受信

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/
#include <ctype.h>
#include "../libs/xbee.c"
#include "../libs/kbhit.c"

int main(int argc,char **argv){
    byte com=0xB0;                                  // 拡張IOコネクタの場合は0xA0
    byte dev[]={0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};   // 相手先XBeeアドレス
    char s[32];                                     // 文字入力用
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)
    
    if(argc==2) com += atoi(argv[1]);               // 引数があれば変数comに値を加算する
    xbee_init( com );                               // XBee用COMポートの初期化
    if( xbee_atee_on("password") <= 1){             // 暗号化ON。passwordは16文字まで
        printf("Encryption On\n");                  // "password" -> 70617373776F7264
    }else{
        printf("Encryption Error\n");               // 暗号化エラー表示
        exit(-1); 
    }
    xbee_atnj( 0xFF );                              // 子機XBeeデバイスを常に参加許可

    while(1){
        /* データ送信 */
        if( kbhit() ){
            fgets(s, 32, stdin);                    // キーボードからの文字入力
            xbee_uart( dev , s );                   // 変数sの文字を送信
            printf("TX-> ");                        // 待ち受け中の表示
        }
        /* データ受信 */
        xbee_rx_call( &xbee_result );               // XBee子機からのデータを受信
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_UART:                         // 子機XBeeからのテキスト受信
                printf("\nRX<- %s\nTX-> ", xbee_result.DATA );  // 受信データ等の表示
                break;
            case MODE_IDNT:                         // 新しいデバイスを発見
                printf("\nFound a New Device\nTX-> ");          // 子機発見表示
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピー
                xbee_ratd_myaddress( dev );         // 子機に親機のアドレス設定を行う
                xbee_uart( dev , "Join in\n" );     // 「Join in」を返信
                break;
        }
    }
}
