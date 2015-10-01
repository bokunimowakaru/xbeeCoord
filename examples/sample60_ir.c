/***************************************************************************************
�T���v���A�v��60 �����[�g�ԊO�������R���Ǘ��p PC(Cygwin)�e�@

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

XBee �t�@�[���E�F�A�́uZIGBEE COORDINATOR API�v���g�p���܂��B

                                                       Copyright (c) 2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#include "../libs/kbhit.c"						// �L�[�{�[�h���� by Wataru
#include <ctype.h>								// isgraph

#define FORCE_INTERVAL  200						// �f�[�^�v���Ԋu(���悻10ms�̔{��)
#define DATA_LEN	16+5						// �����R���R�[�h�̃f�[�^��(byte)+5
#define AEHA        0
#define NEC         1
#define SIRC        2
byte ir_data[DATA_LEN]
// ESC  'I'  'R' AEHA Len  data....
={0x1B,0x49,0x52,0x00,0x06,0xAA,0x5A,0x8F,0x12,0x15,0xE1};	// AQUOS ���ʂ�������

byte SET_IR_MODE[][5]={
	{0x1B,0x49,0x52,0x00,0x00},					// AEHA
	{0x1B,0x49,0x52,0x01,0x00},					// NEC
	{0x1B,0x49,0x52,0x02,0x00}					// SIRC
};
	
byte IR_DATA_SHARP_TV[][6]={
	{0xAA,0x5A,0x8F,0x12,0x16,0xD1},			// AQUOS TV POWER
	{0xAA,0x5A,0x8F,0x12,0x4E,0x32},			// AQUOS TV [1]
	{0xAA,0x5A,0x8F,0x12,0x4F,0x22},			// AQUOS TV [2]
	{0xAA,0x5A,0x8F,0x12,0x50,0xC2},			// AQUOS TV [3]
	{0xAA,0x5A,0x8F,0x12,0x51,0xD2},			// AQUOS TV [4]
	{0xAA,0x5A,0x8F,0x12,0x52,0xE2},			// AQUOS TV [5]
	{0xAA,0x5A,0x8F,0x12,0x53,0xF2},			// AQUOS TV [6]
	{0xAA,0x5A,0x8F,0x12,0x54,0x82},			// AQUOS TV [7]
	{0xAA,0x5A,0x8F,0x12,0x55,0x92},			// AQUOS TV [8]
	{0xAA,0x5A,0x8F,0x12,0x56,0xA2},			// AQUOS TV [9]
	{0xAA,0x5A,0x8F,0x12,0x57,0xB2},			// AQUOS TV [10]
	{0xAA,0x5A,0x8F,0x12,0x58,0x42},			// AQUOS TV [11]
	{0xAA,0x5A,0x8F,0x12,0x59,0x52},			// AQUOS TV [12]
	{0xAA,0x5A,0x8F,0x12,0x14,0xF1},			// AQUOS TV VOL_UP
	{0xAA,0x5A,0x8F,0x12,0x15,0xE1},			// AQUOS TV VOL_DOWN
};

int main(int argc,char **argv){
	int i;
	char c;
	int send_flag=0;							// IR���M�t���O(1:���M��)
	int read_flag=0;							// IR�ǂݍ��݃t���O(1:�ǂݎ�蒆)
	int quit_flag=0;							// �I���t���O
	byte com=0;									// �V���A��COM�|�[�g�ԍ�
	byte dev[8]={0,0,0,0,0xff,0xff,0xff,0xff};	// XBee�q�@�f�o�C�X�̃A�h���X
	XBEE_RESULT xbee_result;					// ��M�f�[�^(�ڍ�)
	int len;									// ��M�f�[�^��

	if(argc==2) com=(byte)atoi(argv[1]);		// ����������Εϐ�com�ɑ������
	xbee_init( com );							// XBee�pCOM�|�[�g�̏�����
	xbee_at("ATSP0AF0");						// SP = 28 sec(�ő�l)
	xbee_at("ATSN0E10");						// SN = 3600��
	xbee_atnj(0xFF);							// �f�o�C�X�̎Q���󂯓�����J�n

	while(!quit_flag){
		/* �L�[�{�[�h��M */
		if( kbhit() ){
			c = getchar();
			printf(" -> Accepted\n");
			switch( c ){
				case 'a':
					printf("Set AEHA mode\n");	xbee_bytes(dev,SET_IR_MODE[0],5);
					break;
				case 'n':
					printf("Set NEC mode\n");	xbee_bytes(dev,SET_IR_MODE[1],5);
					break;
				case 's':
					printf("Set SIRC mode\n");	xbee_bytes(dev,SET_IR_MODE[2],5);
					break;
				case '\n': case '\r':
					printf("Send IR\n");
					send_flag=1;
					break;
				case 'r':
					printf("Read IR\n");
					read_flag=1;
					break;
				case 'p':
					i=(int)xbee_ping(dev);
					if(i != 0xFF ) printf("Ping OK (%02X)\n",i);
					else printf("Ping ERROR\n");
					break;
				case 'j':
					printf("Join Enable\n");
					xbee_atcb(2);
					break;
				case '+':
					printf("AQOUS Vol. Up\n");
					ir_data[3]=0; ir_data[4]=6;			// AEHA  6�o�C�g
					bytecpy(&ir_data[5],IR_DATA_SHARP_TV[13],6);
					send_flag=1;
					break;
				case '-':
					printf("AQOUS Vol. Down\n");
					ir_data[3]=0; ir_data[4]=6;			// AEHA  6�o�C�g
					bytecpy(&ir_data[5],IR_DATA_SHARP_TV[14],6);
					send_flag=1;
					break;
				case 'Q': case 'q': case 'Z': case '!':
					printf("Read IR\n");
					quit_flag=1;
					break;
				default:
					if( isdigit((int)c) ){
						i=(int)(c - '0');
						printf("AQOUS Ch = %d\n",i);
						ir_data[3]=0; ir_data[4]=6;		// AEHA  6�o�C�g
						bytecpy(&ir_data[5],IR_DATA_SHARP_TV[i],6);
						send_flag=1;
					}else{
						printf("Error\n");
						printf("Usage : s=send r=read q=quit\n");
					}
					break;
			}
		}
		if(send_flag && !read_flag){
			xbee_bytes(dev,ir_data,ir_data[4]+5);
			printf("Send IR [%2d] :",ir_data[4]+5);
			for(i=0;i<ir_data[4]+5;i++) printf(" %02X",ir_data[i]); printf("\n");
			send_flag=0;
		}
		/* XBee �f�[�^��M */
		len = xbee_rx_call( &xbee_result );		// �f�[�^����M
		switch(xbee_result.MODE){
			case MODE_UART:
				printf("Recieved[%2d] : ",len);
				for(i=0;i<len;i++) printf("%02X ",xbee_result.DATA[i]);
				for(i=0;i<len;i++) if(isgraph(xbee_result.DATA[i])) putchar(xbee_result.DATA[i]); else putchar('.'); printf("\n");
				if(	read_flag &&
					xbee_result.DATA[0]==0x1B &&
					xbee_result.DATA[1]==(byte)'I' &&
					xbee_result.DATA[2]==(byte)'R' &&
					xbee_result.DATA[4]+5 <= len &&
					len <= DATA_LEN
				){
					printf("Stored Read IR Data\n");
					bytecpy(ir_data,xbee_result.DATA,len);
					printf("Recieved[%2d] :",len);
					for(i=0;i<len;i++) printf(" %02X",ir_data[i]); printf("\n");
					read_flag=0;
				}else if(len==2 &&						// strncnp(xbee_result.DATA,"OK",2)
					xbee_result.DATA[0]==(byte)'O' &&
					xbee_result.DATA[1]==(byte)'K'
				){
					printf("Done Send IR\n");
					send_flag=0;
				}else if(len >= 3 &&						// strncnp(xbee_result.DATA,"ERR",3)
					xbee_result.DATA[0]==(byte)'E' &&
					xbee_result.DATA[1]==(byte)'R' &&
					xbee_result.DATA[2]==(byte)'R'
				){
					printf("Send IR ERROR\n");
					send_flag=0;
				}
				break;
			case MODE_IDNT:
				printf("Found a Dev. :");
				bytecpy(dev,xbee_result.FROM,8);
				for(i=0;i<8;i++) printf(" %02X",dev[i]); printf("\n");
				xbee_atnj(10);
				break;
			
		}
	}
	printf("Done\n");
	return(0);
}
