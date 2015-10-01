/***************************************************************************************
自作ブレッドボードセンサの測定値を自動送信する

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

void set_ports(byte *dev){
   xbee_gpio_config( dev, 1 , AIN );            // XBee子機のポート1をアナログ入力へ
   xbee_end_device( dev, 3, 3, 0);              // 起動間隔3秒,自動測定3秒,S端子無効
}

int main(int argc,char **argv){

    byte com=0;                                 // シリアルCOMポート番号
    byte dev[8];                                // XBee子機デバイスのアドレス
    float value;                                // 受信データの代入用
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)

    if(argc==2) com=(byte)atoi(argv[1]);        // 引数があれば変数comに代入する
    xbee_init( com );                           // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                          // 子機XBeeデバイスを常に参加受け入れ
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示

    while(1){
        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );               // データを受信
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_GPIN:                         // 子機XBeeの自動送信の受信
                // 照度測定結果をvalueに代入してprintfで表示する
                value = (float)xbee_result.ADCIN[1] * 3.55;
                printf("%.1f Lux\n" , value );
                break;
            case MODE_IDNT:                         // 新しいデバイスを発見
                printf("Found a New Device\n");
                xbee_atnj(0);                       // 子機XBeeデバイスの参加を不許可へ
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピーする
                set_ports( dev );                   // 子機のGPIOポートの設定
                break;
        }
    }
}
