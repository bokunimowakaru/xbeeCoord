/***************************************************************************************
XBee Wi-Fiのスイッチ状態をリモートで取得しつつスイッチ変化通知でも取得する

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"                      // XBeeライブラリのインポート
#include "../libs/kbhit.c"
#define FORCE_INTERVAL  200                         // データ要求間隔(およそ30msの倍数)

// お手持ちのXBeeモジュールのIPアドレスに変更してください(区切りはカンマ)
byte dev_gpio[] = {192,168,0,135};                  // 子機XBee
byte dev_my[]   = {192,168,0,255};                  // 親機パソコン

int main(void){
    byte trig=0;
    byte value;                                     // 受信値
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    xbee_init( 0 );                                 // XBeeの初期化
    printf("Example 69 SW_F (Any key to Exit)\n");
    if( xbee_ping(dev_gpio)==00 ){
        xbee_myaddress(dev_my);                     // 自分のアドレスを設定する
        xbee_gpio_init(dev_gpio);                   // デバイスdev_gpioにIO設定を行う
        xbee_end_device(dev_gpio,28,0,0);           // デバイスdev_gpioを省電力に設定
        while(1){
            /* 取得要求の送信 */
            if( trig == 0){
                xbee_force( dev_gpio );             // 子機へデータ要求を送信
                trig = FORCE_INTERVAL;
            }
            trig--;

            /* データ受信(待ち受けて受信する) */
            xbee_rx_call( &xbee_result );           // データを受信
            if( xbee_result.MODE == MODE_RESP ||    // xbee_forceに対する応答
                xbee_result.MODE == MODE_GPIN){     // もしくは子機XBeeのDIO入力の時
                value = xbee_result.GPI.PORT.D1;    // D1ポートの値を変数valueに代入
                printf("Value =%d\n",value);        // 変数valueの値を表示
                xbee_gpo(dev_gpio,4,value);         // 子機XBeeのDIOポート4へ出力
            }
            if( kbhit() ) break;                    // PCのキー押下時にwhileを抜ける
        }
        xbee_end_device(dev_gpio,0,0,0);            // デバイスdev_gpioの省電力を解除
    }
    printf("\ndone\n");
    return(0);
}
