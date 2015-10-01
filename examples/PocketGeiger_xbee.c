/*********************************************************************
�|�P�b�g�K�C�K�[����p

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

// #define XBEE_END_DEVICE		// XBee�ɃG���h�f�o�C�X���g�p����Ƃ��̐ݒ�

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
//	byte dev_en = 0;	// �Z���T�[�����̗L��(0:������)
	XBEE_RESULT xbee_result;

	time_t timer;
	struct tm *time_st;

	byte port=0;		// �����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// PocketGeiger
	
	int signCount=0;	//�M���̃J�E���g��
	int noiseCount=0;	//�m�C�Y�̃J�E���g��
	#ifdef XBEE_END_DEVICE
	byte signValue=0;	//�O���sign�l 0 or 1
	#endif

	float cpm = 0; //���݂̃J�E���g��
	float cpmHistory[200]; //�J�E���g�񐔂̗���
	byte cpmIndex=0;//�J�E���g�񐔂̌��ݔz��̊i�[�ʒu

	//cpm�v�Z�p�̎���
	int cpmTimeSec=0;

	//�o�͉��Z�p
	float cpmBuff;
	float uSvBuff;
//	float uSvdBuff;

	float min;

	// ����������
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	
	// �f�o�C�X�T��
	lcd_disp("Searching:PocketGeiger");
	if( xbee_atnj(30) ){				// 30�b�Ԃ̎Q���󂯓���
		lcd_disp("Found device");
		xbee_from( dev_sens );			// ��M�����A�h���X��dev_sens�֊i�[
	//	dev_en = 1;						// sensor������
		xbee_gpio_init(dev_sens);		// �f�o�C�Xdev_sens��IO�ݒ���s�����߂̑��M(End Device�͐ݒ�ł���Ƃ͌���Ȃ�)
	}else{
		lcd_disp("Failed:no dev.");
		exit(-1);
	}
	
	for(i=0; i<200;i++ ) cpmHistory[i]=0;

	lcd_disp("Measuring...");
	
	do{
		time(&timer);
		time_st = localtime(&timer);
	}while( (time_st->tm_sec)%6 != 0 );
	trig = time_st->tm_sec + 6 ;		// ���ݎ����{�U�b
	if( trig >= 60 ) trig = 0;
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
			default:
				break;
		}
		
		if( (time_st->tm_sec >= trig && trig > 0) || (time_st->tm_sec < 54 && trig == 0) ){
			trig = time_st->tm_sec + 6 ;	// ���ݎ����{�U�b
			if( trig >= 60 ) trig = 0;
			if(noiseCount == 0){				//�m�C�Y�����o����Ȃ��Ƃ��i�m�C�Y�����o���ꂽ�Ƃ��́A���������Ȃ��j
				cpmIndex++;
				if(cpmIndex >= 200){        //�Ō�(20����)�܂ł����猳�ɖ߂�
					cpmIndex = 0;
				}
				//������Ă����Ƃ��Ɏ��Ɋi�[����z��ɒl���l�܂��Ă���΁A
				//���݂̍��v���Ă���J�E���g(�ϐ�cpm)��������āA�����������Ƃɂ���
				if(cpmHistory[cpmIndex] > 0){
					cpm -= cpmHistory[cpmIndex];
				}

				cpmHistory[cpmIndex] = signCount;
				cpm += signCount;				//�J�E���g�񐔂̒~��
				
				if( cpmTimeSec >= 20*60 ){		//cpm�����߂�Ƃ��Ɏg�p����v������(�ő�20��)�����Z
					cpmTimeSec = 20*60;
				}else{
					cpmTimeSec += 6 ;
				}
			}
			min = cpmTimeSec / 60.0;      //���݂̌v������(�ő�20��)
			if(min!=0){        //cpm�AuSv/h�AuSv/h�̌덷�����ꂼ��v�Z����
				cpmBuff = cpm / min;
				uSvBuff = cpm / min / alpha;
			//	uSvdBuff = sqrt(cpm) / min / alpha;
			}else{        //0����Z�̂Ƃ��́A0�ɂ���
				cpmBuff = 0;
				uSvBuff = 0;
			//	uSvdBuff = 0;
			}
			//�t���ւ̕\��
			printf("%02d:%02d:%02d ",time_st->tm_hour,time_st->tm_min,time_st->tm_sec);
			printf("%.2f uSv/h ", uSvBuff );
			printf("(%.1f cpm) ", cpmBuff );
			printf("s=%d n=%d\n", signCount, noiseCount );
			
			//���̃��[�v�̂��߂̕ϐ��̏�����
			signCount=0;
			noiseCount=0;
		}
	}
}
