/*********************************************************************
�T���v���A�v��13 �H�� ���^LCD I2C

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

���^�t���ɕ\�������܂Łu��Q�����v�قǂ�����܂��B

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

�������x

	XBee Coord	�N��		��10�b
	LCD Init	������ 		��50�b
	LCD Data 1	1�s�ڕ\��	��45�b
	LCD Data 1	1�s�ڕ\��	��45�b
*/

#include "../libs/xbee.c"
#ifdef LITE	// BeeBee Lite by ��
	int main(){
		printf("Lite mode dewaugokimasen. gomenne.");
		return 0;
	}
#else

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

int main(int argc,char **argv){
	char lcd1[]="�޸��ܶ�";				// �t��1�s��(8����)
	char lcd2[]=" XBee ZB";				// �t��2�s��(8����)
	byte data[2];
	byte i;	//�@�����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};	// PIO �f�o�C�X

	byte port=0;
	
	// ����������
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// XBee�pCOM�|�[�g�̏�����(�����̓|�[�g�ԍ�)
	xbee_atnj( 0xFF );					// �f�o�C�X����ɎQ���󂯓���(�e�X�g�p)
	xbee_i2c_init(dev);					// I2C�C���^�t�F�[�X�̏�����
	
	// ���C������
	
	print_timer();printf("LCD Init\n");
//	data[0]=0x00; data[1]=0x38; xbee_i2c_write(dev,0x7C,data,2);	// IS=0
	data[0]=0x00; data[1]=0x39; xbee_i2c_write(dev,0x7C,data,2);	// IS=1
//	data[0]=0x00; data[1]=0x14; xbee_i2c_write(dev,0x7C,data,2);	// OSC Freq.	4
	data[0]=0x00; data[1]=0x7E; xbee_i2c_write(dev,0x7C,data,2);	// �R���g���X�g	E
	data[0]=0x00; data[1]=0x5D; xbee_i2c_write(dev,0x7C,data,2);	// Power/Cont	D
	data[0]=0x00; data[1]=0x6C; xbee_i2c_write(dev,0x7C,data,2);	// FollowerCtrl	C
	delay(200);
	data[0]=0x00; data[1]=0x38; xbee_i2c_write(dev,0x7C,data,2);	// IS=0
	data[0]=0x00; data[1]=0x0C; xbee_i2c_write(dev,0x7C,data,2);	// DisplayON	C
//	data[0]=0x00; data[1]=0x01; xbee_i2c_write(dev,0x7C,data,2);	// ClearDisplay
//	data[0]=0x00; data[1]=0x06; xbee_i2c_write(dev,0x7C,data,2);	// EntryMode	6
	
	print_timer();printf("LCD Data 1\n");
	data[0]=0x00; data[1]=0x80; xbee_i2c_write(dev,0x7C,data,2);	// 1�s��
	for(i=0;i<8;i++){
		data[0]=0x40;
		data[1]=(byte)lcd1[i];
		xbee_i2c_write(dev,0x7C,data,2);
	}
	
	print_timer();printf("LCD Data 2\n");
	data[0]=0x00; data[1]=0xC0; xbee_i2c_write(dev,0x7C,data,2);	// 2�s��
	for(i=0;i<8;i++){
		data[0]=0x40;
		data[1]=(byte)lcd2[i];
		xbee_i2c_write(dev,0x7C,data,2);
	}
	
	print_timer();printf("Done\n");
	return(0);
}

#endif
