/***************************************************************************************
Digi純正XBee Sensorで照度と温度を測定する

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

void set_ports(byte *dev){
    /* XBee子機のGPIOを設定 */
    xbee_gpio_config( dev, 1 , AIN );               // XBee子機ポート1をアナログ入力へ
    xbee_gpio_config( dev, 2 , AIN );               // XBee子機ポート2をアナログ入力へ
    /* XBee子機をスリープに設定 */
    xbee_end_device(dev, 3, 3, 0);                  // 起動間隔3秒,測定間隔3秒
}                                                   // SLEEP端子無効

int main(int argc,char **argv){

    byte com=0;                                     // シリアルCOMポート番号
    byte dev[8];                                    // XBee子機デバイスのアドレス
    float value;                                    // 受信データの代入用
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    if(argc==2) com=(byte)atoi(argv[1]);            // 引数があれば変数comに代入する
    xbee_init( com );                               // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                              // 親機に子機のジョイン許可を設定
    printf("Waiting for XBee Commissoning\n");      // 待ち受け中の表示

    while(1){
        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );               // データを受信
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_GPIN:                         // 子機XBeeの自動送信の受信
                // 照度測定結果をvalueに代入してprintfで表示する
                value = xbee_sensor_result( &xbee_result, LIGHT);
                printf("%.1f Lux, " , value );
                // 温度測定結果をvalueに代入してprintfで表示する
                value = xbee_sensor_result( &xbee_result, TEMP);
                printf("%.1f degC\n" , value );
                break;
            case MODE_IDNT:                         // 新しいデバイスを発見
                printf("Found a New Device\n");
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピー
                xbee_atnj(0);                       // 子機XBeeデバイスの参加制限設定
                set_ports( dev );                   // 子機のGPIOポートの設定
                break;
        }
    }
}
