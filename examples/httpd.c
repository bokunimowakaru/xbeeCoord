/***************************************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

HTMLファイルとGIF,PNG,JPEG画像ファイル(拡張子)のみサポートしています。

コンパイル	gcc -Wall httpd.c -o httpd.exe
実行方法	./httpd 2>> err.log

                               					Copyright (c) 2014-2016 Wataru KUNINO
                               					http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#include <stdio.h>								// 標準入出力
#include <unistd.h>								// Unix関数用 write usleep
#include <arpa/inet.h>							// htons, INADDR_ANY, inet_ntoa
#include <string.h>								// str*を使用
#include <ctype.h>								// isdigit,isgraphを使用
#include <time.h>								// time, localtime
#include <sys/file.h>							// flock
#include <sys/socket.h>							// Windows用TCPソケット

#define NAME			"HTTPD"					// 本ソフトの名前
#define VERSION			"1.1.0"					// 本ソフトのバージョン
#define HTTP_ADDR		"127.0.0.1"				// HTTPサーバのサーバのアドレス
#define HTTP_PORT		80						// HTTPサーバのポート番号
#define HTTP_TIMEOUT 	500						// HTTPサーバ待ち受け時間 500ms
#define HTDOCS_SIZE		32767					// 32 K Bytes (最大ファイルサイズ)
#define HTDOCSIN_SIZE	2048					// 2 kB (コマンド入力用バッファ)
#define HTDOCS "htdocs/httpd.html"				// 制御用HTMLファイル(htdocsは固定長)
#define HTSTAT "htdocs/stat.html"  				// 結果HTMLファイル

int readHtml(char *buf,int size,char *filename){
	FILE *fp;
	int i,len=0,lenP=0;
	
	if(size < 90) return 0;						// HTML応答の最小容量を確認
	for(i=0;i<strlen(filename);i++)if(!isgraph((int)filename[i])){
		fprintf(stderr,"ERROR Detected ctrl code:0x%X\n",filename[i]);
		return 0;
	}
	if((fp=fopen(filename,"rb"))==NULL){		// ファイルオープン
		fprintf(stderr,"ERROR open file:%s\n",filename);
		return 0;
	}
	flock(fileno(fp),LOCK_EX);					// ファイルをロック
	fseek(fp, 0L, SEEK_END);					// ファイルの最後へ移動
	len = ftell(fp);							// その位置を取得。つまりファイルサイズ
	fseek(fp, 0L, SEEK_SET);					// ファイルの先頭に戻る

	sprintf(buf,"HTTP/1.0 200 OK\r\nContent-Length: %d\r\nContent-Type: ",len);
	if(strncmp(".html",&filename[strlen(filename)-5],5)==0){
		strcat(buf,"text/html\r\n");
	}else if(strncmp(".jpg",&filename[strlen(filename)-4],4)==0){
		strcat(buf,"image/jpeg\r\n");
	}else if(strncmp(".png",&filename[strlen(filename)-4],4)==0){
		strcat(buf,"image/png\r\n");
	}else if(strncmp(".gif",&filename[strlen(filename)-4],4)==0){
		strcat(buf,"image/gif\r\n");
	}else{
		flock(fileno(fp),LOCK_UN);
		fclose( fp );
		buf[0]='\0';
		return 0;								// 拡張子がhtml,jpg,png,gifでないときに
	}											// リターン
	strcat(buf,"Connection: close\r\n\r\n");
	lenP = (int)strlen(buf);
	size -= lenP + 1;
	while( feof(fp) == 0 && lenP < size){		// ファイルの読み込み
		buf[lenP]=getc(fp);
		lenP++;
	}
	flock(fileno(fp),LOCK_UN);					// ファイルロックの解除
	buf[--lenP]='\0';							// 文字の終端
	fclose( fp );								// ファイルを閉じる
	return lenP;								// ファイルサイズ+HTMLヘッダ
}

int writeHtml(char *filename){
	FILE *fp_html;
	
	if( (fp_html = fopen(filename,"w")) == NULL ){
		fprintf(stderr,"ERROR open file:%s\n",filename);
		return 0;
	}
	fprintf(fp_html,"<head><title>httpd Web UI</title>\r\n");
	fprintf(fp_html, "<meta charset=\"utf-8\">\r\n");
	fprintf(fp_html, "<meta http-equiv=\"refresh\" content=127></head>\r\n");
	fprintf(fp_html, "<body><center>\r\n");
	fprintf(fp_html, "<h3>httpd Web UI</h3><table><tr><td>\r\n");
	fprintf(fp_html, "<form method=\"POST\"><tt>\r\n");
	fprintf(fp_html, "<hr>コマンド　　：\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"時刻確認\" name=\"t\">\r\n");
	fprintf(fp_html, "<hr>緊急停止　　：\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"Webサーバ停止\" name=\"Z\">\r\n");
	fprintf(fp_html, "</tt></form>\r\n");
	fprintf(fp_html, "<form method=\"POST\"><tt>\r\n");
	fprintf(fp_html, "手動コマンド：<input type=\"text\" name=\".\">\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"送信\" >\r\n");
	fprintf(fp_html, "<hr></tt></form>\r\n");
	fprintf(fp_html, "</td></tr></table>\r\n");
	fprintf(fp_html, "<font size=1>%s %s (gcc:%s)<br>\r\n",NAME,VERSION,__VERSION__);
	fprintf(fp_html, "date:%s %s<br><br></font>\r\n",__DATE__,__TIME__);
	fprintf(fp_html, "[<a href=\"index.html\">戻る</a>]\r\n");
	fprintf(fp_html, "</center></body></html>\r\n");
	fclose(fp_html);
	return 1;
}

int writeHtmlPrint(char *filename,char *url,char *s){
	FILE *fp_html;
	
	if( (fp_html = fopen(filename,"w")) == NULL ){
		fprintf(stderr,"ERROR open file:%s\n",filename);
		return 0;
	}
	fprintf(fp_html,"<head><title>XBee Responce</title>\r\n");
	fprintf(fp_html, "<meta charset=\"utf-8\">\r\n");
	fprintf(fp_html, "<meta http-equiv=\"refresh\" content=\"3;URL=%s\"></head>\r\n",url);
	fprintf(fp_html, "<body><center>\r\n");
	fprintf(fp_html, "<h3>%s</h3>\r\n",s);
	fprintf(fp_html, "</center></body></html>\r\n");
	fclose(fp_html);
	return 1;
}

int checkHtml(char *filename){
	FILE *fp;
	if( (fp = fopen(filename,"r")) == NULL ){
		fprintf(stderr,"ERROR open file:%s\n",filename);
		return 0;
	}
	fclose( fp );
	return 1;
}

int main(int argc,char **argv){
	int i;
	int EXIT=0;
	time_t timer;
	struct tm *time_st;
	char today_s[11]   = "2000/01/01";
	char time_s[6]     = "00:00";

	/* HTTPサーバ用 変数宣言 */
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
//	struct timeval timeout;
//	fd_set Mask;
//	fd_set readOk;
	int len, sock, yes = 1;
	socklen_t socklen;
	char buf[HTDOCS_SIZE],inbuf[HTDOCSIN_SIZE],filename[256];
	char *strP;
	char user_command='\0';

	printf("------------------------------------------------------------\n");
	printf("                      httpd is started \n");
	printf("------------------------------------------------------------\n");

	/* HTTPサーバ 初期処理 */
//	timeout.tv_sec = 0; 
//	timeout.tv_usec = HTTP_TIMEOUT * 1000;
	if((sock0=socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("ERROR socket fault\n"); return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HTTP_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
	if(bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0){
		perror("ERROR bind fault\n"); return -1;
	}
	/*
	FD_ZERO(&Mask);
	FD_SET(sock0,&Mask);
	FD_SET(0,&Mask);
	*/
	if(listen(sock0, 5) != 0){
		perror("ERROR listen fault"); return -1;
	}
	if( writeHtml(HTDOCS)==0 ) return -1;

	// 初期化処理　時刻
	time(&timer);
	time_st = localtime(&timer);
	sprintf(today_s,"%4d/%02d/%02d",time_st->tm_year+1900,time_st->tm_mon+1,time_st->tm_mday);
	sprintf(time_s,"%02d:%02d",time_st->tm_hour,time_st->tm_min);
	printf("%s %s httpd Start %s:%d\n",today_s,time_s,HTTP_ADDR,HTTP_PORT);
	fprintf(stderr,"%s %s httpd Start %s:%d\n",today_s,time_s,HTTP_ADDR,HTTP_PORT);
	strcpy(filename,HTDOCS);
	strcpy(filename,&filename[7]);
	printf("Usage for Web UI: http://%s/%s\n",HTTP_ADDR,filename);

	// メイン処理
	while( !EXIT ){	// ほぼ永久に受信する

		/* HTTPサーバ処理 */
		user_command = '\0';
	//	readOk=Mask;
	//	timeout.tv_sec = 0; 
	//	timeout.tv_usec = HTTP_TIMEOUT * 1000;
	//	if(select(sock0+1,(fd_set *)&readOk,NULL,NULL,&timeout)>0){

		/* 待ち受け */
		printf("Waiting for packets\n");
		socklen = (socklen_t)sizeof(client);
		sock = accept(sock0, (struct sockaddr *)&client, &socklen);
		time(&timer);
		time_st = localtime(&timer);
		sprintf(today_s,"%4d/%02d/%02d",time_st->tm_year+1900,time_st->tm_mon+1,time_st->tm_mday);
		sprintf(time_s,"%02d:%02d",time_st->tm_hour,time_st->tm_min);
		printf("HTTP from %s:%d ", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		if(sock >= 0){
			memset(inbuf, 0, sizeof(inbuf));
			usleep(1000);						// クライアント側の待ち時間
			recv(sock, inbuf, sizeof(inbuf),0);
			usleep(1000);						// クライアント側の待ち時間
		//	printf("%s[EOF]\n\n", inbuf);		// テスト用
			if(strncmp(inbuf,"GET",3)==0){		// HTTP-GETの時
				strP=strchr(&inbuf[4],' ');		// スペースを検索
				i = strP - &inbuf[4];			// スペースまでの文字数
				strcpy(filename,"htdocs");	// 6文字(7バイト)
				if( i>1 && i<(256-7)) strncat(filename,&inbuf[4],i);	// セグフォ確認 filename[256] inbuf[2048] ⇒ i<249
				else strcpy(filename,"htdocs/index.html");
				printf(" GET [%s]\n",filename);
				len = readHtml(buf,sizeof(buf),filename);
				if(len){
					i=send(sock, buf, len, 0);		// write(sock, buf, len);
					if(i<=0) fprintf(stderr,"%s %s ERROR:sending sockets (%d)\n",today_s,time_s,i);
				}else{
					fprintf(stderr,"%s %s ERROR:readHtml '%s'(%d bytes)\n",today_s,time_s,filename,len);
					//          12345678901234567890123 4 -> 24文字
					write(sock,"HTTP/1.0 404 Not Found\r\n",24);
				}
			}else if(strncmp(inbuf,"POST",4)==0){	// HTTP-POSTの時
				i=0; user_command='\0';
			//	printf("\n recieved [%s] \n",inbuf);		// テスト用
				while(i<sizeof(inbuf) && user_command=='\0'){
					strP = strchr(&inbuf[i],'\n');	// 改行を検索
					if(strP==0) break;
					if(strP - &inbuf[i] <= 2) user_command = *(strP+1);
					i += strP - &inbuf[i] + 1;
				}
				if(strP){
					printf(" POST [%c]\n",user_command);
					sprintf(filename,"http://%s/httpd.html",HTTP_ADDR);	// 標準HTTPポート(80)へ
					switch(user_command){
						case 't':
							sprintf(buf,"日付 = %s<br>時刻 = %s",today_s,time_s);
							writeHtmlPrint(HTSTAT,filename,buf);
							break;
						case 'Z':
							writeHtmlPrint(HTSTAT,filename,"システムの緊急停止を行いました");
							break;
						default:
							sprintf(buf,"コマンド=%c",user_command);
							writeHtmlPrint(HTSTAT,filename,buf);
							break;
					}
					readHtml(buf,sizeof(buf),HTSTAT);
					write(sock, buf, (int)strlen(buf));
				}else{
					printf(" ERROR 400\n");
					fprintf(stderr,"%s %s ERROR:No HTTP Content\n",today_s,time_s);
					//          1234567890123456789012345 6 -> 26文字
					write(sock,"HTTP/1.0 400 Bad Request\r\n",26);
				}
			}else{
				printf(" ERROR 500\n");
				printf("==== recieved ====\n%s[EOF]\n",inbuf);		// テスト用
				inbuf[4]='\0';
				fprintf(stderr,"%s %s ERROR:unsupported HTTP command '%s'\n",today_s,time_s,inbuf);
				//          12345678901234567890123456789012345 6 -> 36文字
				usleep(20);
				write(sock,"HTTP/1.0 500 Internal Server Error\r\n",36);
				printf("\n==== recieved ====\n%s[EOF]\n",inbuf);		// テスト用
			}
			close(sock);
		}else fprintf(stderr,"%s %s ERROR:open socket from %s:%d\n",today_s,time_s,inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		switch( user_command ){
			case 'Z':
				EXIT=1;
				printf("\n                          E X I T \n");
				printf("------------------------------------------------------------\n");
				fprintf(stderr,"%s %s Recept exit\n",today_s,time_s);
				break;
			default:
				break;
		}
	}
	fprintf(stderr,"%s %s EXIT --------\n",today_s,time_s);
	printf("------------------------------------------------------------\n");
	printf("        Bye! (restart to './httpd 2>> err.log')\n");
	printf("------------------------------------------------------------\n");
	return 0;
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
