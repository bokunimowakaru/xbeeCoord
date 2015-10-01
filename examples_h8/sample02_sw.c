/*********************************************************************
�T���v���A�v���Q�@�X�C�b�`
�q�@�̃X�C�b�`2�`4(Port1�`3)�����������ɁA���̏�Ԃ��t���ɕ\�����܂��B
XBee�q�@��DIO1�`3(XBee_pin 19�`17)��High �� Low �ɂ���� PC�܂���H8��
�e�@�ɃX�C�b�`�̏�Ԃ��\������܂��B

���փ`���C���i�q�@�j�Ȃǂ̌��o�p�Ɏg�p���܂��B

����F���L�̃T�C�g�ɂ���񂪌f�ڂ���Ă��܂��B
http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-tutorial_2.html

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
			
	�q�@��End Device�ɂ���ꍇ�AX-CTU���g�p���Ďq�@XBee�Ɉȉ���ݒ肵�Ă����܂��B
	SP��SN�͐e�@�ɂ��ݒ肵�Ă����܂��B
		
			DH: �e�@(����)�̏�ʃA�h���X
			DL: �e�@(����)�̉��ʃA�h���X
			SM: Sleep Mode			= 05   hex (CYCLIC SLEEP PIN-WAKE)
			SP: Cyclic Sleep Period	= 07D0 hex (20�b)
			SN: Sleep Number 		= 0E10 hex (3600��)
			D1: AD1/DIO1 Config.	= 03   hex (DIGITAL INPUT)
			PR: Pull-up Resistor	= 1FFF hex (�v���A�b�v�L��)
			IC: I/O Change Detection= 000E hex (port 1�`3�����o�ݒ�)
			
			�܂��A�N���p�̃{�^����SLEEP_RQ(XBee_pin 9)�ɐڑ����܂��B
			�Ⴆ�΁A�{�^��port=1�Ƃ���ꍇ�ASLEEP_RQ(XBee_pin 9)��
			DIO1(XBee_pin 19)�ƊԂ�ڑ����Ă����܂��B
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
	byte data;
	XBEE_RESULT xbee_result;

	byte port=0;		//�@�����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_gpio[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};	// PIO �f�o�C�X
	
	// ����������
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// XBee�pCOM�|�[�g�̏�����(�����̓|�[�g�ԍ�)
	xbee_atnj( 0xFF );					// �f�o�C�X����ɎQ���󂯓���(�e�X�g�p)
	xbee_gpio_init(dev_gpio);			// �f�o�C�Xdev_gpio��IO�ݒ���s�����߂̑��M
	
	// ���C������
	lcd_disp("Waiting"); lcd_home();
	while(1){
		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;			// ������M���[�h�ɐݒ�
		data = xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B
												// data�F��M���ʂ��������܂�
		if( xbee_result.MODE == MODE_GPIN){		// PIO����(�����[�g�@�̃{�^���������ꂽ)�̎�
			lcd_disp_bin( data );				// data�ɓ������l���o�C�i���ŕ\��
			lcd_home();
		}
	}
}
