/***************************************************************************************
Bluetoothモジュール RN-42XVPを搭載したArduino子機に室温と外気温を表示します。

                                                Copyright (c) 2013-2016 Wataru KUNINO
                                                http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#include "../libs/bt_rn42.c"
#include "../libs/kbhit.c"
#include <time.h>                                   // time,localtime用
#define FORCE_INTERVAL  3600                        // データ要求間隔(秒)
char rx_data[RX_MAX];                               // 受信データの格納用の文字列変数

int main(int argc,char **argv){
    int temp[3]={-99,-99,-99};                      // temp[0]室内,[1]最高,[2]最低
    time_t timer;                                   // タイマー変数の定義
    time_t trig=0;                                  // 取得タイミング保持用
    struct tm *time_st;                             // タイマーによる時刻格納用の構造体
    char  s[256];                                   // HTTP受信データ等の文字列保持用
    FILE  *fp;                                      // パイプ処理受信用・ファイル書込用
    int len;                                        // 文字長さ
    char c;                                         // 文字変数c
    char *p;                                        // 文字用ポインタ
    
    if(argc != 2 || strlen(argv[1]) != 17){
        fprintf(stderr,"usage: %s xx:xx:xx:xx:xx:xx\n",argv[0]);
        return -1;
    }
    printf("example 50 Temperature for Arduino + RN-42\n");
    bt_init(argv[1]);                               // Bluetooth RN-42接続の開始
    printf("CONNECTED\nPress [Q] to Quit.\n");
    while(1){
        time(&timer);                               // 現在の時刻を変数timerに取得する
        time_st = localtime(&timer);                // timer値を時刻に変換してtime_stへ
        if( timer%60 == 0 || trig ==0 ){            // 60秒毎、または Trigが0の時
            len = bt_cmd("\n\x1b");                 // Arduinoへ室温を問い合わせる
            if(len) temp[0]=atoi(rx_data);          // 受信した室温をtemp[0]に保持する
            strftime(s,17,"%H:%M",time_st);         // 文字列変数sに時刻を代入
            sprintf(s,"%s %d / %d",s,temp[1],temp[2]);  // 文字列変数sに温度を追加
            bt_cmd(s);                              // Arduinoへ送信
        }
        if( timer >= trig ){                        // 変数trigまで時刻が進んだとき
            fp=popen("curl -s rss.weather.yahoo.co.jp/rss/days/6200.xml|cut -d'<' -f17|cut -d'>' -f2","r");
            if(fp){
                while( !feof(fp) ) fgets(s,256,fp);
                pclose(fp);
                p=strchr(s,'-');
                if(strlen(p)>0) temp[1]=atoi(&p[1]);
                p=strchr(s,'/');
                if(strlen(p)>0) temp[2]=atoi(&p[1]);
            }
            strftime(s,255,"%Y/%m/%d %H:%M:%S",time_st); // 時刻を代入
            printf("%s Temp.Hi=%d / Lo=%d Room=%d\n",s,temp[1],temp[2],temp[0]);
            fp=fopen("/var/www/html/index.html","w");
            if(fp){
                fprintf(fp,"<HTML>\n<meta http-equiv=\"refresh\" content=10>\n<h1>%s</h1>Temp.<br>\n",s);
                fprintf(fp,"Hi= %d<br>Lo= %d<br>Room= %d<br>\n</HTML>\n",temp[1],temp[2],temp[0]);
                fclose(fp);
            }
            trig = timer + FORCE_INTERVAL;          // 次回の時刻を変数trigを設定
        }
        if( kbhit() ){                              // キーボードから入力があるまで待つ
            c=getchar();                            // 入力された文字を変数cへ代入
            if( c=='q' ) break;                     // 「Q」キーが押された場合に終了
            trig=0;                                 // 情報取得の実行
        }
        if( bt_rx() ) trig=0;                       // Arduinoからキー受信時に情報取得
    }
    bt_close();                                     // 切断処理
    return 0;
}
