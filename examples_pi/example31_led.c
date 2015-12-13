/***************************************************************************************
XBee Wi-Fi��LED�������[�g���䂷��A���C�u�����֐�xbee_gpo�ŊȒP����

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"          // XBee���C�u�����̃C���|�[�g

// ���莝����XBee���W���[����IP�A�h���X�ɕύX����(��؂�̓J���})
byte dev[] = {192,168,0,135};

int main(void){
    char s[4];                          // ���͗p(3�����܂�)
    byte port=4;                        // �����[�g�@�̃|�[�g�ԍ�(�����l��4)
    byte value=1;                       // �����[�g�@�ւ̐ݒ�l(�����l��1)

    xbee_init( 0 );                     // XBee�̏�����
    printf("Example 31 LED ('q' to Exit)\n");
    while( xbee_ping(dev)==00 ){        // �J��Ԃ�����
        xbee_gpo(dev,port,value);       // �����[�g�@�|�[�g(port)�ɐ���l(value)��ݒ�
        printf("Port  =");              // �|�[�g�ԍ����͂̂��߂̕\��
        fgets(s, 4, stdin);             // �W�����͂���擾
        if( s[0] == 'q' ) break;        // [q]�����͂��ꂽ����while�𔲂���
        port = atoi( s );               // ���͕����𐔎��ɕϊ�����port�ɑ��
        printf("Value =");              // �l�̓��͂̂��߂̕\��
        fgets(s, 4, stdin);             // �W�����͂���擾
        value = atoi( s );              // ���͕����𐔎��ɕϊ�����value�ɑ��
    }
    printf("done\n");
    return(0);
}
