/***************************************************************************************
�e�@�Ǝq�@�Ƃ�UART���������V���A������M

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include <ctype.h>
#include "../libs/xbee.c"
#include "../libs/kbhit.c"

int main(int argc,char **argv){

    char c;                                         // �������͗p
    char s[32];                                     // ���M�f�[�^�p
    byte len=0;                                     // ������
    byte com=0xB0;                                  // �g��IO�R�l�N�^�̏ꍇ��0xA0
    byte dev[]={0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};
                                                    // ����XBee�A�h���X
    XBEE_RESULT xbee_result;                        // ��M�f�[�^(�ڍ�)

    if(argc==2) com += atoi(argv[1]);               // ����������Εϐ�com�ɒl�����Z����
    xbee_init( com );                               // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                              // �q�@XBee�f�o�C�X����ɎQ������
    printf("Waiting for XBee Commissoning\n");      // �R�~�b�V���j���O�҂��󂯒��̕\��
    s[0]='\0';                                      // ������̏�����

    while(1){
        
        /* �f�[�^���M */
        if( kbhit() ){
            c=getchar();                            // �L�[�{�[�h����̕�������
            if( isprint( (int)c ) ){                // �\���\�ȕ��������͂��ꂽ��
                s[len]=c;                           // ������ϐ�s�ɓ��͕�����������
                len++;                              // ������������₷
                s[len]='\0';                        // ������̏I����\��\0��������
            }
            if( c == '\n' || len >= 31 ){           // ���s�������͕�������31�����̎�
                xbee_uart( dev , s );               // �ϐ�s�̕����𑗐M
                xbee_uart( dev,"\r");               // �q�@�ɉ��s�𑗐M
                len=0;                              // ��������0�Ƀ��Z�b�g����
                s[0]='\0';                          // ������̏�����
                printf("TX-> ");                    // �҂��󂯒��̕\��
            }
        }
        
        /* �f�[�^��M(�҂��󂯂Ď�M����) */
        xbee_rx_call( &xbee_result );               // XBee�q�@����̃f�[�^����M
        switch( xbee_result.MODE ){                 // ��M�����f�[�^�̓��e�ɉ�����
            case MODE_UART:                         // �q�@XBee����̃e�L�X�g��M
                printf("\n");                       // �҂��󂯒������uTX�v�̍s�����s
                printf("RX<- ");                    // ��M�����ʂ��邽�߂̕\��
                printf("%s\n", xbee_result.DATA );  // ��M����(�e�L�X�g)��\��
                printf("TX-> %s",s );               // �������͗��Ɠ��͒��̕����̕\��
                break;
            case MODE_IDNT:                         // �V�����f�o�C�X�𔭌�
                printf("\n");                       // �҂��󂯒������uTX�v�̍s�����s
                printf("Found a New Device\n");     // XBee�q�@�f�o�C�X�̔����\��
                bytecpy(dev, xbee_result.FROM, 8);  // ���������A�h���X��dev�ɃR�s�[
                xbee_atnj(0);                       // �q�@XBee�̎󂯓��ꐧ����ݒ�
                xbee_ratnj(dev,0);                  // �q�@�ɑ΂��đ��@�̎󂯓��ꐧ��
                xbee_ratd_myaddress( dev );         // �q�@�ɐe�@�̃A�h���X�ݒ���s��
                printf("TX-> %s",s );               // �������͗��Ɠ��͒��̕����̕\��
                break;
        }
    }
}
