/*********************************************************************
�T���v���A�v��18 HTTP�T�[�o

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                             Copyright (c) 2014 Wataru KUNINO & S.Noda
                             http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
#define XBEE_ERROR_TIME
#include "../libs/xbee.c"
#include "../libs/kbhit.c"
#include <sys/socket.h>
// #include <windows.h>						// Sleep
#define HTTP_PORT	80						// HTTP�T�[�o�̃|�[�g�ԍ�
#define HTDOCS "examples/htdocs/index.html" // ���JHTML�t�@�C��
#define HTSTAT "examples/htdocs/stat.html"  // ���JHTML�t�@�C��
											// (10�����ȏ�255�����ȉ�)
#define FORCE_INTERVAL	100					// �A�����莞�̑���Ԋu

int readHtml(char *buf,int len,char *file);	// HTML�t�@�C���̓ǂݍ���
int writeHtml(char *file);					// ���茋�ʂ̃t�@�C���o��
int writeHtmlPrint(char *filename,char *s);	// �e�L�X�g�̃t�@�C���o��
int checkHtml(char *file);					// �t�@�C���m�F

int HTDOCS_CURDIR_FLAG=0;					// 1:htdocs���J�����g�̎�
float sens_temp	=-1;						// ���x����l
float sens_lux	=-1;						// �Ɠx����l

int main(int argc,char **argv){
	/* XBee�p �ϐ��錾 */
	int i,trig=-1;
	byte port=0x00;
	byte dev_sens[8];
	int dev_sens_en=0;
	XBEE_RESULT xbee_result;
	
	/* HTTP�T�[�o�p �ϐ��錾 */
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	struct timeval timeout;
	fd_set Mask,readOk;
	int len, sock, yes = 1;
	char buf[2048],inbuf[2048],filename[256];
	char *strP;
	char user_command='\0';

	/* XBee �������� */
	if( argc==2 ) port = atoi(argv[1]);
	xbee_init( port );						// COM�|�[�g������

	/* HTTP�T�[�o �������� */
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

	/* �҂��� */
	printf("Receiving\n");
	while(user_command != 'Z'){
		/* XBee���� */
		if(dev_sens_en && trig == 0){
			xbee_force( dev_sens );		// �f�o�C�Xdev_sens�փf�[�^�v��
			trig = FORCE_INTERVAL;
		}
		if( trig > 0 ) trig--;
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B
		if(xbee_result.MODE){
			printf("ZigBee from ");
			for(i=4;i<8;i++) printf("%02X",xbee_result.FROM[i]); printf(" ");
			switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
				case MODE_RESP:				// xbee_force�̉�������M
					printf(" RESP");
				case MODE_GPIN:				// �q�@���玩�����M����M
					sens_lux = xbee_sensor_result( &xbee_result,LIGHT);
					printf(" %.0f Lux, ",sens_lux);		// �Ɠx���茋��
					sens_temp = xbee_sensor_result( &xbee_result,TEMP);
					printf("%.1f degC\n",sens_temp);	// ���x���茋��
					break;
				case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
					printf(" Found a Device\n");
					dev_sens_en = 1;					// sensor������
					memcpy(dev_sens,xbee_result.FROM,8); // ��M�����A�h���X���i�[
					xbee_gpio_config(dev_sens,1,AIN);	// �ڑ�����̃|�[�g1��AIN���͂�
					xbee_gpio_config(dev_sens,2,AIN);	// �ڑ�����̃|�[�g2��AIN���͂�
					xbee_atnj( 0 );						// �W���C���֎~
					trig = FORCE_INTERVAL;
					break;
				default:
					break;
			}
			writeHtml(filename);
		}
		/* HTTP�T�[�o���� */
		readOk=Mask;
		if(select(sock0+1,(fd_set *)&readOk,NULL,NULL,&timeout)>0){
			len = sizeof(client);
			sock = accept(sock0, (struct sockaddr *)&client, &len);
			printf("HTTP from %s:%d ", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			if(sock >= 0){
				memset(inbuf, 0, sizeof(inbuf));
				usleep(20000);
				recv(sock, inbuf, sizeof(inbuf),0);
				if(strncmp(inbuf,"GET",3)==0){		// HTTP-GET�̎�
					strP=strchr(&inbuf[4],' ');		// �X�y�[�X������
					i = strP - &inbuf[4];			// �X�y�[�X�܂ł̕�����
					strcpy(filename,"examples/htdocs");	// 15����(examples/�͌Œ�)
					if( i>1 && i<(256-15)) strncat(filename,&inbuf[4],i);
					else strncpy(filename,HTDOCS,255);
					printf(" GET [%s]\n",filename);
					len = readHtml(buf,sizeof(buf),filename);
					if(len) write(sock, buf, len);
					else write(sock,"HTTP/1.0 404 Not Found\r\n",24);
				}else if(strncmp(inbuf,"POST",4)==0){	// HTTP-POST�̎�
					i=0; user_command='\0';
					while(i<sizeof(inbuf) && user_command=='\0'){
						strP = strchr(&inbuf[i],'\n');	// ���s������
						if(strP==0) break;
						if(strP - &inbuf[i] <= 2) user_command = *(strP+1);
						i += strP - &inbuf[i] + 1;
					}
					if(strP){
						printf(" POST [%c]\n",user_command);
						switch(user_command){
							case 't':	// ��񑪒�
								if(dev_sens_en){
									xbee_force( dev_sens );
									trig = -1;
									writeHtmlPrint(HTSTAT,"��񑪒�");
								}else writeHtmlPrint(HTSTAT,"��񑪒�:ERROR");
								break;
							case 'T':	// �A������
								if(dev_sens_en){
									trig = 0;
									writeHtmlPrint(HTSTAT,"�A������");
								}else writeHtmlPrint(HTSTAT,"�A������:ERROR");
								break;
							case 'J':	// Join����
								writeHtmlPrint(HTSTAT,"Join����");
								xbee_atnj(0xFF);
								break;
							case 'v':	// �d���m�F
								if(dev_sens_en){
									sprintf(buf,"�d��=%d",xbee_batt(dev_sens));
								}else sprintf(buf,"�d���m�F:ERROR");
								writeHtmlPrint(HTSTAT,buf);
								break;
							default:
								sprintf(buf,"�R�}���h=%c",user_command);
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
//	flock(fileno(fp),LOCK_EX);	// �t�@�C�������b�N
	fseek(fp, 0L, SEEK_END);	// �t�@�C���̍Ōォ��0�o�C�g�܂�Ō�ֈړ�
	len = ftell(fp);			// ���̈ʒu���擾�B�܂�t�@�C���T�C�Y
	fseek(fp, 0L, SEEK_SET);	// �t�@�C���̐擪�ɖ߂�
		
	//           0123456789012345 6 7890123456789012  3 4 567890123456789012345678 9 0 1
	sprintf(buf,"HTTP/1.0 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n",len);
	lenP = (int)strlen(buf);
	size -= lenP + 1;
	while( feof(fp) == 0 && lenP < size){
		buf[lenP]=getc(fp);
		lenP++;
	}
	flock(fileno(fp),LOCK_UN);	// �t�@�C�����b�N�̉���
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
	fprintf(fp_html, "<tr><td rowspan=2>�Z���T</td>\r\n");
	fprintf(fp_html, "<td>�C��</td>\r\n<td align=right>%2.1f [��]</td>\r\n",sens_temp);
	fprintf(fp_html, "<tr><td>�Ɠx</td>\r\n<td align=right>%4.0f [lux]</td>\r\n",sens_lux);
	fprintf(fp_html, "</table><br>\r\n");
	fprintf(fp_html, "<form method=\"POST\">\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"��񑪒�\" name=\"t\">\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"�A������\" name=\"T\"><br>\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"Join����\" name=\"J\">\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"�d���m�F\" name=\"v\"><br>\r\n");
	fprintf(fp_html, "<input type=\"submit\" value=\"�I��\" name=\"Z\">\r\n");
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
�Q�l����
Geek�Ȃ؁[�� TCP�T�[�o�T���v��
http://www.geekpage.jp/programming/linux-network/http-server.php

�\�P�b�g�v���O���~���O
http://www.katto.comm.waseda.ac.jp/~katto/Class/GazoTokuron/code/socket.html

C����u���FTCP/IP�v���O����(����2�F�T�[�o) select�ɂ��^�C���A�E�g
http://www.ncad.co.jp/~komata/c-kouza10.htm
*/
