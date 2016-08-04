/***************************************************************************************
ネットワーク設定をリセットする

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte port=0;                                // シリアルCOMポート番号
    byte ret=0;

    if( argc==2 ){
        if( atoi(argv[1]) < 0 ){
            port = 0x9F + (byte)(-atoi(argv[1])) ;
        }else  if( ( argv[1][0]=='b' || argv[1][0]=='B' )&& argv[1][1]!='\0' ){
            port = 0xB0 + ( argv[1][1] - '0');
        }else  if( ( argv[1][0]=='a' || argv[1][0]=='A' )&& argv[1][1]!='\0' ){
            port = 0xA0 + ( argv[1][1] - '0');
        }else port = (byte)(atoi(argv[1]));
    }
    xbee_init( port );                          // XBee用COMポートの初期化
    printf("Resetting Network Setting\n");
    ret = xbee_atcb(4);                         // ネットワーク設定をリセットする
    if( ret==00 ){                              // 暗号化をオフにする
        printf("SUCCESS\n");                    // 表示
    }else printf("Error\n");                    // エラー表示
    return( ret );
}
