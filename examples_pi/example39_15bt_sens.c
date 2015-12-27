/**************************************************************************************
Ichigo Term for Raspberry Pi

                                                       Copyright (c) 2015 Wataru KUNINO
***************************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <ctype.h>
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
    return c;                                       // 戻り値＝受信データ(文字変数c)
}

int close_serial_port(void){
    tcsetattr(ComFd, TCSANOW, &ComTio_Bk);
    return close(ComFd);
}

int main(){
    char c;                                         // 文字入力用の文字変数
    char s[32];                                     // 送信データ用の文字列変数
    int len=0;                                      // 送信データ長

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
            s[len]=c;                               // 文字列変数sに入力文字を代入する
            len++;                                  // 文字長を一つ増やす
            s[len]='\0';                            // 文字列の終了を表す\0を代入する
            if( !isprint(c) || len >= 31 ){         // 制御コード又は文字長が31文字の時
                write(ComFd, s, len );              // IchigoJamへ文字列変数sを送信
                if(isdigit(s[0]))printf("     ");   // 先頭が行番号の時にインデントする
                len=0;                              // 文字長を0にリセットする
                s[0]='\0';                          // 文字列の初期化
            }
            if(strncmp(s,"---",3)==0) break;        // 「---」が入力された場合に終了
        }
        c=read_serial_port();                       // シリアルからデータを受信
        if(c){
            printf("\nRX<- ");                      // 受信を識別するための表示
            while(c){
                if( isprint(c) ) printf("%c",c);    // 表示可能な文字の時に表示する
                if( c=='\n' ) printf("\n     ");    // 改行時に改行と5文字インデントする
                c=read_serial_port();               // シリアルからデータを受信
            }
            printf("\nTX-> %s",s);                  // キーボードの入力待ち表示
        }
    }
    printf("\nDONE\n");
    close_serial_port();
    return 0;
}
