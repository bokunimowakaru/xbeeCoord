/*********************************************************************
�T���v���A�v���P�@�����[�g���LED��ON/OFF����B
�N����T�b���炢����Ɓuinput�v�̕\���������̂�0�`2�̐���������
���^�[���L�[�������B
�P�������LED1���_���A�Q�������LED2���_���A0�ŗ����������܂��B
�����[�g��XBee�q�@�́ALED1��DIO11(XBee_pin 7)�ɁALED2��DIO12(XBee_
pin 4)�ɐڑ����Ă����܂��B

����F���L�̃T�C�g�ɂ���񂪌f�ڂ���Ă��܂��B
http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-tutorial_2.html

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port�w��	IO��	�s���ԍ�			USB�]���{�[�h(XBIB-U-Dev)
			port=11		DIO11	XBee_pin  7			    LED2	��port11��LED2
			port=12		DIO12	XBee_pin  4			    LED1	��port12��LED1
*/

#include "../libs/xbee.c"

int main(int argc,char **argv){
	char s[4];
	byte port=0;		//�@�����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_gpio[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};	// PIO �f�o�C�X
	
	// ����������
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// XBee�pCOM�|�[�g�̏�����(�����̓|�[�g�ԍ�)
	xbee_atnj( 0xFF );					// �f�o�C�X����ɎQ���󂯓���(�e�X�g�p)
	
	// ���C������
	while(1){							// �i�v�Ɏ�M����
		printf("input[0-2]:");
		fgets(s, 4, stdin);
		switch( s[0] ){
			case '0':
				xbee_gpo(dev_gpio,12,1);		// GPO�|�[�g12��H���x��(3.3V)��
				xbee_gpo(dev_gpio,11,1);		// GPO�|�[�g11��H���x��(3.3V)��
				break;
			case '1':
				xbee_gpo(dev_gpio,12,0);		// GPO�|�[�g12��L���x��(0V)��
				break;
			case '2':
				xbee_gpo(dev_gpio,11,0);		// GPO�|�[�g11��L���x��(0V)��
				break;
		}
	}
}
