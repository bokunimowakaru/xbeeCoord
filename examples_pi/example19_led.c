/***************************************************************************************
LEDをリモート制御する④通信の暗号化

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){
    
    byte com=0xB0;                      // シリアル(USB)、拡張IOコネクタの場合は0xA0
    byte dev[8];                        // XBee子機デバイスのアドレス
    
    /* 初期化 */
    if(argc==2) com += atoi(argv[1]);   // 引数があれば変数comに代入する
    xbee_init( com );                   // XBee用COMポートの初期化
    /* 暗号化有効 */
    if( xbee_atee_on("password") <= 1){ // 暗号化ON設定。passwordは16文字まで
        printf("Encryption On\n");      // "password" -> 70617373776F7264
    }else{
        printf("Encryption Error\n");   // 暗号化エラー表示
        exit(-1); 
    }
    /* ペアリング */
    printf("XBee in Commissioning\n");  // 待ち受け中の表示
    if(xbee_atnj(30)){                  // デバイスの参加受け入れを開始（最大30秒間）
        printf("Found a Device\n");     // XBee子機デバイスの発見表示
        xbee_from( dev );               // 見つけたデバイスのアドレスを変数devに取込む
        xbee_ratnj(dev,0);              // 子機に対して孫機の受け入れ制限を設定
    }else{
        printf("No Devices\n");         // エラー時の表示
        exit(-1);                       // 異常終了
    }
    /* LEDの点滅（暗号化） */
    while(1){                           // 5回の繰り返し処理
        xbee_gpo(dev, 4, 1);            // リモートXBeeのポート4を出力'H'に設定する
        delay( 1000 );                  // 1000ms(1秒間)の待ち
        xbee_gpo(dev, 4, 0);            // リモートXBeeのポート4を出力'L'に設定する
        delay( 1000 );                  // 1000ms(1秒間)の待ち
    }
}
