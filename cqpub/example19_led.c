/***************************************************************************************
LED�������[�g���䂷��C�ʐM�̈Í���

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){
    
    byte com=0;                         // �V���A��COM�|�[�g�ԍ�
    byte dev[8];                        // XBee�q�@�f�o�C�X�̃A�h���X
    
    /* ������ */
    if(argc==2) com=(byte)atoi(argv[1]);// ����������Εϐ�com�ɑ������
    xbee_init( com );                   // XBee�pCOM�|�[�g�̏�����
    /* �Í����L�� */
    if( xbee_atee_on("password") <= 1){ // �Í���ON�ݒ�Bpassword��16�����܂�
        printf("Encryption On\n");      // "password" -> 70617373776F7264
    }else{
        printf("Encryption Error\n");   // �Í����G���[�\��
        exit(-1); 
    }
    /* �y�A�����O */
    printf("XBee in Commissioning\n");  // �҂��󂯒��̕\��
    if(xbee_atnj(30)){                  // �f�o�C�X�̎Q���󂯓�����J�n�i�ő�30�b�ԁj
        printf("Found a Device\n");     // XBee�q�@�f�o�C�X�̔����\��
        xbee_from( dev );               // �������f�o�C�X�̃A�h���X��ϐ�dev�Ɏ捞��
        xbee_ratnj(dev,0);              // �q�@�ɑ΂��đ��@�̎󂯓��ꐧ����ݒ�
    }else{
        printf("No Devices\n");         // �G���[���̕\��
        exit(-1);                       // �ُ�I��
    }
    /* LED�̓_�Łi�Í����j */
    while(1){                           // 5��̌J��Ԃ�����
        xbee_gpo(dev, 4, 1);            // �����[�gXBee�̃|�[�g4���o��'H'�ɐݒ肷��
        delay( 1000 );                  // 1000ms(1�b��)�̑҂�
        xbee_gpo(dev, 4, 0);            // �����[�gXBee�̃|�[�g4���o��'L'�ɐݒ肷��
        delay( 1000 );                  // 1000ms(1�b��)�̑҂�
    }
}
