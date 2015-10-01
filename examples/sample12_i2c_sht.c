/*********************************************************************
�T���v���A�v��12 ���x�Z���T Sensirion SHT�p �f�W�^��IF�T���v��

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
xbee_i2c_read(const byte *address, byte adr,byte len,byte *rx){
	
�q�@XBee�f�o�C�X(�����[�g��)��I2C����w��o�C�g�̏���ǂݎ��֐�
���́Fbyte adr�@I2C�A�h���X
���́Fbyte len�@��M���A�O�̎��� Sensirion SHT�p�Q�o�C�g��M
�o�́Fbyte *rx�@��M�f�[�^�p�|�C���^
			port=11		DIO11	XBee_pin  7		SCL
			port=12		DIO12	XBee_pin  4		SDA

XBee�n�[�h�E�F�A
	Sensirion SHT-11�܂���SHT-71��XBee��1,4,7,10�ԃs���ɐڑ�����B
	�s���͂��傤��3�����Ȃ̂ŁA�����o���₷���Ǝv���܂��B
			XBee 1 -> SHT 2 (VCC)
			XBee 4 -> SHT 4 (SDA)
			XBee 7 -> SHT 1 (SCL)
			XBee10 -> SHT 3 (GND)
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
	byte data[2];
	byte i;	//�@�����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};	// PIO �f�o�C�X
	byte port=0;
	float temp,hum,hum_;
	
	// ����������
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// XBee�pCOM�|�[�g�̏�����(�����̓|�[�g�ԍ�)
	xbee_atnj( 0xFF );					// �f�o�C�X����ɎQ���󂯓���(�e�X�g�p)
	xbee_i2c_init(dev);					// I2C�C���^�t�F�[�X�̏�����
	
	// ���C������
	while(1){									// �i�v�Ɏ�M����
		print_timer();
		//	i= xbee_i2c_read(dev,0x51 ,1,data);
		i=xbee_i2c_read(dev,1,data,0);	// ���x�̓ǂݎ��
		if(i==2){
			temp = (float)data[0]*256. + (float)data[1];
			temp *= 0.01;
			temp -= 39.7;
			printf("SHT TEMP = %.2f ",temp);
		}else{
			printf("SHT TEMP = ERROR");
			temp=27.;
		}
		printf("(%02X,%02X)\n",data[0],data[1]);
		
		print_timer();
		i=xbee_i2c_read(dev,2,data,0);	// ���x�̓ǂݎ��
		if(i==2){
			hum_= (float)data[0]*256. + (float)data[1];
			hum = -1.5955e-6 * hum_ * hum_;
			hum += 0.0367 * hum_;
			hum -= 2.0468;
			hum += (temp - 25) * (0.01 + 0.00008 * hum_ );
			printf("SHT HUM  = %.2f ",hum);
		}else{
			printf("SHT HUM  = ERROR");
		}
		printf("(%02X,%02X)\n\n",data[0],data[1]);
	}
}

#endif
