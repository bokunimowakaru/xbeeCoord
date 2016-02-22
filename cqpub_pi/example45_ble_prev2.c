/***************************************************************************************
BLEタグを使った盗難防止システム(徐々に遠ざかるケースに対応)

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
#define ALART_RSSI_OFFSET     10                                // 警告1:RSSI低下レベル
#define ALART_RSSI_TIMES       4                                // 警告2:RSSI低下回数

int main(int argc,char **argv){
    char  s[256];
    FILE  *fp;
    time_t timer;                                                       // タイマー用
    time_t trig=0;                                                      // 取得時刻保持
    int rssi=-999;                                                      // 受信レベル
    int rssi_times=0;                                                   // 受信低下回数
    int mac_f=0;                                                        // MAC一致フラグ
    int i;
    char c;
    
    if(argc != 2 || checkMac(argv[1]) ){                                // 書式の確認
        fprintf(stderr,"usage: %s MAC_Address\n",argv[0]);              // 入力誤り表示
        return -1;                                                      // 終了
    }
    
    printf("Example 45 BLE Prev 2 (0:off 1:LED 2:Alert Q:Quit)\n");     // 起動表示
    while(1){
        if( kbhit() ){                                                  // キー入力時
            c=getchar();                                                // 入力キー取得
            if(c=='q' || c=='Q') break;                                 // 「Q」で終了
            if(c!='1' && c!='2') c='0';                                 // 1,2以外で0に
            sprintf(s,"sudo gatttool -b %s --char-write -a 13 -n 0%c",argv[1],c);                                                          // コマンド作成
            system(s);                                                  // コマンド実行
        }
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
        if( i < rssi ) rssi_times++; else rssi_times = 0;               // 劣化検出
        if( i+ALART_RSSI_OFFSET < rssi ) rssi_times=99;                 // 著しい劣化
        rssi=i;
        printf("Recieved BLE Beacon, RSSI=%d, ",rssi);                  // 受信,RSSI表示
        printf("Times Degraded=%d\n",rssi_times);                       // 劣化回数表示
        if( rssi_times >= ALART_RSSI_TIMES){                            // 連続劣化回数
            printf("Send Alart!\n");                                    // 警告を送信
            sprintf(s,"sudo gatttool -b %s --char-write -a 13 -n 02",argv[1]);
            system(s);                                                  // コマンド実行
        }
        time(&timer);                                                   // 現在時刻取得
        if( timer >= trig && rssi_times < ALART_RSSI_TIMES){            // 定期時刻時
            sprintf(s,"sudo gatttool -b %s --char-read -a 0x0003 > /dev/null",argv[1]);
            system(s);                                                  // コマンド実行
            trig = timer + FORCE_INTERVAL;                              // 次trigを設定
        }
    }
    return 0;
}
