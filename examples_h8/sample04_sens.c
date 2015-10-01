/*********************************************************************
�T���v���A�v���S�@�Z���T�[
�V�����Z���T�[�f�o�C�X�𔭌�����ƁA���̃f�o�C�X����Ɠx�Ɖ��x�𓾂܂��B

�����ł͎q�@��XBee�Z���T�[��Digi������XBee Sensor��z�肵�Ă��܂����A
���삵���Z���T�[�ł�DIO1�`2(XBee_pin 19�`18)��0�`1.2V�̃A�i���O�d����
�^���Ă���΁A���炩�̒l��ǂ݂Ƃ邱�Ƃ��ł��܂��B

�V����XBee�q�@�̃R�~�b�V�����{�^��DIO0(XBee_pin 20)����������(�M�����x
����H��L��H�ɐ��ڂ���)�ƁA�q�@����Q���v���M�������s����āAPC�܂���
H8�e�@���q�@�𔭌����A�ufound a device�v�ƕ\�����܂��B

�������A������ZigBee�l�b�g���[�N�ɑΉ����邽�߂ɁA�y�A�����O�͋N�����
10�b�Ԃ����󂯕t���܂���BPC/H8�e�@���N�����āA�R�~�b�V�����{�^����
�����ăy�A�����O�����{���Ă����A�ēx�A�R�~�b�V�����{�^���Ńf�o�C�X����
���܂��B�f�o�C�X������10�b�̃y�A�����O���Ԍ�ł��s���܂��B

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port�w��	IO��	�s���ԍ�			�Z���T�[�f�o�C�X
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 �Ɠx�Z���T�[
			port= 2		DIO2	XBee_pin 18 (AD2)	 ���x�Z���T�[
			port= 3		DIO3	XBee_pin 17 (AD3)	 ���x�Z���T�[
*/

#define	H3694			//�d�v�FH3694��define�����H8�}�C�R���p�ɂȂ�܂��B
//#define ERRLOG			//ERRLOG��define����ƃG���[���O���o�͂���܂��B

#ifdef H3694
	#ifndef __3694_H__
		#include <3694.h>
	#endif
	#include "../libs/lcd_h8.c"
#else
	#include "../libs/lcd_pc.c"
#endif
#include "../libs/xbee.c"

int main(int argc,char **argv){
	byte i;
	byte trig=0;
	byte dev_en = 0;	// �Z���T�[�����̗L��(0:������)
	XBEE_RESULT xbee_result;
	float value;

	byte port=0;		// �����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// �Z���T�f�o�C�X
	
	// ����������
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	
	// �f�o�C�X�T��
	lcd_disp("Searching:SENSOR");
	if( xbee_atnj(10) ){				// 10�b�Ԃ̎Q���󂯓���
		lcd_disp("Found device");
		xbee_from( dev_sens );			// ��M�����A�h���X��dev_sens�֊i�[
		dev_en = 1;						// sensor������
		xbee_gpio_config( dev_sens, 1 , AIN ); // �ڑ�����̃|�[�g1��AIN���͂�
		xbee_gpio_config( dev_sens, 2 , AIN ); // �ڑ�����̃|�[�g2��AIN���͂�
		// xbee_gpio_config( dev_sens, 3 , AIN ); // �ڑ�����̃|�[�g3��AIN���͂�
	}else{
		lcd_disp("Failed:no dev.");
	}
	
	// ���C������
	lcd_disp("Receiving");
	while(1){							// �i�v�Ɏ�M����
		if( dev_en && trig == 0){
			xbee_force( dev_sens );		// �f�o�C�Xdev_sens�փf�[�^�v��(100���1��̕p�x��)
			trig = 250;
		}
		trig--;

		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;	// ������M���[�h�ɐݒ�
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B
										// data�F��M���ʂ̑�\�l
										// xbee_result�F�ڍׂȎ�M����
										// xbee_result.MODE�F��M���[�h
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// sensor����̉����̏ꍇ�ɏƓx�Ɖ��x��\��
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 &&
				xbee_result.STATUS == STATUS_OK ){
					// �Ɠx���茋��
					value = xbee_sensor_result( &xbee_result,LIGHT);
					lcd_cls();	lcd_goto(LCD_ROW_1);lcd_disp_5( (unsigned int)value );	lcd_putstr("Lux ");
					// ���x���茋��
					value = xbee_sensor_result( &xbee_result,TEMP);
					i = (byte)value; lcd_disp_2( i ); lcd_putstr(".");
					i = (byte)((value-(float)i)*100); lcd_disp_2( i ); lcd_putstr("degC");
				}
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				lcd_disp("found a new device");
				xbee_from( dev_sens );	// ��M�����A�h���X��dev_sens�֊i�[
				dev_en = 1;				// sensor������
				xbee_gpio_config( dev_sens, 1 , AIN ); // �ڑ�����̃|�[�g1��AIN���͂�
				xbee_gpio_config( dev_sens, 2 , AIN ); // �ڑ�����̃|�[�g2��AIN���͂�
				// xbee_gpio_config( dev_sens, 3 , AIN ); // �ڑ�����̃|�[�g3��AIN���͂�
				break;
			default:
				break;
		}
	}
}
