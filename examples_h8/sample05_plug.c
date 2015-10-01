/*********************************************************************
�T���v���A�v���T�@�X�}�[�g�v���O

XBee Smart Plug(Digi �����i)�𔭌�����ƁA���̃f�o�C�X����Ɠx�Ə���
�d�͂𓾂܂��B�Â��Ȃ��Smart Plug�ɐڑ������@��̓d�����؂�܂��B

�V����XBee�q�@�̃R�~�b�V�����{�^��DIO0(XBee_pin 20)����������(�M�����x
����H��L��H�ɐ��ڂ���)�ƁA�q�@����Q���v���M�������s����āAPC�܂���
H8�e�@���q�@�𔭌����A�ufound a device�v�ƕ\�����܂��B

�������A������ZigBee�l�b�g���[�N�ɑΉ����邽�߂ɁA�y�A�����O�͋N�����
30�b�Ԃ����󂯕t���܂���BPC/H8�e�@���N�����āA�R�~�b�V�����{�^����
�����ăy�A�����O�����{���Ă����A�ēx�A�R�~�b�V�����{�^���Ńf�o�C�X����
���܂��B�f�o�C�X������30�b�̃y�A�����O���Ԍ�ł��s���܂��B

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

�����ӁF�p�\�R����t�@���q�[�^�Ȃ�AC�d���̓r�₦��ƌ̏�⎖�̂̔�������
�@�@���Smart Plug�ɐڑ����Ȃ����ƁB
                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port�w��	IO��	�s���ԍ�			�Z���T�[�f�o�C�X
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 �Ɠx�Z���T�[
			port= 2		DIO2	XBee_pin 18 (AD2)	 ���x�Z���T�[
			port= 3		DIO3	XBee_pin 17 (AD3)	 AC�d���Z���T�[
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
	unsigned int value_i;

	byte port=0;		// �����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// PIO �f�o�C�X
	
	// ����������
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	
	// �f�o�C�X�T��
	lcd_disp("Searching:Smart Plug");
	if( xbee_atnj(30) ){				// 30�b�Ԃ̎Q���󂯓���
		lcd_disp("Found device");
		xbee_from( dev_sens );			// ��M�����A�h���X��dev_sens�֊i�[
		dev_en = 1;						// sensor������
		xbee_gpio_config( dev_sens, 1 , AIN ); // �ڑ�����̃|�[�g1��AIN���͂�
		xbee_gpio_config( dev_sens, 2 , AIN ); // �ڑ�����̃|�[�g2��AIN���͂�
		xbee_gpio_config( dev_sens, 3 , AIN ); // �ڑ�����̃|�[�g3��AIN���͂�
	}else{
		lcd_disp("Failed:no dev.");
		exit(-1);
	}
	wait_millisec( 1000 );
	
	// ���C������
	lcd_disp("Receiving");
	while(1){							// �i�v�Ɏ�M����
		if( dev_en && trig == 0){
			xbee_force( dev_sens );		// �f�o�C�Xdev_sens�փf�[�^�v��(100���1��̕p�x��)
			trig = 100;
		}
		trig--;
		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;	// ������M���[�h�ɐݒ�
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B(������50ms������܂�)
										// data�F��M���ʂ̑�\�l
										// xbee_result�F�ڍׂȎ�M����
										// xbee_result.MODE�F��M���[�h
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// sensor����̉����̏ꍇ�ɏƓx�Ɖ��x��\��
			case MODE_GPIN:				// ���������Ńf�[�^��M�����ꍇ���z��
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 &&
				xbee_result.STATUS == STATUS_OK ){
					// �Ɠx���茋��
					value = xbee_sensor_result( &xbee_result,LIGHT);	
					lcd_cls(); lcd_goto(LCD_ROW_1); lcd_disp_5( (unsigned int)value ); lcd_putstr(" Lux");
					// �Â��Ƃ��͓d����؂�A���邢�Ƃ��͓d��������
					if( value < 1.0 ){
						xbee_gpo(dev_sens , 4 , 0 );	// Turns outlet off (Port 4 = 0)
					}else{
						xbee_gpo(dev_sens , 4 , 1 );	// Turns outlet on (Port 4 = 1)
					}
					// ���x���茋�� (�����Ŋm�F�����Ƃ���7.12���̍��߂��\������܂����ADigi�Ђ̎d�l�ł��B)
					value = xbee_sensor_result( &xbee_result,TEMP);
					i = (byte)value; lcd_goto(LCD_ROW_2); lcd_disp_2( i ); lcd_putstr(".");
					i = (byte)((value-(float)i)*100); lcd_disp_2( i ); lcd_putstr(" degC");
					// �d�͑��茋��
					value = xbee_sensor_result( &xbee_result,WATT);
					if( value < 1.0 ) value_i = 0;
					else value_i = (unsigned int)value;
					lcd_goto(LCD_ROW_3); lcd_disp_3( value_i ); lcd_putstr(" Watts"); lcd_putch('\n');
				}
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				lcd_disp("found a new device");
				xbee_from( dev_sens );	// ��M�����A�h���X��dev_sens�֊i�[
				dev_en = 1;				// sensor������
				xbee_gpio_config( dev_sens, 1 , AIN ); // �ڑ�����̃|�[�g1��AIN���͂�
				xbee_gpio_config( dev_sens, 2 , AIN ); // �ڑ�����̃|�[�g2��AIN���͂�
				xbee_gpio_config( dev_sens, 3 , AIN ); // �ڑ�����̃|�[�g3��AIN���͂�
				break;
			default:
				break;
		}
	}
}
