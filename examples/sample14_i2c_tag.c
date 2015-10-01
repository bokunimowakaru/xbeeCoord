/*********************************************************************
�T���v���A�v��14 �H�� ���^LCD I2C Router

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

���^�t���ɕ\�������܂Łu��Q���v�قǂ�����܂��B
�q�@XBee ���W���[���́uRouter�v�ɐݒ肵�܂��B(End Device�ł͓����܂���)

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
XBee�n�[�h�E�F�A
	Sensirion SHT-11�܂���SHT-71��XBee��1,4,7,10�ԃs���ɐڑ�����B
	�s���͂��傤��3�����Ȃ̂ŁA�����o���₷���Ǝv���܂��B
			XBee 1 -> LCD 2 (VCC)
			XBee 4 -> LCD 4 (SDA)
			XBee 7 -> LCD 3 (SCL)
			XBee10 -> LCD 5 (GND)

��������
	XBee Coord	�N��		��10�b
	LCD Init	������ 		��30�b
	LCD Data 1	1�s�ڕ\��	��45�b
	LCD Data 1	1�s�ڕ\��	��45�b
	
����d��
	��������	24mA �~120�b				��  0.8 mAh
	����������	24mA �~ 90�b				��  0.6 mAh
	
	�\����		0.8mA�~24����				�� 19.2 mAh
	�Ԍ��ʐM��	40mA �~0.5�b�^60�b�~24����	��  8.0 mAh
	�\�����̏���d���ʂ̍��v 19.2mAh+24mAh	�� 27.2 mAh
*/
//#define DEBUG
#include "../libs/xbee.c"
#include "../libs/kbhit.c"
#ifdef LITE	// BeeBee Lite by ��
	int main(){
		printf("Lite mode dewaugokimasen. gomenne.");
		return 0;
	}
#else

//�@�@�@�@�@�@�@�����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
byte dev[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

void print_timer(){						// ���v�p
	time_t timer;
	struct tm *time_st;
	
	time(&timer);
	time_st = localtime(&timer);
	printf("%4d/%02d/%02d %02d:%02d:%02d ",
		time_st->tm_year+1900,
		time_st->tm_mon+1,
		time_st->tm_mday,
		time_st->tm_hour,
		time_st->tm_min,
		time_st->tm_sec );
}

void print_data(byte *dev, char *lcd, byte y, char *s){
	byte data[2];
	byte i;
	print_timer();printf("%s\tDisplay '%s'\n",s,lcd);
	xbee_rat(dev,"ATSTFFFE");	// Time before Sleep ��65�b��
	data[0]=0x00;
	if(y==0) data[1]=0x80; else data[1]=0xC0;
	xbee_i2c_write(dev,0x7C,data,2);
	for(i=0;i<8;i++){
		data[0]=0x40;
		data[1]=(byte)lcd[i];
		xbee_i2c_write(dev,0x7C,data,2);
	}
}

void wakeup(byte *dev, char *s){
	print_timer();printf("%s\tWake XBee Up\n",s);
	xbee_rat(dev,"ATSTFFFE");	// Time before Sleep ��65�b��
	xbee_rat(dev,"ATSP20");		// Sleep Period 
	xbee_rat(dev,"ATSN01");		// Sleep Number
	xbee_rat(dev,"ATIR00");		// Periodic IO Sampling OFF
	xbee_rat(dev,"ATSO02");		// Sleep Options = Always wake for full ST time
}

void goto_sleep(byte *dev, char *s){
	print_timer();printf("%s\tXBee Go Sleep\n",s);
	xbee_rat(dev,"ATSO00");		// Sleep Options = �ʏ탂�[�h
	xbee_rat(dev,"ATST01F4");	// Time before Sleep 0.5�b��
	xbee_rat(dev,"ATSP07D0");	// Sleep Period 
	xbee_rat(dev,"ATSN03");		// Sleep Number
	xbee_rat(dev,"ATIREA60");	// Periodic IO Sampling
	xbee_rat(dev,"ATSM04");		// Sleep Mode = CYCLIC SLEEP
}

int main(int argc,char **argv){
	char lcd[2][9]={"�޸��ܶ�",	// �t��1�s��(8����)
					" XBee ZB"};// �t��2�s��(8����)
	byte lcd_x=0,lcd_y=0;		// �\���ʒu
	byte disp_sw=0;				// �\���ύX�t���O
	byte data[2];
	byte i,j;
	XBEE_RESULT xbee_result;

	byte port=0;
	
	// setup����
	print_timer();printf("setup\tXBee Init\n");
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );			// XBee�pCOM�|�[�g�̏�����(�����̓|�[�g�ԍ�)
	xbee_at("ATSP07D0");		// �e�@�̐ݒ� Sleep Period 
	xbee_at("ATSN03");			// �e�@�̐ݒ� Sleep Number
	xbee_atnj( 0xFF );			// �f�o�C�X����ɎQ���󂯓���(�e�X�g�p)
	while( xbee_rat(dev,"ATSTFFFE")!=STATUS_OK ){
		print_timer();printf("setup\tWainting for XBee\n");
		delay(1000);
	}
	wakeup(dev,"setup");
	xbee_ratd_myaddress(dev);	// �e�@�̃A�h���X���q�@�ɐݒ�
	xbee_rat(dev,"ATD103");		// DIO�|�[�g1����͂�
	xbee_rat(dev,"ATPR1FFF");	// �S�|�[�g�Ƀv���A�b�v��ݒ�
	xbee_i2c_init(dev);			// I2C�C���^�t�F�[�X�̏�����
	
	print_timer();printf("setup\tLCD Init\n");
	data[0]=0x00; data[1]=0x39; xbee_i2c_write(dev,0x7C,data,2);	// IS=1
	data[0]=0x00; data[1]=0x7E; xbee_i2c_write(dev,0x7C,data,2);	// �R���g���X�g	E
	data[0]=0x00; data[1]=0x5D; xbee_i2c_write(dev,0x7C,data,2);	// Power/Cont	D
	data[0]=0x00; data[1]=0x6C; xbee_i2c_write(dev,0x7C,data,2);	// FollowerCtrl	C
	delay(200);
	data[0]=0x00; data[1]=0x38; xbee_i2c_write(dev,0x7C,data,2);	// IS=0
	data[0]=0x00; data[1]=0x0C; xbee_i2c_write(dev,0x7C,data,2);	// DisplayON	C

	print_data(dev,lcd[0],0,"setup");
	print_data(dev,lcd[1],1,"setup");
	
	goto_sleep(dev,"setup");
	print_timer();printf("setup\tDone\n");
	xbee_clear_cache();			// ��M�L���b�V���̃N���A

	// loop ����
	while(1){
		if ( kbhit() ){
			j = (byte)getchar();
			if(j == (byte)'\n'){
				for(i=lcd_x;i<8;i++) lcd[lcd_y][i]=' ';
				printf("\nDisplay(%d) '%s'\n",lcd_y+1,lcd[lcd_y]);
				if( disp_sw < 2 ) disp_sw++;
				print_timer();printf("loop\tWaiting for XBee\n");
				lcd_x=0;
				lcd_y++;
				if(lcd_y > 1) lcd_y=0;
			}else if(j >= 0x20 && lcd_x<8){
				lcd[lcd_y][lcd_x]=(char)j;
				lcd_x++;
			}
		}
		
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B
		
		// ��M���̏���
		if(bytecmp(dev, xbee_result.FROM ,8 )==0){
			print_timer();printf("loop\tFound XBee Packet\n");
			if( disp_sw ){
				wakeup(dev,"resp");
				if(disp_sw==1){
					if(lcd_y) lcd_y=0; else lcd_y=1;
					print_data(dev,lcd[lcd_y],lcd_y,"resp");
					if(lcd_y) lcd_y=0; else lcd_y=1;
				}else{	// disp_sw==2
					print_data(dev,lcd[0],0,"resp");
					print_data(dev,lcd[1],1,"resp");
				}
				goto_sleep(dev,"resp");
				disp_sw=0;
			}
			xbee_clear_cache();			// ��M�L���b�V���̃N���A
		}
	}
}

#endif
