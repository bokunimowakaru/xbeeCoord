/*********************************************************************
�|�P�b�g�K�C�K�[����p
FET SW�Ń|�P�K�̓d����ON/OFF����

�e�@XBee���̑���p�\�[�X�R�[�h�ł��B

�����̃\�[�X��PC�p�ł��B������H8�ł͓��삵�܂��񁡁���

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B
XBee���C�u������Ver 1.5B�ȍ~���g�p���Ă��������B

�Ȃ��ARadiation-Watch.org�̃\�[�X�R�[�h���܂݂܂��B

�����ӁF�p�\�R����t�@���q�[�^�Ȃ�AC�d���̓r�₦��ƌ̏�⎖�̂̔�������
�@�@���Smart Plug�ɐڑ����Ȃ����ƁB
                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////
/*

Radiation-watch.org�v���W�F�N�g�͊J�����ʂ��N���G�C�e�B�u�E�R�����Y(CC 
BY 3.0)���C�Z���X�̌��ŃI�[�v���\�[�X�����Ă��܂��B�ȉ��ł́A�|�P�b�g
�K�C�K�[�̉�H�}�A�d�l�A�\�[�X�R�[�h�A�������ʂ��A�������J���܂��B
�����͑S���E�ɂ����āA���p�E�񏤗p���킸���R�ɍĔz�z�E���ρE���J��
�邱�Ƃ��ł��܂��B�Ȃ��ꕔ�̃R���e���c�͉p��܂��̓I�����_��݂̂ƂȂ�
�܂��B

POCKET GEIGER by Radiation-Watch.org is licensed under a Creative 
Commons Attribution 3.0 Unported License.

*/

/*
	port:	port�w��	IO��	�s���ԍ�			�Z���T�[�f�o�C�X
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 ���ː��p���X���́i���F�j
			port= 2		DIO2	XBee_pin 18 (AD2)	 �U���Z���T���́i�ΐF�j
			port= 3		DIO3	XBee_pin 17 (AD3)	 �Ȃ�
*/

//#define	H3694			//�d�v�FH3694��define�����H8�}�C�R���p�ɂȂ�܂��B
//#define ERRLOG			//ERRLOG��define����ƃG���[���O���o�͂���܂��B

/*
�݌v����
	�f�o�C�X�FEnd Device (���ː��p���X�̓g�O���d�l)�{�|�P�K�d��FET SW
	����Ԋu�F60���Ԃ�1��
	���莞�ԁF3����
*/

#define XBEE_END_DEVICE		// XBee�ɃG���h�f�o�C�X���g�p����Ƃ��̐ݒ�
#define TIME_INTERVAL	60
#define TIME_MEAS		3	// �ő�20��
#define DATA_NUM		TIME_MEAS * 60 / 6 + 4
		//	24�b��( 6�b * 4 )�̗]�����K�v(�d���I�t���Ă������ɃI�t�ɂȂ�Ȃ��̂�)

#include <math.h>

#ifdef H3694
	#ifndef __3694_H__
		#include <3694.h>
	#endif
	#include "../libs/lcd_h8.c"
#else
	#include "../libs/lcd_pc.c"
#endif
#include "../libs/xbee.c"

const float alpha=53.032; // cpm = uSv �~ alpha

int main(int argc,char **argv){
	byte i;
	byte trig=0;
	byte trig_min_s=0;
	byte trig_min_e=0;
	byte prev=0;
//	byte dev_en = 0;	// �Z���T�[�����̗L��(0:������)
	XBEE_RESULT xbee_result;

	time_t timer;
	struct tm *time_st;

	byte port=0;		// �����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// PocketGeiger
	
	int signCount=0;	//�M���̃J�E���g��
	int noiseCount=0;	//�m�C�Y�̃J�E���g��
	byte signValue=0;	//�O���sign�l 0 or 1

	float cpm = 0; //���݂̃J�E���g��
	float cpmHistory[DATA_NUM]; //�J�E���g�񐔂̗���
	byte cpmIndex=0;//�J�E���g�񐔂̌��ݔz��̊i�[�ʒu

	//cpm�v�Z�p�̎���
	int cpmTimeSec=0;

	//�o�͉��Z�p
	float cpmBuff=0.;
	float uSvBuff=0.;
//	float uSvdBuff=0.;

	float min;

	// ����������
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	xbee_at( "ATSP07D0");				// End Device�p�f�[�^�ێ����Ԃ�20�b�ɐݒ�
	
	// �f�o�C�X�T��
	lcd_disp("Searching:PocketGeiger");
	if( xbee_atnj(30) ){				// 30�b�Ԃ̎Q���󂯓���
		lcd_disp("Found device");
		xbee_from( dev_sens );			// ��M�����A�h���X��dev_sens�֊i�[
	//	dev_en = 1;						// sensor������
		xbee_gpio_init(dev_sens);		// �f�o�C�Xdev_sens��IO�ݒ���s�����߂̑��M(End Device�͐ݒ�ł���Ƃ͌���Ȃ�)
		xbee_rat( dev_sens , "ATD405");	// �|�P�b�g�K�C�K��OFF�ɂ���
		xbee_rat( dev_sens , "ATSM05");	// Cyclic Sleep wakes on timer expiration�ɐݒ�
		xbee_rat( dev_sens , "ATSP07D0");	// ���M�Ԋu��20�b�ɐݒ�
	}else{
		lcd_disp("Failed:no dev.");
		exit(-1);
	}
	
	for(i=0; i<DATA_NUM;i++ ) cpmHistory[i]=0;

	lcd_disp("Started...");
	time(&timer);
	time_st = localtime(&timer);
	printf("%02d:%02d:%02d \n",time_st->tm_hour,time_st->tm_min,time_st->tm_sec);
	do{
		time(&timer);
		time_st = localtime(&timer);
	}while( (time_st->tm_sec) != 0 );
	
	lcd_disp("Measuring...");
	trig = 1 ;						// ���蒆
	prev = 0 ;						// �O��̑��莞���̕b
	trig_min_s = time_st->tm_min ;	// �J�n�̕����L�^
	trig_min_e = time_st->tm_min + TIME_MEAS;	// �I���̕����L�^
	if( trig_min_e >= 60 ) trig_min_e -= 60;
	xbee_rat( dev_sens , "ATD404");	// �|�P�b�g�K�C�K��ON�ɂ���
	printf("%02d:%02d:%02d ---- uSv/h (--- cpm)\n",time_st->tm_hour,time_st->tm_min,time_st->tm_sec);
	
	// ���C������
	while(1){							// �i�v�Ɏ�M����
	
		time(&timer);
		time_st = localtime(&timer);
	
		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;	// ������M���[�h�ɐݒ�
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B(������50ms������܂�)
										// data�F��M���ʂ̑�\�l
										// xbee_result�F�ڍׂȎ�M����
										// xbee_result.MODE�F��M���[�h
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// sensor����̉����̏ꍇ�ɏƓx�Ɖ��x��\��
			case MODE_GPIN:				// ���������Ńf�[�^��M�����ꍇ���z��
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 ){
				//	printf("gpin\n");	// �e�X�g�p
					#ifdef XBEE_END_DEVICE
						signCount++;		//���ː��p���X(Low)���J�E���g
						if( xbee_result.GPI.PORT.D1 == signValue ){
							signCount++;	//�p�P�b�g���X�΍�
							printf("detected packet loss\n");
						}
						signValue = xbee_result.GPI.PORT.D1;					// ���݂̒l��ۑ�
					#else
						if( xbee_result.GPI.PORT.D1 == 0 ) signCount++;			//���ː��p���X(Low)���J�E���g
					#endif
					if( xbee_result.GPI.PORT.D2 == 1 ) {
						noiseCount++;		//�m�C�Y(High)���J�E���g
						printf("detected noise\n");
					}
				}else{
					printf("gpin from=");
					for( i=0 ; i<8 ; i++ ) printf("%02X ",xbee_result.FROM[i]);
					printf("\nstatus=[%x]\n",xbee_result.STATUS);
				}
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				lcd_disp("found a new device");
				xbee_from( dev_sens );	// ��M�����A�h���X��dev_sens�֊i�[
			//	dev_en = 1;				// sensor������
				xbee_gpio_init(dev_sens);	// �f�o�C�Xdev_sens��IO�ݒ���s�����߂̑��M
				xbee_rat( dev_sens , "ATD405");	// �|�P�b�g�K�C�K��OFF�ɂ���
				xbee_rat( dev_sens , "ATSM05");	// Cyclic Sleep wakes on timer expiration�ɐݒ�
				xbee_rat( dev_sens , "ATSP07D0");	// ���M�Ԋu��20�b�ɐݒ�
				break;
			default:
				break;
		}
		
		if( trig == 1 ){					// ���蒆
			if(time_st->tm_min == trig_min_e ){
				trig = 2;					// 20�b��ɒ�~����
				xbee_rat( dev_sens , "ATD405");	// �|�P�b�g�K�C�K��OFF�ɂ���
			}
		}else if( trig == 0 ){				// ���肵�Ă��Ȃ�
			if(time_st->tm_min == trig_min_s ){
				trig = 1;
				xbee_rat( dev_sens , "ATD404");	// �|�P�b�g�K�C�K��ON�ɂ���
				for(i=0; i<DATA_NUM;i++ ) cpmHistory[i]=0;
				cpmIndex = 0;
				cpm = 0;
				cpmTimeSec = 0;
			}
		}else{ // trig == 2					// 20�b��ɒ�~����
			if(time_st->tm_sec == 20 ){
				trig = 0;
			}
		}
		
		if( ((time_st->tm_sec)%6 == 0 ) && (prev != (time_st->tm_sec)) ){
			prev = time_st->tm_sec;
			if( trig != 0){
				if(noiseCount == 0){				//�m�C�Y�����o����Ȃ��Ƃ��i�m�C�Y�����o���ꂽ�Ƃ��́A���������Ȃ��j
					cpmIndex++;
					if(cpmIndex >= DATA_NUM){        //�Ō�(TIME_MEAS����)�܂ł����猳�ɖ߂�
						cpmIndex = 0;
					}
					//������Ă����Ƃ��Ɏ��Ɋi�[����z��ɒl���l�܂��Ă���΁A
					//���݂̍��v���Ă���J�E���g(�ϐ�cpm)��������āA�����������Ƃɂ���
					if(cpmHistory[cpmIndex] > 0){
						cpm -= cpmHistory[cpmIndex];
					}

					cpmHistory[cpmIndex] = signCount;
					cpm += signCount;				//�J�E���g�񐔂̒~��
					
					if( cpmTimeSec >= TIME_MEAS * 60 ){		//cpm�����߂�Ƃ��Ɏg�p����v������(�ő�TIME_MEAS��)�����Z
						cpmTimeSec = TIME_MEAS * 60;
					}else{
						cpmTimeSec += 6 ;
					}
				}
				min = cpmTimeSec / 60.0;      //���݂̌v������(�ő�TIME_MEAS��)
				if(min!=0){        //cpm�AuSv/h�AuSv/h�̌덷�����ꂼ��v�Z����
					cpmBuff = cpm / min;
					uSvBuff = cpm / min / alpha;
				//	uSvdBuff = sqrt(cpm) / min / alpha;
				}else{        //0����Z�̂Ƃ��́A0�ɂ���
					cpmBuff = 0;
					uSvBuff = 0;
				//	uSvdBuff = 0;
				}
			}
			//�t���ւ̕\��
			printf("%02d:%02d:%02d ",time_st->tm_hour,time_st->tm_min,time_st->tm_sec);
			if( trig == 0 ) printf("off "); else printf("ON  ");
			printf("%.2f uSv/h ", uSvBuff );
			printf("(%.1f cpm) ", cpmBuff );
			if( trig == 0 ) printf("\n"); else printf("s=%d n=%d\n", signCount, noiseCount );
			
			//���̃��[�v�̂��߂̕ϐ��̏�����
			signCount=0;
			noiseCount=0;
		}
	}
}
