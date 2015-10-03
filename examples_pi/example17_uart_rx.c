/***************************************************************************************
子機XBeeのUARTからのシリアル情報を受信する

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
    byte dev[8];                                // XBee子機デバイスのアドレス
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに代入する
    xbee_init( com );                           // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                          // 子機XBeeデバイスを常に参加受け入れ
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示
    
    while(1){
        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );           // XBee子機からのデータを受信
        
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_UART:                         // 子機XBeeからのテキスト受信
                printf("RX<- ");                    // 受信を識別するための表示
                printf("%s\n", xbee_result.DATA );  // 受信結果(テキスト)を表示
                break;
            case MODE_IDNT:                         // 新しいデバイスを発見
                printf("Found a New Device\n");
                xbee_atnj(0);                       // 子機XBeeの受け入れ制限を設定
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピー
                xbee_ratnj(dev,0);                  // 子機に対して孫機の受け入れ制限
                xbee_ratd_myaddress( dev );         // 子機に本機のアドレス設定を行う
                break;
        }
    }
}

