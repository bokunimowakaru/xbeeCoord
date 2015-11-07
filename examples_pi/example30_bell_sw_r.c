/***************************************************************************************
XBeeスイッチとXBeeブザーで玄関呼鈴を製作する

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // データ要求間隔(約10～20msの倍数)

void bell(byte *dev, byte c){
    byte i;
    for(i=0;i<c;i++){
        xbee_gpo( dev , 4 , 1 );                // 子機 devのポート4を1に設定(送信)
        xbee_gpo( dev , 4 , 0 );                // 子機 devのポート4を0に設定(送信)
    }
    xbee_gpo( dev , 4 , 0 );                    // 子機 devのポート4を0に設定(送信)
}

int main(int argc,char **argv){
    
    byte com=0xB0;                              // シリアル、拡張IOコネクタの場合は0xA0
    byte dev_bell[8];                           // XBee子機(ブザー)デバイスのアドレス
    byte dev_sw[8];                             // XBee子機(スイッチ)デバイスのアドレス
    byte trig=0;                                // 子機へデータ要求するタイミング調整用
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに値を加算する
    xbee_init( com );                           // XBee用COMポートの初期化
    printf("Waiting for XBee Bell\n");
    xbee_atnj(60);                              // デバイスの参加受け入れ
    xbee_from( dev_bell );                      // 見つけた子機のアドレスを変数devへ
    bell(dev_bell,3);                           // ベルを3回鳴らす
    xbee_end_device(dev_bell, 1, 0, 0);         // 起動間隔1秒,自動測定OFF,SLEEP端子無効
    printf("Waiting for XBee Switch\n");
    xbee_atnj(60);                              // デバイスの参加受け入れ
    xbee_from( dev_sw );                        // 見つけた子機のアドレスを変数devへ
    bell(dev_bell,3);                           // ブザーを3回鳴らす
    xbee_gpio_init( dev_sw );                   // 子機のDIOにIO設定を行う
    xbee_end_device(dev_sw, 3, 0, 1);           // 起動間隔3秒,自動送信OFF,SLEEP端子有効
    printf("done\n");

    while(1){
        if( trig == 0){
            xbee_force( dev_sw );                       // 子機へデータ要求を送信
            trig = FORCE_INTERVAL;
        }
        trig--;
        xbee_rx_call( &xbee_result );                   // データを受信
        switch( xbee_result.MODE ){                     // 受信したデータの内容に応じて
            case MODE_RESP:                             // データ取得指示に対する応答
            case MODE_GPIN:                             // 子機XBeeの自動送信の受信
                if(xbee_result.GPI.PORT.D1 == 0){       // DIOポート1がLレベルの時
                    printf("D1=0 Ring\n");              // 表示
                    bell(dev_bell,3);                   // ブザーを3回鳴らす
                }else printf("D1=1\n");                 // 表示
                bell(dev_bell,0);                       // ブザー音を消す
                break;
        }
    }
}
