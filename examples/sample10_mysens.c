/*********************************************************************
�T���v���A�v��10�@����Z���T�[
�V�����Z���T�[�f�o�C�X�𔭌�����ƁA���̃f�o�C�X����Ɠx�Ɖ��x�𓾂܂��B

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2011 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port�w��	IO��	�s���ԍ�			�Z���T�[�f�o�C�X
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 �Ɠx�Z���T�[
			port= 2		DIO2	XBee_pin 18 (AD2)	 ���x�Z���T�[
			port= 3		DIO3	XBee_pin 17 (AD3)	 ���x�Z���T�[
*/

//#define	H3694			//�d�v�FH3694��define�����H8�}�C�R���p�ɂȂ�܂��B
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
	unsigned int trig=0;
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
		xbee_gpio_config( dev_sens, 3 , AIN ); // �ڑ�����̃|�[�g3��AIN���͂�
	}else{
		lcd_disp("Failed:no dev.");
	}
	wait_millisec( 1000 );
	
	// ���C������
	lcd_disp("Receiving");
	while(1){							// �i�v�Ɏ�M����
		if( dev_en && trig == 0){
			xbee_force( dev_sens );		// �f�o�C�Xdev_sens�փf�[�^�v��(100���1��̕p�x��)
			trig = 1000;
			lcd_disp("forced");
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
				if( xbee_result.STATUS == STATUS_OK){
					/* �Ɠx���茋��
					    �Ɠx�Z���T    [+3.3V]----(C)[�Ɠx�Z���T](E)--+--[10k��]----[GND]
					    =NJL7502L                                    |
					                                                 +--(19)[XBee ADC1]
						lux = I(uA) / 0.3 = ADC(mV) / 10(kOhm) / 0.3
						    = ADC�l /1023 * 1200(mV) / 10(kOhm) / 0.3
						    = ADC�l * 0.391
					*/
					value = (float)xbee_result.ADCIN[1] * 0.391;
					lcd_cls();	lcd_goto(LCD_ROW_1);lcd_disp_5( (unsigned int)value );	lcd_putstr("Lux ");
					lcd_disp_5( xbee_result.ADCIN[1] );
					/* ���x���茋��
					    ���x�Z���T    [+3.3V]----(1VSS)[���x�Z���T](3GND)---[GND]
					    =LM61CIZ                      (2VOUT)|
					                                         +--(18)[XBee ADC2]
						Temp = ( ADC(mV) - 600 ) / 10 = ( ADC�l /1023 * 1200(mV) - 600 ) / 10
						     = ADC�l * 0.1173 - 60;
					*/
					value = (float)xbee_result.ADCIN[2] * 0.1173 - 60;
					i = (byte)value; lcd_goto(LCD_ROW_2); lcd_disp_2( i ); lcd_putstr(".");
					i = (byte)((value-(float)i)*100); lcd_disp_2( i ); lcd_putstr("degC");
					lcd_disp_5( xbee_result.ADCIN[2] );
				}else lcd_disp("no responce");
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				xbee_from( dev_sens );	// ��M�����A�h���X��dev_sens�֊i�[
				dev_en = 1;				// sensor������
				lcd_disp("found a new device");
				break;
			default:
				break;
		}
	}
}
