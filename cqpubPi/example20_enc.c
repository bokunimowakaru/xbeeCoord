/***************************************************************************************
�e�@�Ǝq�@�Ƃ̈Í����f�[�^�̑���M

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/
#include <ctype.h>
#include "../libs/xbee.c"
#include "../libs/kbhit.c"

int main(int argc,char **argv){

    char c;                                         // �������͗p
    char s[32];                                     // �������͗p
    byte len=0;                                     // ������
    byte com=0;                                     // �V���A��COM�|�[�g�ԍ�
    byte dev[]={0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};   // �����XBee�A�h���X
    XBEE_RESULT xbee_result;                        // ��M�f�[�^(�ڍ�)
    
    /* ������ */
    if(argc==2) com=(byte)atoi(argv[1]);            // ����������Εϐ�com�ɑ������
    xbee_init( com );                               // XBee�pCOM�|�[�g�̏�����
    /* �Í����L�� */
    if( xbee_atee_on("password") <= 1){             // �Í���ON�Bpassword��16�����܂�
        printf("Encryption On\n");                  // "password" -> 70617373776F7264
    }else{
        printf("Encryption Error\n");               // �Í����G���[�\��
        exit(-1); 
    }
    /* �y�A�����O */
    if( xbee_atvr() == ZB_TYPE_COORD ){             // �{�@��Coordinator(�e�@)�̎�
        printf("Xbee in Commissioning\n");          // �҂��󂯒��̕\��
        xbee_atnj( 0xFF );                          // �q�@XBee�f�o�C�X����ɎQ������
    }else{                                          // �{�@��Router(�q�@)�̎�
        do{                                         // �J��Ԃ��̊J�n
            wait_millisec( 1000 );                  // 1�b�҂�
            printf("Commissioning\n");              // XBee�e�@�̌���
            xbee_atcb( 1 );                         // �R�~�b�V�����{�^���̉���
        }while( xbee_atai() > 0 );                  // �Q����������܂ŌJ��Ԃ�
        printf("Joined\nTX-> ");                    // �Q������
    }
    s[0]='\0';                                      // ������̏�����

    /* �Í����ʐM���� */
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
        /* �f�[�^��M */
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
