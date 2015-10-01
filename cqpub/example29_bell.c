/***************************************************************************************
����u���b�h�{�[�h���g���������[�g�u�U�[�̐���

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#include "../libs/kbhit.c"

void bell(byte *dev, byte c){
    byte i;
    for(i=0;i<c;i++){
        xbee_gpo( dev , 4 , 1 );                // �q�@ dev�̃|�[�g4��1�ɐݒ�(���M)
        xbee_gpo( dev , 4 , 0 );                // �q�@ dev�̃|�[�g4��0�ɐݒ�(���M)
    }
    xbee_gpo( dev , 4 , 0 );                    // �q�@ dev�̃|�[�g4��0�ɐݒ�(���M)
}

int main(int argc,char **argv){
    
    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte dev[8];                                // XBee�q�@�f�o�C�X�̃A�h���X
    char c;                                     // ���͗p

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    printf("Waiting for XBee Commissoning\n");  // �҂��󂯒��̕\��
    if(xbee_atnj(30) != 0){                     // �f�o�C�X�̎Q���󂯓�����J�n
        printf("Found a Device\n");             // XBee�q�@�f�o�C�X�̔����\��
        xbee_from( dev );                       // �q�@�̃A�h���X��ϐ�dev��
        bell( dev, 3);                          // �u�U�[��3��炷
    }else{                                      // �q�@��������Ȃ������ꍇ
        printf("no Devices\n");                 // ������Ȃ��������Ƃ�\��
        exit(-1);                               // �ُ�I��
    }
    printf("Hit any key >");

    while(1){                                   // �J��Ԃ�����
        if ( kbhit() ) {                        // �L�[�{�[�h���̗͂L������
            c = getchar();                      // ���͕�����ϐ�s�ɑ��
            c -= '0';                           // ���͕����𐔎��ɕϊ�
            if( c < 0 || c > 10 ) c = 10;       // �����ȊO�̎���c��10����
            bell(dev,c);                        // �u�U�[��c��炷
        }
    }
}
