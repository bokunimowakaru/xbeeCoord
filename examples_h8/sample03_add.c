/*********************************************************************
�T���v���A�v���R�@�X�C�b�`
�q�@�̃X�C�b�`2�`4(Port1�`3)�����������ɁA���̏�Ԃ��t���ɕ\�����܂��B
�V�����f�o�C�X�𔭌�����ƁA���̃f�o�C�X��GPIO�̐ݒ��ύX���܂��B
�i�����ɂQ�ȏ�̃f�o�C�X�̃X�C�b�`��Ԃ����o�ł��܂��j
�V����XBee�q�@�̃R�~�b�V�����{�^��DIO0(XBee_pin 20)����������(�M�����x
����H��L��H�ɐ��ڂ���)�ƁA�q�@����Q���v���M�������s����āAArduino�e�@
���q�@�𔭌����A�ufound a device�v�ƕ\�����܂��B
�n�߂́A��x�A�y�A�����O�����Ă���A�ēx�R�~�b�V�����{�^���������K�v��
����܂��B

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port�w��	IO��	�s���ԍ�			USB�]���{�[�h(XBIB-U-Dev)
			port= 0		DIO0	XBee_pin 20 (Cms)	    SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	    SW2
			port= 2		DIO2	XBee_pin 18 (AD2)	    SW3
			port= 3		DIO3	XBee_pin 17 (AD3)	    SW4
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
	byte data;
	XBEE_RESULT xbee_result;

	byte port=0;		//�@�����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_gpio[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};	// PIO �f�o�C�X
	
	// ����������
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	xbee_atnj( 0xFF );					// �f�o�C�X����ɎQ���󂯓���(�e�X�g�p)
	xbee_gpio_init(dev_gpio);			// �f�o�C�Xdev_gpio��IO�ݒ���s�����߂̑��M
	
	// ���C������
	lcd_disp("Receiving");
	while(1){							// �i�v�Ɏ�M����
		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;			// ������M���[�h�ɐݒ�
		data = xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B
										// data�F��M���ʂ̑�\�l
										// xbee_result�F�ڍׂȎ�M����
										// xbee_result.MODE�F��M���[�h
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_GPIN:				// GPIO���͂����o(�����[�g�@�̃{�^���������ꂽ)
				lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("SW ");
				for( i=1;i<=3;i++ ){					// �X�C�b�`�̏�Ԃ�\��
					lcd_disp_1(i); lcd_putch(':');
					lcd_disp_1( (data>>i)&0x01 ); lcd_putch(' ');
				}
				/* �ȉ��͏ڍׂȎ�M���ʂ̗� */
				lcd_goto(LCD_ROW_2); lcd_putstr("D ");	// GPIO���͒l(�f�W�^��)��\��
				lcd_disp_bin( xbee_result.GPI.BYTE[1] ); lcd_putch(' ');
				lcd_disp_bin( xbee_result.GPI.BYTE[0] ); 
				lcd_goto(LCD_ROW_3); lcd_putstr("A ");	// ADC���͒l(�A�i���O)��\��
				for( i=0;i<3;i++ ){ lcd_disp_5( xbee_result.ADCIN[i] ); // �A���A�v�ݒ�
					lcd_putch(' ');	}
				lcd_goto(LCD_ROW_4); lcd_putstr("@ ");	// ���M���A�h���X��\��
				for( i=0;i<4;i++ ) lcd_disp_hex( xbee_result.FROM[i] );
				lcd_putch(' ');
				for( i=4;i<8;i++ ) lcd_disp_hex( xbee_result.FROM[i] );
				lcd_putch('\n');
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				xbee_from( dev_gpio );	// ���������A�h���X��dev_gpio�ɓǂݍ���
				xbee_gpio_init( dev_gpio );
				lcd_disp("found a new device");
				break;
			default:
				break;
		}
	}
}
