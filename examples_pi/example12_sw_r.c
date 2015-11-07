/***************************************************************************************
子機XBeeのスイッチ変化通知を受信する

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){
    
    byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
    byte value;                                 // 受信値
    byte dev[8];                                // XBee子機デバイスのアドレス
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに値を加算する
    xbee_init( com );                           // XBee用COMポートの初期化
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示
    if(xbee_atnj(30) != 0){                     // デバイスの参加受け入れを開始
        printf("Found a Device\n");             // XBee子機デバイスの発見表示
        xbee_from( dev );                       // 見つけた子機のアドレスを変数devへ
        xbee_ratnj(dev,0);                      // 子機に対して孫機の受け入れ制限を設定
        xbee_gpio_init( dev );                  // 子機のDIOにIO設定を行う(送信)
    }else printf("no Devices\n");               // 子機が見つからなかった
    
    while(1){
        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );           // データを受信
        if( xbee_result.MODE == MODE_GPIN){     // 子機XBeeのDIO入力
            value = xbee_result.GPI.PORT.D1;    // D1ポートの値を変数valueに代入
            printf("Value =%d ",value);         // 変数valueの値を表示
            value = xbee_result.GPI.BYTE[0];    // D7～D0ポートの値を変数valueに代入
            lcd_disp_bin( value );              // valueに入った値をバイナリで表示
            printf("\n");                       // 改行
        }
    }
}
