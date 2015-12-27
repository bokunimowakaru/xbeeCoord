/***************************************************************************************
自作ブレッドボードを使ったリモートブザーの製作

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#include "../libs/kbhit.c"

void bell(byte *dev, byte c){
    byte i;
    for(i=0;i<c;i++){
        xbee_gpo( dev , 4 , 1 );                // 子機 devのポート4を1に設定(送信)
        xbee_gpo( dev , 4 , 0 );                // 子機 devのポート4を0に設定(送信)
    }
    xbee_gpo( dev , 4 , 0 );                    // 子機 devのポート4を0に設定(送信)
}

int main(int argc,char **argv){
    
    byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
    byte dev[8];                                // XBee子機デバイスのアドレス
    char c;                                     // 入力用

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに値を加算する
    xbee_init( com );                           // XBee用COMポートの初期化
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示
    if(xbee_atnj(30) != 0){                     // デバイスの参加受け入れを開始
        printf("Found a Device\n");             // XBee子機デバイスの発見表示
        xbee_from( dev );                       // 子機のアドレスを変数devへ
        bell( dev, 3);                          // ブザーを3回鳴らす
    }else{                                      // 子機が見つからなかった場合
        printf("no Devices\n");                 // 見つからなかったことを表示
        exit(-1);                               // 異常終了
    }
    printf("Hit any key >");

    while(1){                                   // 繰り返し処理
        if ( kbhit() ) {                        // キーボード入力の有無判定
            c = getchar();                      // 入力文字を変数sに代入
            c -= '0';                           // 入力文字を数字に変換
            if( c < 0 || c > 10 ) c = 10;       // 数字以外の時はcに10を代入
            bell(dev,c);                        // ブザーをc回鳴らす
        }
    }
}
