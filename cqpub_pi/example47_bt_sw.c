/***************************************************************************************
Bluetoothモジュール RN-42XVPのGPIOポート3と7に接続したスイッチ状態を取得します。

                                                Copyright (c) 2013-2016 Wataru KUNINO
                                                http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#include "../libs/bt_rn42.c"
#include "../libs/kbhit.c"
char rx_data[RX_MAX];                               // 受信データの格納用の文字列変数

int main(int argc,char **argv){
    int in,i;
    
    if(argc != 2 || strlen(argv[1]) != 17){
        fprintf(stderr,"usage: %s xx:xx:xx:xx:xx:xx\n",argv[0]);
        return -1;
    }
    printf("example 47 Bluetooth SW from RN-42\n");
    bt_init(argv[1]);                           // Bluetooth RN-42接続の開始
    printf("CONNECTED\nHit any key to EXIT\n");
    while( bt_cmd_mode('$') ){                  // リモートコマンドモードへの移行
    //  bt_cmd("S@,8800");                      // GPIOポート3と7を入力に設定(初期値)
        bt_cmd("G&");                           // GPIOポートの読み取り
        for(i=0;i<2;i++){
            in = rx_data[1-i];                  // 文字コードを変数inに代入
            if( in >= '0' && in <= '9' ) in -= '0';        // 0～9なら数値へ変換
            else if( in >= 'A' && in <= 'F' ) in -= 'A'-10;// A～Fなら16進数値へ変換
            else in = 0;
            in = (in>>3) & 0x01;                // 変数inに入力値を代入。bitRead(in,3)
            if(i==0){
                printf("IN3=%d, ",in);
            }else{
                printf("IN7=%d\n",in);
            }
        }
        bt_cmd("---");                          // コマンドモードの解除
        sleep(5);
        if( kbhit() ) break;
    }
    bt_close();                                 // 切断処理
    return 0;
}
