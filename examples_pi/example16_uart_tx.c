/***************************************************************************************
子機XBeeのUARTからシリアル情報を送信する

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    char s[32];                                 // 文字入力用
    byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
    byte dev[]={0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};
                                                // 宛先XBeeアドレス(ブロードキャスト)

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに値を加算する
    xbee_init( com );                           // XBee用COMポートの初期化
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示
    if(xbee_atnj(30) != 0){                     // デバイスの参加受け入れを開始
        printf("Found a Device\n");             // XBee子機デバイスの発見表示
        xbee_from( dev );                       // 見つけた子機のアドレスを変数devへ
        xbee_ratnj(dev,0);                      // 子機に対して孫機の受け入れ制限を設定
    }else{                                      // 子機が見つからなかった場合
        printf("no Devices\n");                 // 見つからなかったことを表示
        exit(-1);                               // 異常終了
    }
    
    while(1){
        /* データ送信 */
        printf("TX-> ");                        // 文字入力欄の表示
        fgets(s, 32, stdin);                    // 入力文字を変数sに代入
        xbee_uart( dev , s );                   // 変数sの文字を送信
    }
}
