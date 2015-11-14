/***********************************************************************
xbee_at_term for Raspberry Pi

by ������
                   http://www.geocities.jp/bokunimowakaru/diy/ichigojam/

ư���ǧ�Ѥ�USB�ɥ饤��

- FDTI FT231XS
- FDTI FT232RL
- Silicon Labo CP2102�ʤդ����� Fu-sen.�ͤ���ξ����
- Silicon Labo CP2104

ư���ǧ�Ѥ�Linux OS
- Raspberry Pi : Linux version 3.1
- Ubuntu �ʤդ����� Fu-sen.�ͤ���ξ����
- Fedora �ʤդ����� Fu-sen.�ͤ���ξ����

************************************************************************
�����Υ����������ɤ����Ѥ��ޤ�����

// TinyTerm
// serial programming example for Raspberry Pi

// http://www.soramimi.jp/raspberrypi/rs232c/

TinyTerm������ѹ���
����λ�������ɲá�---�פǥ����ߥʥ뽪λ
����ư����ɸ�������������¸����λ�����������뵡ǽ���ɲ�
����ư�ڤӽ�λ�����ɽ����ǽ���ɲ�
��USB��³��IchigoJam�ؤ��б�
��XBee�Ѥ������ѹ��ʥܡ��졼�ȡ������������ԥ����ɡ�

************************************************************************/

#include "../libs/xbee.c"


void log_date(char *s){
    time_t error_time;
    struct tm *error_time_st;
    
    time(&error_time);
    error_time_st = localtime(&error_time);
    printf("%4d/%02d/%02d %02d:%02d:%02d %s\n",
        error_time_st->tm_year+1900,
        error_time_st->tm_mon+1,
        error_time_st->tm_mday,
        error_time_st->tm_hour,
        error_time_st->tm_min,
        error_time_st->tm_sec,
        s
    );
}

int main(int argc,char **argv){
	fd_set readfds;
	int loop=1,i;
	char c;
    byte j,port=0;
	struct termios stdinattr,stdin_bk;

	log_date("xbee_at_term for Raspberry Pi");
	
	if( argc==2 ){
		if( atoi(argv[1]) < 0 ){
			port = 0x9F + (byte)(-atoi(argv[1])) ;
		}else  if( ( argv[1][0]=='b' || argv[1][0]=='B' )&& argv[1][1]!='\0' ){
			port = 0xB0 + ( argv[1][1] - '0');
		}else  if( ( argv[1][0]=='a' || argv[1][0]=='A' )&& argv[1][1]!='\0' ){
			port = 0xA0 + ( argv[1][1] - '0');
		}else port = (byte)(atoi(argv[1]));
	}

	if( port != 0 ){
		j = sci_init( port );		// ���ꥢ������
	}else{
		// �ݡ��ȸ���
		for( i=14 ; i>0; i--){
			if( i > 4 ) j = sci_init( i-4 );
			else if(i>1) j = sci_init( 0xAE + i );
			else j = sci_init( 0xA0 );
			if(j)break;
		}
	}
	if( j == 0 ){
		log_date("ERROR @ UART OPEN");
		exit(-1);
	}

	// set to non canonical mode for stdin
	tcgetattr(0, &stdinattr);
	tcgetattr(0, &stdin_bk);
	stdinattr.c_lflag &= ~ICANON;
//	stdinattr.c_lflag &= ~ECHO;
//	stdinattr.c_lflag &= ~ISIG;
	stdinattr.c_cc[VMIN] = 0;
	stdinattr.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &stdinattr);
	log_date("CONNECTED");
	log_date("Type '---' to exit.");

	c=0x10;write(xbeeComFd, &c, 1);	// clear
	write(1, "AT> ", 4);
	while (loop) {
		FD_ZERO(&readfds);
		FD_SET(0, &readfds); // stdin
		FD_SET(xbeeComFd, &readfds);
		if (select(xbeeComFd + 1, &readfds, 0, 0, 0) != 0) {
			if (FD_ISSET(0, &readfds)) {
				if (read(0, &c, 1) == 1) { // input from stdin
//					write(1, &c, 1);		// echo back
					if (c == '\n') {
						write(xbeeComFd, "\r", 1); // no need to do
//						write(1, "AT> ", 4);
					} else {
						write(xbeeComFd, &c, 1);
					}
					if(c=='-'){
						loop++;
						if(loop>3){
							loop=0;
							write(1, "\n", 1);
						}
					}else loop=1;
				}
			}
			if (FD_ISSET(xbeeComFd, &readfds)) {
				if (read(xbeeComFd, &c, 1) == 1) {
					// output to stdout
					if (c == '\r') {		 // no need to do
						write(1, "\n", 1);
						write(1, "AT> ", 4);
					} else {
						write(1, &c, 1);
					}
				}
			}
		}
	}

	// back to canonical mode for stdin
	tcsetattr(0, TCSANOW, &stdin_bk);
	
	if(close_serial_port()){
		log_date("ERROR @ UART CLOSE");
		return 1;
	}
	log_date("CLOSED");
	return(0);
}


