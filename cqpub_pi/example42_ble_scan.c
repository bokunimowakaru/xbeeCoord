/***************************************************************************************
BLEタグのビーコンを受信する

                                                        Copyright (c) 2016 Wataru KUNINO
***************************************************************************************/
#include <stdio.h>                                              // 標準入出力ライブラリ
#include <stdlib.h>                                             // system命令に使用
#include <string.h>                                             // strncmp命令に使用

int main(){
    char  s[256];
    FILE  *fp;
    int i;
    
    system("sudo hcitool lescan --passive > /dev/null &");      // LEスキャンの実行
    fp=popen("sudo hcidump","r");                               // hcidumpの実行
    if( fp==NULL ){                                             // 開始できなかった時
        fprintf(stderr,"System Command Error!\n");              // エラー表示
        return -1;                                              // 終了
    }
    while(1){
        fgets(s,256,fp);                                        // hcidumpからデータ入力
        if( strncmp(&s[4],"LE Advertising Report",21)==0 ){     // ビーコン判定
            printf("Found BLE Beacon\n");                       // ビーコン受信表示
            for(i=0;i<6;i++){
                fgets(s,256,fp);                                // 受信データの取得
                printf("%s",s);                                 // 受信データの表示
            }
            break;
        }
    }
    pclose(fp);                                                 // popenを閉じる
    system("sudo hcitool cmd 08 000c 00 01 > /dev/null");       // LEスキャンの停止
    system("sudo kill `pidof hcitool` > /dev/null");            // プロセスの停止
    return 0;
}
