/**************************************************************************************
Bluetooth Sensor powered by IchigoJam 用 コントローラ (Raspbery Pi)

                                                       Copyright (c) 2015 Wataru KUNINO
***************************************************************************************/
#include "../libs/15term.c"
#include "../libs/kbhit.c"
#include <time.h>                                   // time,localtime用
#define FORCE_INTERVAL  5                           // データ要求間隔(秒)
#define S_MAX           256                         // 文字列変数sの最大容量(255文字)

int main(){
    char mac[]="00:06:66:61:E6:81";                 // 子機のMACアドレス
    time_t timer;                                   // タイマー変数の定義
    time_t trig=0;                                  // 取得タイミング保持用
    struct tm *time_st;                             // タイマーによる時刻格納用の構造体
    char c;                                         // 文字変数
    char s[S_MAX];                                  // 文字列用の変数
    int len=0;                                      // 受信文字長

    printf("example 39 Bluetooth Sensor for IchigoJam\n");
    if(open_rfcomm(mac) < 0){                       // Bluetooth接続の開始
        printf("Bluetooth Open ERROR\n");
        return -1;
    }
    printf("CONNECTED\nHit any key to exit.\n");
    write(ComFd, "\x1b\x10 CLS\n", 7);              // IchigoJamの画面制御
    usleep(250000);                                 // 250msの(IchigoJam処理)待ち時間
    write(ComFd, "ifVer()>11006uart1\n", 19);       // IchigoJamの送信モード設定
    while(1){
        time(&timer);                               // 現在の時刻を変数timerに取得する
        time_st = localtime(&timer);                // timer値を時刻に変換してtime_stへ

        if( timer >= trig ){                        // 変数trigまで時刻が進んだとき
            write(ComFd, "? ANA(2)\n", 9);          // アナログ入力の取得命令を送信
            trig = timer + FORCE_INTERVAL;          // 次回の時刻を変数trigを設定
        }

        c=read_serial_port();                       // シリアルからデータを受信
        if( c ){                                    // 受信データ有
            if(len==0){
                strftime(s,S_MAX,"%Y/%m/%d, %H:%M:%S, ", time_st);  // 時刻→文字列
                len=strlen(s);                      // 時刻表示容量を代入(22バイト)
            }
            if(c=='\n'){                            // 改行コードの時
                if( strncmp(&s[22],"OK",2) ){       // 受信文字が「OK」では「無い」時
                    s[len]='\0';                    // 文字列の終端を追加
                    printf("%s, ",s);               // 文字列を表示
                    printf("(%d bytes)\n",len-22);  // 受信長を表示
                }
                s[0]='\0';                          // 文字列のクリア
                len=0;                              // 文字列長を0に
            }else{
                s[len]=c;                           // 文字列変数へ代入
                if(len < S_MAX-1) len++;            // 最大容量以下ならlenに1を加算
            }
        }
        if( kbhit() ) break;                        // キーボードからの入力があれば終了
    }
    printf("\nDONE\n");
    close_rfcomm();
    return 0;
}
