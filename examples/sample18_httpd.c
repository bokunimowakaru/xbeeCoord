/*********************************************************************
サンプルアプリ18 HTTPサーバ

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                             Copyright (c) 2014 Wataru KUNINO & S.Noda
                             http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
#define XBEE_ERROR_TIME
#include "../libs/xbee.c"
#include "../libs/kbhit.c"
#include <sys/socket.h>
// #include <windows.h>						// Sleep
#define HTTP_PORT	80						// HTTPサーバのポート番号
#define HTDOCS "examples/htdocs/index.html" // 公開HTMLファイル
#define HTSTAT "examples/htdocs/stat.html"  // 公開HTMLファイル
											// (10文字以上255文字以下)
#define FORCE_INTERVAL	100					// 連続測定時の測定間隔

int readHtml(char *buf,int len,char *file);	// HTMLファイルの読み込み
int writeHtml(char *file);					// 測定結果のファイル出力
int writeHtmlPrint(char *filename,char *s);	// テキストのファイル出力
int checkHtml(char *file);					// ファイル確認

int HTDOCS_CURDIR_FLAG=0;					// 1:htdocsがカレントの時
float sens_temp	=-1;						// 温度測定値
float sens_lux	=-1;						// 照度測定値

int main(int argc,char **argv){
	/* XBee用 変数宣言 */
	int i,trig=-1;
	byte port=0x00;
	byte dev_sens[8];
	int dev_sens_en=0;
	XBEE_RESULT xbee_result;
	
	/* HTTPサーバ用 変数宣言 */
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	struct timeval timeout;
	fd_set Mask,readOk;
	int len, sock, yes = 1;
	char buf[2048],inbuf[2048],filename[256];
	char *strP;
	char user_command='\0';

	/* XBee 初期処理 */
	if( argc==2 ) port = atoi(argv[1]);
	xbee_init( port );						// COMポート初期化

	/* HTTPサーバ 初期処理 */
	timeout.tv_sec = 0; 
	timeout.tv_usec = 10000; 	// 10 ms
	if((sock0=socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("ERROR socket fault\n"); return(-1);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HTTP_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
	if(bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0){
		perror("ERROR bind fault\n"); return(-1);
	}
	FD_ZERO(&Mask);
	FD_SET(sock0,&Mask);
	FD_SET(0,&Mask);
	if(listen(sock0, 5) != 0){
		perror("ERROR listen fault"); return(-1);
	}
	strncpy(filename,HTDOCS,255);
	checkHtml(filename);
	writeHtml(filename);

	/* 待ち受け */
	printf("Receiving\n");
	while(user_command != 'Z'){
		/* XBee処理 */
		if(dev_sens_en && trig == 0){
			xbee_force( dev_sens );		// デバイスdev_sensへデータ要求
			trig = FORCE_INTERVAL;
		}
		if( trig > 0 ) trig--;
		xbee_rx_call( &xbee_result );	// データを受信します。
		if(xbee_result.MODE){
			printf("ZigBee from ");
			for(i=4;i<8;i++) printf("%02X",xbee_result.FROM[i]); printf(" ");
			switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
				case MODE_RESP:				// xbee_forceの応答を受信
					printf(" RESP");
				case MODE_GPIN:				// 子機から自動送信を受信
					sens_lux = xbee_sensor_result( &xbee_result,LIGHT);
					printf(" %.0f Lux, ",sens_lux);		// 照度測定結果
					sens_temp = xbee_sensor_result( &xbee_result,TEMP);
					printf("%.1f degC\n",sens_temp);	// 温度測定結果
					break;
				case MODE_IDNT:				// 新しいデバイスを発見
					printf(" Found a Device\n");
					dev_sens_en = 1;					// sensor発見済
					memcpy(dev_sens,xbee_result.FROM,8); // 受信したアドレスを格納
					xbee_gpio_config(dev_sens,1,AIN);	// 接続相手のポート1をAIN入力へ
					xbee_gpio_config(dev_sens,2,AIN);	// 接続相手のポート2をAIN入力へ
					xbee_atnj( 0 );						// ジョイン禁止
					trig = FORCE_INTERVAL;
					break;
				default:
					break;
			}
			writeHtml(filename);
		}
		/* HTTPサーバ処理 */
		readOk=Mask;
		if(select(sock0+1,(fd_set *)&readOk,NULL,NULL,&timeout)>0){
			len = sizeof(client);
			sock = accept(sock0, (struct sockaddr *)&client, &len);
			printf("HTTP from %s:%d ", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			if(sock >= 0){
				memset(inbuf, 0, sizeof(inbuf));
				usleep(20000);
				recv(sock, inbuf, sizeof(inbuf),0);
				if(strncmp(inbuf,"GET",3)==0){		// HTTP-GETの時
					strP=strchr(&inbuf[4],' ');		// スペースを検索
					i = strP - &inbuf[4];			// スペースまでの文字数
					strcpy(filename,"examples/htdocs");	// 15文字(examples/は固定)
					if( i>1 && i<(256-15)) strncat(filename,&inbuf[4],i);
					else strncpy(filename,HTDOCS,255);
					printf(" GET [%s]\n",filename);
					len = readHtml(buf,sizeof(buf),filename);
					if(len) write(sock, buf, len);
					else write(sock,"HTTP/1.0 404 Not Found\r\n",24);
				}else if(strncmp(inbuf,"POST",4)==0){	// HTTP-POSTの時
					i=0; user_command='\0';
					while(i<sizeof(inbuf) && user_command=='\0'){
						strP = strchr(&inbuf[i],'\n');	// 改行を検索
						if(strP==0) break;
						if(strP - &inbuf[i] <= 2) user_command = *(strP+1);
						i += strP - &inbuf[i] + 1;
					}
					if(strP){
						printf(" POST [%c]\n",user_command);
						switch(user_command){
							case 't':	// 一回測定
								if(dev_sens_en){
									xbee_force( dev_sens );
									trig = -1;
									writeHtmlPrint(HTSTAT,"一回測定");
								}else writeHtmlPrint(HTSTAT,"一回測定:ERROR");
								break;
							case 'T':	// 連続測定
								if(dev_sens_en){
									trig = 0;
									writeHtmlPrint(HTSTAT,"連続測定");
								}else writeHtmlPrint(HTSTAT,"連続測定:ERROR");
								break;
							case 'J':	// Join許可
								writeHtmlPrint(HTSTAT,"Join許可");
								xbee_atnj(0xFF);
								break;
							case 'v':	// 電圧確認
								if(dev_sens_en){
									sprintf(buf,"電圧=%d",xbee_batt(dev_sens));
								}else sprintf(buf,"電圧確認:ERROR");
								writeHtmlPrint(HTSTAT,buf);
								break;
							default:
								sprintf(buf,"コマンド=%c",user_command);
								writeHtmlPrint(HTSTAT,buf);
								break;
						}
						readHtml(buf,sizeof(buf),HTSTAT);
						send(sock, buf, (int)strlen(buf), 0);
					}else write(sock,"HTTP/1.0 400 Bad Request\r\n",26);
				}
				close(sock);
			}else perror("ERROR accept sock\n");
		}
	}
	printf("Exit\n");
	close(sock0);
	close_serial_port();
	return 0;
}

int readHtml(char *buf,int size,char *filename){
	FILE *fp;
	int len=0,lenP=0;
	
	if( (fp = fopen(filename,"r")) == NULL ){
		if( (fp = fopen(&filename[9],"r")) == NULL ){
			fprintf(stderr,"ERROR open file:%s\n",filename);
			return(0);
		}else HTDOCS_CURDIR_FLAG=1;
	}
//	flock(fileno(fp),LOCK_EX);	// ファイルをロック
	fseek(fp, 0L, SEEK_END);	// ファイルの最後から0バイトつまり最後へ移動
	len = ftell(fp);			// その位置を取得。つまりファイルサイズ
	fseek(fp, 0L, SEEK_SET);	// ファイルの先頭に戻る
		
	//           0123456789012345 6 7890123456789012  3 4 567890123456789012345678 9 0 1
	sprintf(buf,"HTTP/1.0 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n",len);
	lenP = (int)strlen(buf);
	size -= lenP + 1;
	while( feof(fp) == 0 && lenP < size){
		buf[lenP]=getc(fp);
		lenP++;
	}
	flock(fileno(fp),LOCK_UN);	// ファイルロックの解除
	buf[--lenP]='\0';
	fclose( fp );
	return(lenP);
}

int writeHtml(char *filename){
	FILE *fp_html;
	
	if(HTDOCS_CURDIR_FLAG==0){
		if( (fp_html = fopen(filename,"w")) == NULL ){
			fprintf(stderr,"ERROR open file:%s\n",filename);
			return(0);
		}
	}else if( (fp_html = fopen(&filename[9],"w")) == NULL ){
		fprintf(stderr,"ERROR open file:%s\n",&filename[9]);
		return(0);
	}
	fprintf(fp_html,"<head><title>XBee Sensors</title>\r\n");
	fprintf(fp_html, "<meta charset=\"Shift_JIS\">\r\n");
	fprintf(fp_html, "<meta http-equiv=\"refresh\" content=5></head>\r\n");
	fprintf(fp_html, "<body><center>\r\n");
	fprintf(fp_html, "<h3>XBee Sensors</h3>\r\n<table border=1>\r\n");
	fprintf(fp_html, "<tr><td rowspan=2>センサ</td>\r\n");
	fprintf(fp_html, "<td>気温</td>\r\n<td align=right>%2.1f [℃]</td>\r\n",sens_temp);
	fprintf(fp_html, "<tr><td>照度</td>\r\n<td align=right>%4.0f [lux]</td>\r\n",sens_lux);
	fprintf(fp_html, "</table><br>\r\n");
	fprintf(fp_html, "<form method=\"POST\">\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"一回測定\" name=\"t\">\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"連続測定\" name=\"T\"><br>\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"Join許可\" name=\"J\">\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"電圧確認\" name=\"v\"><br>\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"終了\" name=\"Z\">\r\n");
	fprintf(fp_html, "</form>\r\n");
	fprintf(fp_html, "</center></body></html>\r\n");
	fclose(fp_html);
	return(1);
}

int writeHtmlPrint(char *filename,char *s){
	FILE *fp_html;
	
	if(HTDOCS_CURDIR_FLAG==0){
		if( (fp_html = fopen(filename,"w")) == NULL ){
			fprintf(stderr,"ERROR open file:%s\n",filename);
			return(0);
		}
	}else if( (fp_html = fopen(&filename[9],"w")) == NULL ){
		fprintf(stderr,"ERROR open file:%s\n",&filename[9]);
		return(0);
	}
	fprintf(fp_html,"<head><title>XBee Sensors</title>\r\n");
	fprintf(fp_html, "<meta charset=\"Shift_JIS\">\r\n");
	fprintf(fp_html, "<meta http-equiv=\"refresh\" content=3></head>\r\n");
	fprintf(fp_html, "<body><center>\r\n");
	fprintf(fp_html, "<h3>%s</h3>\r\n",s);
	fprintf(fp_html, "</center></body></html>\r\n");
	fclose(fp_html);
	return(1);
}

int checkHtml(char *filename){
	FILE *fp;
	if( (fp = fopen(filename,"r")) == NULL ){
		if( (fp = fopen(&filename[9],"r")) == NULL ){
			fprintf(stderr,"ERROR open file:%s\n",filename);
			return(0);
		}else HTDOCS_CURDIR_FLAG=1;
	}
	fclose( fp );
	return(1);
}


/*
参考文献
Geekなぺーじ TCPサーバサンプル
http://www.geekpage.jp/programming/linux-network/http-server.php

ソケットプログラミング
http://www.katto.comm.waseda.ac.jp/~katto/Class/GazoTokuron/code/socket.html

C言語講座：TCP/IPプログラム(その2：サーバ) selectによるタイムアウト
http://www.ncad.co.jp/~komata/c-kouza10.htm
*/
