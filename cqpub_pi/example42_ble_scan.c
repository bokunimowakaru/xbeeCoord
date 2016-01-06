/***************************************************************************************
BLEタグのビーコンを受信する

                                                        Copyright (c) 2016 Wataru KUNINO
***************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 											// sleepに使用

int main(){
	char  s[256];
	FILE  *fp;
	int i;
	
	system("sudo hcitool lescan --passive > /dev/null &");		// LEスキャンの実行
	if((fp=popen("sudo hcidump","r"))==NULL){					// hcidumpの実行
		fprintf(stderr,"error!!!\n");
		return -1;
	}
	while(1){
		fgets(s,512,fp);										// hcidumpからデータ入力
		if( strncmp(&s[4],"LE Advertising Report",21)==0 ){		// ビーコン判定
			printf("Found BLE Beacon\n");						// ビーコン受信表示
			for(i=0;i<7;i++){
				fgets(s,512,fp);								// 受信データの取得
				printf("%s",s);									// 受信データの表示
			}
			break;
		}
	}
	pclose(fp);
	system("sudo hcitool cmd 08 000c 00 01 > /dev/null");		// LEスキャンの停止
	system("sudo kill `pidof hcitool` > /dev/null");			// プロセスの停止
	return 0;
}