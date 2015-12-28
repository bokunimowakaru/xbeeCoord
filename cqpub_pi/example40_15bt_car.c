/**************************************************************************************
IchigoJam搭載 ワイヤレス・モータ車用 コントローラ (Raspberry Pi)

                                                       Copyright (c) 2015 Wataru KUNINO
***************************************************************************************/
#include "../libs/15term.c"
#include "../libs/kbhit.c"

int main(){
    char mac[]="00:06:66:xx:xx:xx";                 // 子機のMACアドレス
    char c;                                         // キー入力用の文字変数c
    int loop=1;                                     // 変数loop(0:ループ終了)
    struct termios termKey,termKey_bk;              // キー入力のエコーを止める処理用
    
    printf("example 40 Bluetooth Motor Car for IchigoJam\n");
    if(open_rfcomm(mac) <= 0){                      // Bluetooth接続の開始
        printf("Bluetooth Open ERROR\n");
        return -1;
    }
    printf("CONNECTED\n[LEFT] <-> [RIGHT], [UP]:Speed, [DOWN]:Slow [Q]:Quit\n");
    tcgetattr(0, &termKey);                         // 現在の標準入力端末設定を読み込み
    tcgetattr(0, &termKey_bk);                      // 同じものを別の構造体に保存
    termKey.c_lflag &= ~ECHO;                       // キー入力のエコーを止める処理
    tcsetattr(0, TCSANOW, &termKey);                // ローカルエコー停止の実行
    while(loop){
        if(kbhit()){                                // キーボードから入力があった時
            c=getchar();                            // 入力された文字を変数cへ代入
            switch(c){
                case 0x44:                          // 「左」が入力された時
                    c=(char)28; printf("[L]");      // IchigoJam文字コード 28
                    break;
                case 0x43:                          // 「右」が入力された時
                    c=(char)29; printf("[R]");      // IchigoJam文字コード 29
                    break;
                case 0x41:                          // 「上」が入力された時
                    c=(char)30; printf("[U]");      // IchigoJam文字コード 30
                    break;
                case 0x42:                          // 「下」が入力された時
                    c=(char)31; printf("[D]");      // IchigoJam文字コード 31
                    break;
                case '\n':                          // 「Enter」が入力された時
                    c=0; printf("\n[ANA]\n");       // 方向入力では無い
                    write(ComFd, "\x1b\x10", 2);    // エスケープを送信する
                    usleep(50000);                  // 50msの(IchigoJam処理)待ち時間
                    write(ComFd, "?ANA(2)\n",8);    // IchigoJamへ「?ANA(2)」を送信する
                    break;
                case ' ':                           // 「スペース」が入力された時
                    c=0; printf("\n[LRUN]\n");      // 方向入力では無い
                    write(ComFd, "\x1b\x10", 2);    // エスケープを送信する
                    usleep(50000);                  // 50msの(IchigoJam処理)待ち時間
                    write(ComFd, "LRUN0\n",6);      // IchigoJamへ「LRUN0」を送信する
                    break;
                case 'q': case 'Q':                 // 「q」が入力された時
                    c=0; printf("\n[QUIT]\n");      // 方向入力では無い
                    write(ComFd, "\x1b\x10", 2);    // エスケープを送信する
                    for(loop=3;loop>0;loop--){
                        usleep(200000);             // 200msの(IchigoJam処理)待ち時間
                        write(ComFd, "OUT0\n",5);   //「OUT0」を3回送信
                    }                               // whileを抜けるためにloopを0に設定
                    break;
                default:                            // 以上のcaseに当てはまらない時
                    c=0;                            // 方向入力では無い
            }
            if(c) write(ComFd, &c, 1);              // IchigoJamへ方向キーを送信
        }
        c=read_serial_port();                       // シリアルからデータを受信
        if(c) printf("%c",c);                       // 受信を表示
    }
    printf("\nDONE\n");
    tcsetattr(0, TCSANOW, &termKey_bk);             // ローカルエコー停止の解除
    close_rfcomm();
    return 0;
}
