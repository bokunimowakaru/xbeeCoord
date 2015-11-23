/***************************************************************************************
暗くなったらSmart Plugの家電の電源をOFFにする

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  1000                    // データ要求間隔(およそms単位)

void set_ports(byte *dev){
    xbee_gpio_config( dev, 1 , AIN );           // XBee子機のポート1をアナログ入力AINへ
    xbee_gpio_config( dev, 2 , AIN );           // XBee子機のポート2をアナログ入力AINへ
    xbee_gpio_config( dev, 3 , AIN );           // XBee子機のポート3をアナログ入力AINへ
    xbee_gpio_config( dev, 4 , DOUT_H );        // XBee子機のポート3をデジタル出力へ
}

int main(int argc,char **argv){

    byte com=0xB0;                                  // 拡張IOコネクタの場合は0xA0
    byte dev[8];                                    // XBee子機デバイスのアドレス
    int trig = -1;                                  // データ要求するタイミング調整用
    byte id=0;                                      // パケット送信番号
    float value;                                    // 受信データの代入用
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);               // 引数があれば変数comに値を加算する
    xbee_init( com );                               // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                              // 親機に子機のジョイン許可を設定
    printf("Waiting for XBee Commissoning\n");      // 待ち受け中の表示
    
    while(1){
        if( trig == 0){
            id = xbee_force( dev );                 // 子機へデータ要求を送信
            trig = FORCE_INTERVAL;
        }
        if( trig > 0 ) trig--;                      // 変数trigが正の整数の時に値を1減算

        xbee_rx_call( &xbee_result );               // データを受信
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_RESP:                         // xbee_forceに対する応答の時
                if( id == xbee_result.ID ){         // 送信パケットIDが一致
                    value = xbee_sensor_result( &xbee_result, LIGHT);
                    printf("%.1f Lux, " , value );
                    if( value < 10 ){               // 照度が10Lux以下の時
                        xbee_gpo(dev , 4 , 0 );     // 子機XBeeのポート4をLに設定
                    }
                    value = xbee_sensor_result( &xbee_result,WATT);
                    printf("%.1f Watts, " , value );
                    if( xbee_gpi( dev , 4 ) == 0 ){ // ポート4の状態を読みとり、
                        printf("OFF\n");            // 0の時はOFFと表示する
                    }else{
                        printf("ON\n");             // 1の時はONと表示する
                    }
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
