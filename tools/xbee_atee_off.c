/***************************************************************************************
暗号化をオフにする

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte com=0;                                 // シリアルCOMポート番号
    byte ret=0;

    if(argc==2) com=(byte)atoi(argv[1]);        // 引数があれば変数comに代入する
    xbee_init( com );                           // XBee用COMポートの初期化
    ret = xbee_atee_off();
    if( ret <= 1 ){                             // 暗号化をオフにする
        printf("Encryption Off\n");             // 表示
    }else printf("Error(%02X)\n",ret);          // エラー表示
    return( ret );
}
