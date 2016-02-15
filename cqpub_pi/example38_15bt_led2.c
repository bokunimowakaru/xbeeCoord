/**************************************************************************************
Bluetooth LED powered by IchigoJam 用 コントローラ (Raspbery Pi)

                                                       Copyright (c) 2015 Wataru KUNINO
***************************************************************************************/
#include "../libs/15term.c"
#include "../libs/kbhit.c"

int main(int argc,char **argv){
    char c;                                         // キー入力用の文字変数c
    int loop=1;                                     // 変数loop(0:ループ終了)
    
    if(argc != 2 || strlen(argv[1]) != 17){
        fprintf(stderr,"usage: %s xx:xx:xx:xx:xx:xx\n",argv[0]);
        return -1;
    }
    printf("example 38 Bluetooth LED for IchigoJam\n");
    if(open_rfcomm(argv[1]) < 0){                   // Bluetooth接続の開始
        printf("Bluetooth Open ERROR\n");
        return -1;
    }
    printf("CONNECTED\n[0]:LED OFF, [1]-[4]:LED ON, [q]:EXIT\n");
    write(ComFd, "\x1b\x10 CLS\n", 7);              // IchigoJamの画面制御
    while(loop){
        while( !kbhit() );                          // キーボードから入力があるまで待つ
        c=getchar();                                // 入力された文字を変数cへ代入
        printf("-> LED [%c]\n",c);                  // 「LED [数字]」を表示
        switch(c){
            case '0':                               // 「0」が入力された時
                write(ComFd, "OUT 0\n", 6);         // IchigoJamへ全LED消灯命令を送信
                break;
            case '1':                               // 「1」が入力された時
                write(ComFd, "OUT 1+64\n", 9);      // IchigoJamへLED点灯命令を送信する
                break;
            case '2':                               // 「2」が入力された時
                write(ComFd, "OUT 3+64\n", 9);      // IchigoJamへLED点灯命令を送信する
                break;
            case '3':                               // 「3」が入力された時
                write(ComFd, "OUT 7+64\n", 9);      // IchigoJamへLED点灯命令を送信する
                break;
            case '4':                               // 「4」が入力された時
                write(ComFd, "OUT 15+64\n",10);     // IchigoJamへLED点灯命令を送信する
                break;
            case 'q': case 'Q':                     // 「q」が入力された時
                printf("-> EXIT\n");                // 「EXIT」を表示
                loop=0;                             // whileを抜けるためにloopを0に設定
                break;
        }
        usleep(200000);                             // 200msの(IchigoJam処理)待ち時間
        write(ComFd, "BEEP\n", 5);                  // BEEP音の送信
    }
    printf("\nDONE\n");
    close_rfcomm();
    return 0;
}
