/***************************************************************************************
スイッチ状態をリモート取得する②変化通知

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

// お手持ちのXBeeモジュール子機のIEEEアドレスに変更する↓
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){
    
    byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
    byte value;                                 // 受信値
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに代入する
    xbee_init( com );                           // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                          // 親機XBeeを常にジョイン許可状態にする
    xbee_gpio_init( dev );                      // 子機のDIOにIO設定を行う(送信)
    
    while(1){
        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );           // データを受信
        if( xbee_result.MODE == MODE_GPIN){     // 子機XBeeからのDIO入力の時（条件文）
            value = xbee_result.GPI.PORT.D1;    // D1ポートの値を変数valueに代入
            printf("Value =%d\n",value);        // 変数valueの値を表示
        }
    }
}
