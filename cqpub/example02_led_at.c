/***************************************************************************************
LED�������[�g���䂷��@�����[�gAT�R�}���h�F�����[�g�q�@��DIO4(XBee pin 11)��LED��_�ŁB

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

// ���莝����XBee���W���[���q�@��IEEE�A�h���X�ɕύX���遫
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){
    
    byte com=0;                         // �V���A��COM�|�[�g�ԍ�
    
    if(argc==2) com=(byte)atoi(argv[1]);// ����������Εϐ�com�ɑ������
    xbee_init( com );                   // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                  // �e�@XBee����ɃW���C������Ԃɂ���

    while(1){                           // �J��Ԃ�����
        xbee_rat(dev,"ATD405");         // �����[�gAT�R�}���hATD4(DIO4�ݒ�)=05(�o��'H')
        delay( 1000 );                  // 1000ms(1�b��)�̑҂�
        xbee_rat(dev,"ATD404");         // �����[�gAT�R�}���hATD4(DIO4�ݒ�)=04(�o��'L')
        delay( 1000 );                  // 1000ms(1�b��)�̑҂�
    }
}
