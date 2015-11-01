/***************************************************************************************
XBee Wi-Fiのスイッチ変化通知を受信する

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"                      // XBeeライブラリのインポート
#include "../libs/kbhit.c"

// お手持ちのXBeeモジュールのIPアドレスに変更してください(区切りはカンマ)
byte dev_gpio[] = {192,168,0,135};                  // 子機XBee
byte dev_my[]   = {192,168,0,255};                  // 親機パソコン

int main(void){
    byte value;                                     // 受信値
    XBEE_RESULT xbee_result;                        // 受信データ(詳細)

    xbee_init( 0 );                                 // XBeeの初期化
    printf("Example 68 SW_R (Any key to Exit)\n");
    if( xbee_ping(dev_gpio)==00 ){
        xbee_myaddress(dev_my);                     // 自分のアドレスを設定する
        xbee_gpio_init(dev_gpio);                   // デバイスdev_gpioにIO設定を行う
        while(1){
            xbee_rx_call( &xbee_result );           // データを受信
            if( xbee_result.MODE == MODE_GPIN){     // 子機XBeeのDIO入力
                value = xbee_result.GPI.PORT.D1;    // D1ポートの値を変数valueに代入
                printf("Value =%d\n",value);        // 変数valueの値を表示
                xbee_gpo(dev_gpio,4,value);         // 子機XBeeのDIOポート4へ出力
            }
            if( kbhit() ) break;                    // PCのキー押下時にwhileを抜ける
        }
    }
    printf("\ndone\n");
    return(0);
}
