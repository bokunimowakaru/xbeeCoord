/***************************************************************************************
Bluetoothモジュール RN-42XVPのGPIOを制御してGPIOポート9と10に接続したLEDを点滅します。
リモートで使用するための各種設定とペアリングも行います。

                                                Copyright (c) 2013-2016 Wataru KUNINO
                                                http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#include "../libs/bt_rn42.c"
#include "../libs/kbhit.c"
char rx_data[RX_MAX];                               // 受信データの格納用の文字列変数

int main(int argc,char **argv){
    char c;                                         // キー入力用の文字変数c
    
    if(argc != 2 || strlen(argv[1]) != 17){
        fprintf(stderr,"usage: %s xx:xx:xx:xx:xx:xx\n",argv[0]);
        return -1;
    }
    printf("example 46 Bluetooth LED for RN-42\n");
    printf("[0]:LED OFF, [1]-[2]:LED ON, [q]:EXIT\n");

    while(1){
        while( !kbhit() );                          // キーボードから入力があるまで待つ
        c=getchar();                                // 入力された文字を変数cへ代入
        printf("-> LED [%c]\n",c);                  // 「LED [数字]」を表示
        if( c=='q' ) break;                         // 「Q」キーが押された場合に終了
        bt_init(argv[1]);                               // Bluetooth RN-42接続の開始
        if( bt_cmd_mode('$') ){                     // リモートコマンドモードへの移行
            switch(c){
                case '0':                           // 「0」が入力された時
                    bt_cmd("S*,0600");              // GPIOポート9と10の出力をLowレベルに
                    break;
                case '1':                           // 「1」が入力された時
                    bt_cmd("S*,0202");              // GPIOポート9の出力をHighレベルに
                    break;
                case '2':                           // 「2」が入力された時
                    bt_cmd("S*,0404");              // GPIOポート10の出力をHighレベルに
                    break;
            }
            bt_cmd("---");                          // コマンドモードの解除
            printf("Done\n");
        }
        bt_close();                                 // 切断処理
    }
    return 0;
}
