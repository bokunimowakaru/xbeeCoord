/***************************************************************************************
子機XBeeのスイッチ状態をリモートで取得する

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // データ要求間隔(約10～20msの倍数)

int main(int argc,char **argv){

    byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
    byte value;                                 // 受信値
    byte dev[8];                                // XBee子機デバイスのアドレス
    byte trig=0xFF;                             // 子機へデータ要求するタイミング調整用
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに値を加算する
    xbee_init( com );                           // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                          // 子機XBeeデバイスを常に参加受け入れ
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示
    
    while(1){
        /* データ送信 */
        if( trig == 0 ){
            xbee_force( dev );                  // 子機へデータ要求を送信
            trig = FORCE_INTERVAL;
        }
        if( trig != 0xFF ) trig--;              // 変数trigが0xFF以外の時に値を1減算

        /* データ受信(待ち受けて受信する) */
        xbee_rx_call( &xbee_result );           // データを受信
        switch( xbee_result.MODE ){             // 受信したデータの内容に応じて
            case MODE_RESP:                     // xbee_forceに対する応答の時
                value = xbee_result.GPI.PORT.D1;// D1ポートの値を変数valueに代入
                printf("Value =%d ",value);     // 変数valueの値を表示
                value = xbee_result.GPI.BYTE[0];// D7～D0ポートの値を変数valueに代入
                lcd_disp_bin( value );          // valueに入った値をバイナリで表示
                printf("\n");                   // 改行
                break;
            case MODE_IDNT:                     // 新しいデバイスを発見
                printf("Found a New Device\n");
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピー
                xbee_atnj(0);                   // 親機XBeeに子機の受け入れ制限を設定
                xbee_ratnj(dev,0);              // 子機に対して孫機の受け入れ制限を設定
                xbee_gpio_config(dev,1,DIN);    // 子機XBeeのポート1をデジタル入力に
                xbee_gpio_config(dev,2,DIN);    // 子機XBeeのポート2をデジタル入力に
                xbee_gpio_config(dev,3,DIN);    // 子機XBeeのポート3をデジタル入力に
                trig = 0;                       // 子機へデータ要求を開始
                break;
        }
    }
}
