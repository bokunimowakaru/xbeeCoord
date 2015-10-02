/***************************************************************************************
�A�i���O�d���������[�g�擾����B����q�@�̓����擾

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    unsigned int  value;                        // �����[�g�q�@����̓��͒l
    byte dev[8];                                // XBee�q�@�f�o�C�X�̃A�h���X

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    printf("Waiting for XBee Commissoning\n");  // �҂��󂯒��̕\��
    if(xbee_atnj(30) == 0){                     // �f�o�C�X�̎Q���󂯓�����J�n
        printf("no Devices\n");                 // �q�@��������Ȃ�����
        exit(-1);                               // �ُ�I��
    }
    printf("Found a Device\n");                 // XBee�q�@�f�o�C�X�̔����\��
    xbee_from( dev );                           // �������q�@�̃A�h���X��ϐ�dev��
    xbee_ratnj(dev,0);                          // �q�@�ɑ΂��đ��@�̎󂯓��ꐧ����ݒ�
    
    while(1){                                   // �J��Ԃ�����
        /* XBee�ʐM */
        value = xbee_adc(dev,1);                // �q�@�̃|�[�g1����A�i���O�l�����
        printf("Value =%d\n",value);            // �ϐ�value�̒l��\������
        delay( 1000 );                          // 1000ms(1�b��)�̑҂�
    }
}
