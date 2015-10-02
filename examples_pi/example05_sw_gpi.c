/***************************************************************************************
�X�C�b�`��Ԃ������[�g�擾����@�����擾

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

// ���莝����XBee���W���[���q�@��IEEE�A�h���X�ɕύX���遫
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){
    byte com=0;                         // �V���A��COM�|�[�g�ԍ�
    byte value;                         // �����[�g�q�@����̓��͒l
    
    if(argc==2) com=(byte)atoi(argv[1]);// ����������Εϐ�com�ɑ������
    xbee_init( com );                   // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                  // �e�@XBee����ɃW���C������Ԃɂ���

    while(1){                           // �J��Ԃ�����
        /* XBee�ʐM */
        value = xbee_gpi(dev,1);        // �����[�g�q�@�̃|�[�g1����f�W�^���l�����
        printf("Value =%d\n",value);    // �ϐ�value�̒l��\������
        delay( 1000 );                  // 1000ms(1�b��)�̑҂�
    }
}
