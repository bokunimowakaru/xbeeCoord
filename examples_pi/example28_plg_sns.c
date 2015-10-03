/***************************************************************************************
玄関が明るくなったらリビングの家電をONにする

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // データ要求間隔(約10～20msの倍数)

int main(int argc,char **argv){

    byte com=0xB0;                                  // 拡張IOコネクタの場合は0xA0
    byte dev_sens[8];                               // 子機XBee(自作センサ)のアドレス
    byte dev_plug[8];                               // 子機XBee(Smart Plug)のアドレス
    byte dev_sens_en=0;                             // 子機XBee(自作センサ)の状態
    byte dev_plug_en=0;                             // 子機XBee(Smart Plug)の状態
    float value;                                    // 受信データの代入用
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);               // 引数があれば変数comに代入する
    xbee_init( com );                               // XBee用COMポートの初期化
    xbee_atnj(0xFF);                                // 常にジョイン許可に設定
    printf("Waiting for XBee Commissoning\n");      // 待ち受け中の表示
    
    while(1){        
        xbee_rx_call( &xbee_result );                       // データを受信
        switch( xbee_result.MODE ){
            case MODE_GPIN:                                 // 照度センサ自動送信から
                if(bytecmp(xbee_result.FROM,dev_sens,8)==0){ // IEEEアドレスの確認
                    value=(float)xbee_result.ADCIN[1]*3.55; 
                    printf("Entrance %.1f Lux\n" , value );
                    if( dev_plug_en ){
                        if( value > 10 ){                   // 照度が10Lux以下の時
                            xbee_gpo(dev_plug,4,1);         // Smart Plugのポート4をHに
                        }else{
                            xbee_gpo(dev_plug,4,0);         // Smart Plugのポート4をLに
                        }
                        xbee_force( dev_plug );             // Smart Plugの照度取得指示
                    }
                }
                break;
            case MODE_RESP:                                 // xbee_forceに対する応答時
                if(bytecmp(xbee_result.FROM,dev_plug,8)==0){ // IEEEアドレスの確認
                    value=xbee_sensor_result(&xbee_result,WATT);
                    printf("LivingRoom %.1f Watts, ",value);
                    if( xbee_gpi(dev_plug ,4)==0 ){         // ポート4の状態を読みとり、
                        printf("OFF\n");                    // 0の時はOFFと表示する
                    }else{
                        printf("ON\n");                     // 1の時はONと表示する
                    }
                }
                break;
            case MODE_IDNT:                                 // 新しいデバイスを発見
                if( xbee_ping( xbee_result.FROM ) != DEV_TYPE_PLUG){
                    printf("Found a Sensor\n");             // 照度センサの時の処理
                    bytecpy(dev_sens,xbee_result.FROM,8);   // アドレスをdev_sensに代入
                    xbee_gpio_config(dev_sens,1,AIN);       // ポート1をアナログ入力へ
                    xbee_end_device(dev_sens,3,3,0);        // スリープ設定
                    dev_sens_en=1;                          // 照度センサの状態に1を代入
                }else{                                      // Smart Plugの時の処理
                    printf("Found a Smart Plug\n");         // 発見表示
                    bytecpy(dev_plug,xbee_result.FROM,8);   // アドレスをdev_sensに代入
                    xbee_ratnj(dev_plug,0);                 // ジョイン不許可に設定
                    xbee_gpio_config(dev_plug, 1 , AIN );   // ポート1をアナログ入力へ
                    xbee_gpio_config(dev_plug, 2 , AIN );   // ポート2をアナログ入力へ
                    xbee_gpio_config(dev_plug, 3 , AIN );   // ポート3をアナログ入力へ
                    xbee_gpio_config(dev_plug, 4 , DOUT_H); // ポート4をデジタル出力へ
                    dev_plug_en=1;                          // Smart Plugの状態に1を代入
                }
                if( dev_sens_en * dev_plug_en > 0 ){
                    xbee_atnj(0);                           // 親機XBeeに受け入れ制限
                }
                break;
        }
    }
}
