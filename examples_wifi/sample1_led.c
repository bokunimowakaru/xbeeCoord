/*********************************************************************
�T���v���A�v���P�@�����[�g���LED��ON/OFF����B
�N����T�b���炢����Ɓuinput�v�̕\���������̂�0�`2�̐���������
���^�[���L�[�������B
�P�������LED1���_���A�Q�������LED2���_���A0�ŗ����������܂��B
�����[�g��XBee�q�@�́ALED1��DIO11(XBee_pin 7)�ɁALED2��DIO12(XBee_
pin 4)�ɐڑ����Ă����܂��B

XBee Wi-Fi�Ɋւ��ẮA��؂̃T�|�[�g���������܂���B
���ȒP�Ɉ�����XBee ZB�������߂������܂��B
�g���u�����̃}�j���A���͉��L���Q�Ƃ��������B
http://www.geocities.jp/bokunimowakaru/download/xbee/README_wifi.txt

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port�w��	IO��	�s���ԍ�			USB�]���{�[�h(XBIB-U-Dev)
			port=11		DIO11	XBee_pin  7			    LED2	��port11��LED2
			port=12		DIO12	XBee_pin  4			    LED1	��port12��LED1
*/
// #define DEBUG			// �f�o�b�O�p
// #define DEBUG_TX		// ���M�p�P�b�g�̊m�F�p
// #define DEBUG_RX		// ��M�p�P�b�g�̊m�F�p

#include "../libs/xbee_wifi.c"

int main(int argc,char **argv){
	char s[5];
	byte port=0;		//�@�����莝����XBee���W���[����IP�A�h���X�ɕύX����
	byte dev_gpio[] = {192,168,0,135};                  // �q�@XBee
	byte dev_my[]   = {192,168,0,255};                  // �e�@�p�\�R��
	
	xbee_init( port );					// XBee�pCOM�|�[�g�̏�����(�����̓|�[�g�ԍ�)
	xbee_myaddress(dev_my);				// �����̃A�h���X��ݒ肷��
	xbee_gpio_init(dev_gpio);			// �f�o�C�Xdev_gpio��IO�ݒ���s�����߂̑��M
	// ���C������
	while(1){									// �i�v�Ɏ�M����
		printf("input[0-2]:");
		fgets( s ,5 , stdin);
		switch( s[0] ){
			case '0':
				xbee_gpo(dev_gpio,12,1);		// GPO�|�[�g12��H���x��(����)��
				xbee_gpo(dev_gpio,11,1);		// GPO�|�[�g11��H���x��(����)��
				break;
			case '1':
				xbee_gpo(dev_gpio,12,0);		// GPO�|�[�g12��L���x��(�_��)��
				break;
			case '2':
				xbee_gpo(dev_gpio,11,0);		// GPO�|�[�g11��L���x��(�_��)��
				break;
		}
	}
}
