/***************************************************************************************
BLEタグを使った盗難防止システム(シンプル版)

ご注意：必ずしも盗難を防止できるシステムではありません。
出版社および当方はいかなる損害に対しても、一切の責任を負いません。

                                                        Copyright (c) 2016 Wataru KUNINO
***************************************************************************************/
#include <stdio.h>                                              // 標準入出力ライブラリ
#include <stdlib.h>                                             // system命令に使用
#include <string.h>                                             // strncmp命令に使用
#include <time.h>                                               // time命令time_tに使用
#include "../libs/checkMac.c"                                   // MACアドレスの書式確認
#include "../libs/kbhit.c"
#define FORCE_INTERVAL        60                                // データ取得間隔(秒)

int main(int argc,char **argv){
    char  s[256];
    FILE  *fp;
    time_t timer;                                                       // タイマー用
    time_t trig=0;                                                      // 取得時刻保持
    int rssi=-999;                                                      // 受信レベル
    int mac_f=0;                                                        // MAC一致フラグ
    int i;
    
    if(argc != 2 || checkMac(argv[1]) ){                                // 書式の確認
        fprintf(stderr,"usage: %s MAC_Address\n",argv[0]);              // 入力誤り表示
        return -1;                                                      // 終了
    }
    printf("Example 45 BLE Prev\n");                                    // 起動表示
    while(1){
        if( kbhit() ) if( getchar() =='q' ) break;                      // 「Q」で終了
        system("sudo hcitool lescan --passive > /dev/null &");          // LEスキャン
        fp=popen("sudo hcidump","r");                                   // hcidump実行
        while(fp){
            fgets(s,256,fp);                                            // hcidumpから
            if( strncmp(&s[4],"LE Advertising Report",21)==0 ){         // ビーコン判定
                for(i=0;i<6;i++){
                    fgets(s,256,fp);                                    // データの取得
                    if(i==1){                                           // 2行目の時
                        if(strncmp(&s[13],argv[1],17)==0) mac_f=1;      // MAC一致を確認
                        else mac_f=0;                                   // mac_fに代入
                    }
                }
                if(mac_f) break;                                        // whileを抜ける
            }                                                           // (得られるまで
        }                                                               //     抜けない)
        pclose(fp);                                                     // popenを閉じる
        system("sudo hcitool cmd 08 000c 00 01 > /dev/null");           // LEスキャン止
        system("sudo kill `pidof hcitool` > /dev/null");                // プロセス停止
        i=atoi(&s[12]);                                                 // RSSI値をiへ
        printf("Recieved BLE Beacon, RSSI=%d\n",i);                     // 受信,RSSI表示
        if( i+10 < rssi ){                                              // 大幅に低下時
            printf("Send Alart!\n");                                    // 警告を送信
            sprintf(s,"sudo gatttool -b %s --char-write -a 13 -n 02",argv[1]);
            system(s);                                                  // コマンド実行
        }
        rssi=i;                                                         // RSSI値を保持
        time(&timer);                                                   // 現在時刻取得
        if( timer >= trig ){                                            // 定期時刻時
            sprintf(s,"sudo gatttool -b %s --char-read -a 0x0003 > /dev/null",argv[1]);
            system(s);                                                  // コマンド実行
            trig = timer + FORCE_INTERVAL;                              // 次trigを設定
        }
    }
    return 0;
}
