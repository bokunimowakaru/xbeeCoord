/**************************************************************************************
Ichigo Term for Raspberry Pi

                                                       Copyright (c) 2015 Wataru KUNINO
***************************************************************************************/
#include <stdio.h>                                  // 標準入出力用
#include <fcntl.h>                                  // シリアル通信用(Fd制御)
#include <termios.h>                                // シリアル通信用(端末IF)
#include <string.h>                                 // strncmp,bzero用
#include <sys/time.h>                               // fd_set,select用
#include <ctype.h>                                  // isprint,isdigit用
#include <unistd.h>                                 // usleep用
#include "../libs/kbhit.c"
static int ComFd;                                   // シリアル用ファイルディスクリプタ
static struct termios ComTio_Bk;                    // 現シリアル端末設定保持用の構造体

int open_serial_port(){
    struct termios ComTio;                          // シリアル端末設定用の構造体変数
    speed_t speed = B115200;                        // 通信速度の設定
    char modem_dev[15]="/dev/ttyUSBx";              // シリアルポートの初期値
    int i;
    for(i=12;i>=-1;i--){
        if(i>=10) snprintf(&modem_dev[5],8,"rfcomm%1d",i-10);   // ポート探索(rfcomm0-2)
        else if(i>=0) snprintf(&modem_dev[5],8,"ttyUSB%1d",i);  // ポート探索(USB0～9)
        else snprintf(&modem_dev[5],8,"ttyAMA0");   // 拡張IOのUART端子に設定
        ComFd=open(modem_dev, O_RDWR|O_NONBLOCK);   // シリアルポートのオープン
        if(ComFd >= 0){                             // オープン成功時
            printf("com=%s\n",modem_dev);           // 成功したシリアルポートを表示
            tcgetattr(ComFd, &ComTio_Bk);           // 現在のシリアル端末設定状態を保存
            ComTio.c_iflag = 0;                     // シリアル入力設定の初期化
            ComTio.c_oflag = 0;                     // シリアル出力設定の初期化
            ComTio.c_cflag = CLOCAL|CREAD|CS8;      // シリアル制御設定の初期化
            ComTio.c_lflag = 0;                     // シリアルローカル設定の初期化
            bzero(ComTio.c_cc,sizeof(ComTio.c_cc)); // シリアル特殊文字設定の初期化
            cfsetispeed(&ComTio, speed);            // シリアル入力の通信速度の設定
            cfsetospeed(&ComTio, speed);            // シリアル出力の通信速度の設定
            ComTio.c_cc[VMIN] = 0;                  // リード待ち容量0バイト(待たない)
            ComTio.c_cc[VTIME] = 0;                 // リード待ち時間0.0秒(待たない)
            tcsetattr(ComFd, TCSANOW, &ComTio);     // シリアル端末に設定
            break;                                  // forループを抜ける
        }
    }
    return ComFd;
}

char read_serial_port(void){
    char c='\0';                                    // シリアル受信した文字の代入用
    fd_set ComReadFds;                              // select命令用構造体ComReadFdを定義
    struct timeval tv;                              // タイムアウト値の保持用
    FD_ZERO(&ComReadFds);                           // ComReadFdの初期化
    FD_SET(ComFd, &ComReadFds);                     // ファイルディスクリプタを設定
    tv.tv_sec=0; tv.tv_usec=10000;                  // 受信のタイムアウト設定(10ms)
    if(select(ComFd+1, &ComReadFds, 0, 0, &tv)) read(ComFd, &c, 1); // データを受信
    usleep(5000);                                   // 5msの(IchigoJam処理)待ち時間
    return c;                                       // 戻り値＝受信データ(文字変数c)
}

int close_serial_port(void){
    tcsetattr(ComFd, TCSANOW, &ComTio_Bk);
    return close(ComFd);
}

int main(){
    char s[32];                                     // 文字データ用
    int len=0;                                      // 文字長
    char c;                                         // 文字入力用の文字変数
    int ctrl=0;                                     // 制御用 0:先頭, -1～-3:「-」入力数
                                                    //        1:コマンド, 2:プログラム
    printf("Ichigo Term for Raspberry Pi\n");
    if(open_serial_port() <= 0){
        printf("UART OPEN ERROR\n");
        return -1;
    }
    printf("CONNECTED\nHit '---' to exit.\nTX-> ");
    write(ComFd, "\x1b\x10", 2 );                   // IchigoJamの画面制御
    while(1){
        if( kbhit() ){
            c=getchar();                            // キーボードからの文字入力
            s[len]=c;                               // 入力文字を保持
            if(len < 31) len++;                     // 最大長未満のときに文字長に1を加算
            write(ComFd, &c, 1 );                   // IchigoJamへ送信
            if( ctrl<=0 && c =='-' ){               // 先頭かつ入力された文字が「-」の時
                ctrl--;                             // ctrl値を1減算
                if(ctrl <= -3) break;               // 「-」が3回入力された場合に終了
            }else if(ctrl==0){
                if(isdigit(c)) ctrl=2; else ctrl=1; // 先頭に数値でプログラムと判定し、
            }                                       // そうで無い時はコマンド入力と判定
            if(c=='\n'){                            // 入力文字が改行の時
                if(ctrl==2 || len==1) printf("TX-> ");  // 無入力とプログラム入力時
                ctrl=0;                             // 入力文字状態を「先頭」にセット
                len=0;
                usleep(250000);                     // 250msの(IchigoJam処理)待ち時間
            }
            usleep(25000);                          // 25msの(IchigoJam処理)待ち時間
        }
        c=read_serial_port();                       // シリアルからデータを受信
        if(c){
            printf("\nRX<- ");                      // 受信を識別するための表示
            while(c){
                if( isprint(c) ) printf("%c",c);    // 表示可能な文字の時に表示する
                if( c=='\n' ) printf("\n     ");    // 改行時に改行と5文字インデントする
                if( c=='\t' ) printf(", ");         // タブの時にカンマで区切る
                c=read_serial_port();               // シリアルからデータを受信
            }
            s[len]='\0';
            printf("\nTX-> %s",s);                  // キーボードの入力待ち表示
        }
    }
    printf("\nDONE\n");
    close_serial_port();
    return 0;
}
