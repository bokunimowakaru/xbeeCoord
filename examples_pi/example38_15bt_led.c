/**************************************************************************************
Ichigo Term for Raspberry Pi

                                                       Copyright (c) 2015 Wataru KUNINO
***************************************************************************************/
#include "../libs/15term.c"
#include "../libs/kbhit.c"

int main(){
    char c;                                         // キー入力用の文字変数c
    int loop=1;                                     // 変数loop(0:ループ終了)
    
    printf("example 38 Bluetooth LED for IchigoJam\n");
    if(open_serial_port() <= 0){
        printf("UART OPEN ERROR\n");
        return -1;
    }
    printf("CONNECTED\n'0':LED OFF, '1':LED ON, 'q':EXIT\n");
    write(ComFd, "\x1b\x10 CLS\n", 7);              // IchigoJamの画面制御
    while(loop){
        while( !kbhit() );                          // キーボードから入力があるまで待つ
        c=getchar();                                // 入力された文字を変数cへ代入
        switch(c){
            case '0':                               // 「0」が入力された時
                printf("LED 0\n");                  // 「LED 0」を表示
                write(ComFd, "LED 0\n", 6);         // IchigoJamへLED消灯命令を送信する
                break;
            case '1':                               // 「1」が入力された時
                printf("LED 1\n");                  // 「LED 1」を表示
                write(ComFd, "LED 1\n", 6);         // IchigoJamへLED点灯命令を送信する
                break;
            case 'q':                               // 「q」が入力された時
                printf("EXIT\n");                   // 「EXIT」を表示
                loop=0;                             // whileを抜けるためにloopを0に設定
                break;
        }
        usleep(100000);                             // 100msの(IchigoJam処理)待ち時間
        write(ComFd, "BEEP\n", 5);                  // BEEP音の送信
    }
    printf("\nDONE\n");
    close_serial_port();
    return 0;
}
