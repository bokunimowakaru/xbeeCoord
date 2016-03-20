/***************************************************************************************
ZIGBEE Device Type Switcher for XBee ZB S2C Series

実行すると以下のようなメッセージが表示されます。変更したいファームウェアの番号を入力すると、
自動的に設定を開始します。
途中でエラーが出たとしても最後に「SUCCESS」が表示されれば変更完了です。

ZIGBEE Device Type Switcher for XBee ZB S2C Series
00:Coordinator AT,  01:Router AT,  02:End Device AT
10:Coordinator API, 11:Router API, 12:End Device API
mode = 10⏎
Coordinator API

                                                        Copyright (c) 2016 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#include <ctype.h>

char read_serial_port(void){
    char c='\0';                                    // シリアル受信した文字の代入用
    fd_set ComReadFds;                              // select命令用構造体ComReadFdを定義
    struct timeval tv;                              // タイムアウト値の保持用
    FD_ZERO(&ComReadFds);                           // ComReadFdの初期化
    FD_SET(xbeeComFd, &ComReadFds);                     // ファイルディスクリプタを設定
    tv.tv_sec=0; tv.tv_usec=10000;                  // 受信のタイムアウト設定(10ms)
    if(select(xbeeComFd+1, &ComReadFds, 0, 0, &tv)) read(xbeeComFd, &c, 1); // データを受信
//    usleep(5000);                                   // 5msの待ち時間
    return c;                                       // 戻り値＝受信データ(文字変数c)
}

int read_serial_data(char *s,int size){
    int i=0;
    while(size>0){
        s[i]=read_serial_port();
        if( s[i]==0x00 || s[i]=='\n' || s[i]=='\r' ) break;
        i++; size--;
    }
    return i;
}

void print_s(char *s){
    int i=0;
    while(s[i]){
        if( isprint(s[i]) ) printf("%c",s[i]);
        s++;
    }
}

int send_at(int atmode,char *s){  // atmode=1:AT, atmode=0:API
    int ret=0;                          // 0:error 1:ok
    
    printf("AT command (%s) :",s);
    if(atmode){
        strcat(s,"\r");
        write(xbeeComFd,s,strlen(s));
        usleep(100000);
        read_serial_data(s,15);
        if( strncmp(s,"OK",2 ) == 0){
            ret=1;
            printf("OK\n");
        }else{
            ret=0;
            printf("RES\n");
            print_s(s);
            printf("\n");
        }
        if( read_serial_data(s,15) ){
            printf("> ");
            print_s(s);
            printf("\n");
        }
    }else{
        ret=xbee_at(s);
        printf("0x%02X\n",ret);
        ret=1;
    }
    return ret;
}

int main(int argc,char **argv){

    int i,j,atmode;
    byte port=0;                                // シリアルCOMポート番号
    char s[16];
    char mode[16];

    if( argc==2 ){
        if( atoi(argv[1]) < 0 ){
            port = 0x9F + (byte)(-atoi(argv[1])) ;
        }else  if( ( argv[1][0]=='b' || argv[1][0]=='B' )&& argv[1][1]!='\0' ){
            port = 0xB0 + ( argv[1][1] - '0');
        }else  if( ( argv[1][0]=='a' || argv[1][0]=='A' )&& argv[1][1]!='\0' ){
            port = 0xA0 + ( argv[1][1] - '0');
        }else port = (byte)(atoi(argv[1]));
    }

    printf("Running %s\n",argv[0]);
    if( port != 0 ){
        j = sci_init( port );       // シリアル初期化
    }else{
        // ポート検索
        for( i=14 ; i>0; i--){
            if( i > 4 ) j = sci_init( i-4 );
            else if(i>1) j = sci_init( 0xAE + i );
            else j = sci_init( 0xA0 );
            if(j)break;
        }
    }
    if( j == 0 ){
        fprintf(stderr,"ERROR @ UART OPEN\n");
        return( 1 );
    }else{
        printf("\nCONNECTED %d(0x%02X):%d(0x%02X)\n\n",port,port,j,j);
        port=j;
    }
    
    printf("ZIGBEE Device Type Switcher for XBee ZB S2C Series\n");
    printf("00:Coordinator AT,  01:Router AT,  02:End Device AT\n");
    printf("10:Coordinator API, 11:Router API, 12:End Device API\n");
    printf("Mode = ");
    fgets(mode, 15, stdin);
    if(
    	(mode[0]=='0' ||  mode[0]=='1') &&
    	(mode[1]>='0' &&  mode[1]<='2')
    ){
		mode[2]='0';		// エンバグ防止用 mode[1]は判定後に書き換えているので注意
		switch( mode[1] ){
			case '0':
				printf("Coordinator ");
				mode[1]='1';
				mode[2]='0';
				break;
			case '1':	printf("Router ");
				mode[1]='0';
				mode[2]='0';
				break;
			case '2':	printf("End Device ");
				mode[1]='0';
				mode[2]='4';
				break;
    	}
		switch( mode[0] ){
			case '0':	printf("AT\n"); break;
			case '1':	printf("API\n"); break;
    	}
    }else{
        fprintf(stderr,"INPUT ERROR\n");
	    close_serial_port();
	    return( 1 );
    }
    
    for(i=0;i<3;i++){
	    printf("Sending AT commands in AT mode\n");
	    sleep(2);
	    write(xbeeComFd, "+++", 3);
	    sleep(2);
	    read_serial_data(s,15);
	    if( strncmp(s,"OK",2 ) == 0) atmode=1; else{
	        atmode=0;
	        close_serial_port();
	        printf("Sending AT commands in API mode\n");
	        sleep(1);
	        if( xbee_init( port ) ){
	            printf("\nCONNECTED %d(0x%02X)\n\n",port,port);
	        }else{
	            fprintf(stderr,"ERROR @ XBEE OPEN\n");
	            return( 1 );
	        }
	    }
		j=0;
	    sprintf(s,"AT");
	    j+=send_at(atmode,s);
	    sprintf(s,"ATAP0%c",mode[0]);
	    j+=send_at(atmode,s);
	    sprintf(s,"ATCE0%c",mode[1]);
	    j+=send_at(atmode,s);
	    sprintf(s,"ATSM0%c",mode[2]);
	    j+=send_at(atmode,s);
	    sprintf(s,"ATWR");
	    j+=send_at(atmode,s);
	    sleep(3);
	    sprintf(s,"ATFR");
	    j+=send_at(atmode,s);
	    sleep(3);
	    if( read_serial_data(s,15) ){
	        printf("> ");
	        print_s(s);
	        printf("\n");
	    }
	    if(j==6){
		    printf("SUCCESS\n");
		    break;
		}else{
		    printf("ERROR (%d/6) [%d/3]\n",j,i+1);
		}
		sleep(10);
	}
    close_serial_port();
    return( 0 );
}
