/***************************************************************************************
XBee Wi-Fi搭載の照度センサから照度値を取得する

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"                  // XBeeライブラリのインポート
#include "../libs/kbhit.c"

// お手持ちのXBeeモジュールのIPアドレスに変更してください(区切りはカンマ)
byte dev[]   = {192,168,0,135};                 // 子機XBee
byte dev_my[]= {192,168,0,255};                 // 親機Raspberry Pi

#define FORCE_INTERVAL  5                       // データ要求間隔(秒)
#define S_MAX   256                             // 文字列変数sの最大容量(255文字)を定義

int main(void){
    float value;                                // 受信データの代入用
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)
    time_t timer;                               // タイマー変数の定義
    time_t trig=0;                              // 取得タイミング保持用
    struct tm *time_st;                         // タイマーによる時刻格納用の構造体定義
    char s[S_MAX];                              // 文字列用の変数

    printf("Example 34 MySns\n");               // タイトル文字を表示
    xbee_init( 0 );                             // XBee用Ethenet UDPポートの初期化
    printf("ping...\n");
    while( xbee_ping(dev) ) delay(3000);        // XBee Wi-Fiから応答があるまで待機
    xbee_myaddress(dev_my);                     // 自分のアドレスを取得する
    xbee_ratd_myaddress(dev);                   // 子機にPCのアドレスを設定する
    xbee_gpio_config( dev, 1 , AIN );           // XBee子機のポート1をアナログ入力へ
    xbee_end_device(dev,28,0,0);                // XBee Wi-Fiモジュールを省電力へ
    while(1){
        time(&timer);                           // 現在の時刻を変数timerに取得する
        time_st = localtime(&timer);            // timer値を時刻に変換してtime_stへ

        if( timer >= trig ){                    // 変数trigまで時刻が進んだとき
            xbee_force(dev);                    // 状態取得指示を送信
            trig = timer + FORCE_INTERVAL;      // 次回の変数trigを設定
        }
        xbee_rx_call( &xbee_result );           // データを受信
        if( xbee_result.MODE == MODE_RESP){     // 子機XBeeからのIOデータの受信時
            value = (float)xbee_result.ADCIN[1] * 7.4;          // 照度をvalueに代入
            strftime(s,S_MAX,"%Y/%m/%d, %H:%M:%S", time_st);    // 時刻→文字列変換
            sprintf(s,"%s, %.1f", s , value );                  // 測定結果をsに追加
            printf("%s Lux\n" , s );                            // 文字列sを表示
        }
        if( kbhit() ){                          // キーが押された時
            if( getchar()=='q' ) break;         // 押されたキーがqだった場合に終了
        }
    }
    xbee_end_device(dev,0,0,0);                 // デバイスdev_gpioの省電力を解除
    printf("done\n");
    return(0);
}
