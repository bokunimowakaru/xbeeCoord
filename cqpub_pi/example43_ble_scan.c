/***************************************************************************************
BLEタグのビーコンを受信する

                                                        Copyright (c) 2016 Wataru KUNINO
***************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 											// sleepに使用

int main(int argc,char **argv){
	char  s[256];
	FILE  *fp;
	int i;
	
    if(argc != 2 || strlen(argv[1]) != 17){
        fprintf(stderr,"usage: %s MAC_Address\n",argv[0]);		// 入力誤り表示
        return -1;
    }
	system("sudo /usr/bin/hcitool lescan --passive &");
	if((fp=popen("sudo hcidump","r"))==NULL){
		fprintf(stderr,"error!!!\n");
		return -1;
	}
	for(i=0;i<10;i++){
		fgets(s,512,fp);
		printf("%s",s);
	}
	pclose(fp);
	
	system("sudo /usr/bin/hcitool cmd 08 000c 00 01");
	system("sudo kill `pidof hcitool`");
	return 0;
}
