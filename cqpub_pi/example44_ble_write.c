/***************************************************************************************
BLEタグ内へ属性データを書き込む

                                                        Copyright (c) 2016 Wataru KUNINO
***************************************************************************************/
#include <stdio.h>                                              // 標準入出力ライブラリ
#include <stdlib.h>                                             // system命令に使用
#include <time.h>                                               // time命令time_tに使用
#include "../libs/a2hex.c"                                      // 16進数2桁を数値に変換
#include "../libs/checkMac.c"                                   // MACアドレスの書式確認
#include "../libs/kbhit.c"

#define FORCE_INTERVAL  10                                      // データ取得間隔(秒)

int main(int argc,char **argv){
    char  s[256];
    FILE  *fp;
    time_t timer;                                                       // タイマー用
    time_t trig=0;                                                      // 取得時刻保持
    int i;
    char c;
    
    if(argc != 2 || checkMac(argv[1]) ){                                // 書式の確認
        fprintf(stderr,"usage: %s MAC_Address\n",argv[0]);              // 入力誤り表示
        return -1;                                                      // 終了
    }
    
    printf("Example 44 BLE Write (0:off 1:LED 2:Alert Q:Quit)\n");      // 起動表示
    while(1){
        if( kbhit() ){                                                  // キー入力時
            c=getchar();                                                // 入力キー取得
            if(c=='q' || c=='Q') break;                                 // 「Q」で終了
            if(c!='1' && c!='2') c='0';                                 // 1,2以外で0に
            sprintf(s,"sudo gatttool -b %s --char-write -a 13 -n 0%c",argv[1],c);
            i=system(s);                                                // コマンド実行
            if(i){                                                      // エラー発生時
                fprintf(stderr,"System Command Error!\n");              // エラー表示
            }else{
                printf("\nSend Data '%c'\n",c);                         // 送信表示
            }
        }
        time(&timer);                                                   // 現在時刻取得
        if( timer >= trig ){                                            // 定期時刻時
            sprintf(s,"sudo gatttool -b %s --char-read -a 0x0003",argv[1]); // 命令作成
            fp = popen(s,"r");                                          // コマンド実行
            if( fp ){                                                   // 実行成功時
                i=(int)fgets(s,256,fp);                                 // データ取得
                pclose(fp);                                             // popenを閉じる
                if(i==0) return -1;                                     // 無応答時終了
                else{
                    printf("> ");                                       // 受信マーク
                    for(i=33;i<65;i+=3){                                // 34文字目から
                        c=a2hex(&s[i]);                                 // 受信値をcへ
                        if( isprint(c) ) putchar(c);                    // 文字表示
                        else break;                                     // 終端で抜ける
                    }
                    putchar('\n');                                      // 改行を出力
                }
            }
            trig = timer + FORCE_INTERVAL;                              // 次trigを設定
        }
    }
    return 0;
}
