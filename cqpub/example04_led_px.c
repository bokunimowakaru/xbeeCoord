/***************************************************************************************
LED�������[�g���䂷��B�l�X�ȃ|�[�g�ɏo��

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

// ���莝����XBee���W���[���q�@��IEEE�A�h���X�ɕύX���遫
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){
    byte com=0;                         // �V���A��COM�|�[�g�ԍ�
    char s[3];                          // ���͗p(2�����܂�)
    byte port;                          // �����[�g�q�@�̃|�[�g�ԍ�
    byte value;                         // �����[�g�q�@�ւ̐ݒ�l
    
    if(argc==2) com=(byte)atoi(argv[1]);// ����������Εϐ�com�ɑ������
    xbee_init( com );                   // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                  // �e�@XBee����ɃW���C������Ԃɂ���

    while(1){                           // �J��Ԃ�����
    
        /* �q�@�̃|�[�g�ԍ��Ɛ���l�̓��� */
        printf("Port  =");              // �|�[�g�ԍ����͂̂��߂̕\��
        gets( s );                      // �L�[�{�[�h����̓���
        port = atoi( s );               // ���͕����𐔎��ɕϊ�����port�ɑ��
        printf("Value =");              // �l�̓��͂̂��߂̕\��
        gets( s );                      // �L�[�{�[�h����̓���
        value = atoi( s );              // ���͕����𐔎��ɕϊ�����value�ɑ��

        /* XBee�ʐM */
        xbee_gpo(dev,port,value);       // �����[�g�q�@�|�[�g(port)�ɐ���l(value)��ݒ�
    }
}
