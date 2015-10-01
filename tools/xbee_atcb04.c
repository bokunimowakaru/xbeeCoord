/***************************************************************************************
ネットワーク設定をリセットする

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte com=0;                                 // シリアルCOMポート番号
    byte ret=0;

    if(argc==2) com=(byte)atoi(argv[1]);        // 引数があれば変数comに代入する
    xbee_init( com );                           // XBee用COMポートの初期化
    ret = xbee_atcb(4);                         // ネットワーク設定をリセットする
    if( ret==00 ){                              // 暗号化をオフにする
        printf("OK\n");                         // OK表示
    }else printf("Error\n");                    // エラー表示
    return( ret );
}
