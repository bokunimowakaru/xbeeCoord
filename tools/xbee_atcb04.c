/***************************************************************************************
�l�b�g���[�N�ݒ�����Z�b�g����

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte ret=0;

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    ret = xbee_atcb(4);                         // �l�b�g���[�N�ݒ�����Z�b�g����
    if( ret==00 ){                              // �Í������I�t�ɂ���
        printf("OK\n");                         // OK�\��
    }else printf("Error\n");                    // �G���[�\��
    return( ret );
}
