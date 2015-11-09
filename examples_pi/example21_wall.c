/***************************************************************************************
Digi純正XBee Wall Routerで照度と温度を測定する

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  1000                        // データ要求間隔(およそms単位)
#define TEMP_OFFSET     3.8                         // XBee Wall Router内部温度上昇

void set_ports(byte *dev){
    xbee_gpio_config( dev, 1 , AIN );               // XBee子機のポート1をアナログ入力へ
    xbee_gpio_config( dev, 2 , AIN );               // XBee子機のポート2をアナログ入力へ
}

int main(int argc,char **argv){

    byte com=0xB0;                                  // 拡張IOコネクタの場合は0xA0
    byte dev[8];                                    // XBee子機デバイスのアドレス
    byte id=0;                                      // パケット送信番号
    int trig =0xFF;                                 // データ要求するタイミング調整用
    float value;                                    // 受信データの代入用
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);               // 引数があれば変数comに値を加算する
    xbee_init( com );                               // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                              // 親機に子機のジョイン許可を設定
    printf("Waiting for XBee Commissoning\n");      // 待ち受け中の表示
    
    while(1){
        /* データ送信 */
        if( trig == 0){
            id = xbee_force( dev );                 // 子機へデータ要求を送信
            trig = FORCE_INTERVAL;
        }
        if( trig != 0xFF ) trig--;                  // 変数trigが0xFF以外の時に値を1減算

        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );               // データを受信
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_RESP:                         // xbee_forceに対する応答の時
                if( id == xbee_result.ID ){         // 送信パケットIDが一致
                    // 照度測定結果をvalueに代入してprintfで表示する
                    value = xbee_sensor_result( &xbee_result, LIGHT);
                    printf("%.1f Lux, " , value );
                    // 温度測定結果をvalueに代入してprintfで表示する
                    value = xbee_sensor_result( &xbee_result, TEMP);
                    value -= TEMP_OFFSET;
                    printf("%.1f degC\n" , value );
                }
                break;
            case MODE_IDNT:                         // 新しいデバイスを発見
                printf("Found a New Device\n");
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピーする
                xbee_atnj(0);                       // 親機XBeeに子機の受け入れ制限
                xbee_ratnj(dev,0);                  // 子機に対して孫機の受け入れを制限
                set_ports( dev );                   // 子機のGPIOポートの設定
                trig = 0;                           // 子機へデータ要求を開始
                break;
        }
    }
}
