/***************************************************************************************
�Í������I�t�ɂ���

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte ret=0;

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    ret = xbee_atee_off();
    if( ret <= 1 ){                             // �Í������I�t�ɂ���
        printf("Encryption Off\n");             // �\��
    }else printf("Error(%02X)\n",ret);          // �G���[�\��
    return( ret );
}
