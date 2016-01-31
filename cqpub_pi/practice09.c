/***************************************************************************************
コラム 11：xbee_init関数について

                                                       Copyright (c) 2016 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
int main(int argc,char **argv){
    byte com=0xB0;                      // シリアル(USB)、拡張IOコネクタの場合は0xA0
    if(argc==2){                        // 引数があれば
        printf("argc=%d, ",argc);       // argcには引数の数+1が代入されている
        printf("argv[0]=%s, ",argv[0]); // argv[0]には実行コマンド名が代入されている
        printf("argv[1]=%s＼n ",argv[1]); // argv[1]には引数が代入されている
        com += atoi(argv[1]);           // 変数comに引数値を加算する
    }
    xbee_init( com );                   // XBee用COMポートの初期化
    return 0;                           // 関数mainの正常終了(0)
}
