/***************************************************************************************
XBee��LED��_�ł����Ă݂�F�p�\�R���ɐڑ������e�@XBee��RSSI LED��_��

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"               // XBee���C�u�����̃C���|�[�g

int main(int argc,char **argv){
	
//  byte com=0xA0;                      // �V���A��(�g���R�l�N�^)
    byte com=0xB0;                      // �V���A��(USB)
    
    if(argc==2) com=(byte)atoi(argv[1]);// ����������Εϐ�com�ɑ������
    xbee_init( com );                   // XBee�pCOM�|�[�g�̏�����(�����̓|�[�g�ԍ�)
    
    while(1){                           // �J��Ԃ�����
        xbee_at("ATP005");              // ���[�J��AT�R�}���hATP0(DIO10�ݒ�)=05(�o��'H')
        delay( 1000 );                  // ��1000ms(1�b��)�̑҂�
        xbee_at("ATP004");              // ���[�J��AT�R�}���hATP0(DIO10�ݒ�)=04(�o��'L')
        delay( 1000 );                  // ��1000ms(1�b��)�̑҂�
    }
}
