/***************************************************************************************
アナログ電圧をリモート取得する④特定子機の同期取得

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // データ要求間隔(約10～20msの倍数)

int main(int argc,char **argv){

    byte com=0;                                 // シリアルCOMポート番号
    unsigned int  value;                        // リモート子機からの入力値
    byte dev[8];                                // XBee子機デバイスのアドレス
    byte trig=0xFF;                             // 子機へデータ要求するタイミング調整用
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)

    if(argc==2) com=(byte)atoi(argv[1]);        // 引数があれば変数comに代入する
    xbee_init( com );                           // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                          // 親機に子機のジョイン許可を設定
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示
    
    while(1){
        /* データ送信 */
        if( trig == 0 ){
            xbee_force( dev );                  // 子機へデータ要求を送信
            trig = FORCE_INTERVAL;
        }
        if( trig != 0xFF ) trig--;              // 変数trigが0xFF以外の時に値を1減算

        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );           // データを受信
        switch( xbee_result.MODE ){             // 受信したデータの内容に応じて
            case MODE_RESP:                     // xbee_forceに対する応答の時
                value = xbee_result.ADCIN[1];   // AD1ポートのアナログ値をvalueに代入
                printf("Value =%d\n",value);    // 変数valueの値を表示
                break;
            case MODE_IDNT:                     // 新しいデバイスを発見
                printf("Found a New Device\n");
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピー
                xbee_atnj(0);                   // 親機XBeeに子機の受け入れ制限を設定
                xbee_ratnj(dev,0);              // 子機に対して孫機の受け入れ制限を設定
                xbee_gpio_config(dev,1,AIN);    // 子機XBeeのポート1をアナログ入力に
                trig = 0;                       // 子機へデータ要求を開始
                break;
        }
    }
}
