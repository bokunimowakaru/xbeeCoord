/***************************************************************************************
�q�@XBee��UART����V���A�����𑗐M����

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    char s[32];                                 // �������͗p
    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte dev[]={0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};
                                                // ����XBee�A�h���X(�u���[�h�L���X�g)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    printf("Waiting for XBee Commissoning\n");  // �҂��󂯒��̕\��
    if(xbee_atnj(30) != 0){                     // �f�o�C�X�̎Q���󂯓�����J�n
        printf("Found a Device\n");             // XBee�q�@�f�o�C�X�̔����\��
        xbee_from( dev );                       // �������q�@�̃A�h���X��ϐ�dev��
        xbee_ratnj(dev,0);                      // �q�@�ɑ΂��đ��@�̎󂯓��ꐧ����ݒ�
    }else{                                      // �q�@��������Ȃ������ꍇ
        printf("no Devices\n");                 // ������Ȃ��������Ƃ�\��
        exit(-1);                               // �ُ�I��
    }
    
    while(1){
        /* �f�[�^���M */
        printf("TX-> ");                        // �������͗��̕\��
        gets( s );                              // ���͕�����ϐ�s�ɑ��
        xbee_uart( dev , s );                   // �ϐ�s�̕����𑗐M
    }
}
