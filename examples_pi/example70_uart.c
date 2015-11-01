/***************************************************************************************
XBee Wi-Fi���g����UART�V���A������M

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"                      // XBee���C�u�����̃C���|�[�g
#include "../libs/kbhit.c"
#include <ctype.h>

// ���莝����XBee���W���[����IP�A�h���X�ɕύX���Ă�������(��؂�̓J���})
byte dev[]      = {192,168,0,135};                  // �q�@XBee
byte dev_my[]   = {192,168,0,255};                  // �e�@�p�\�R��

int main(void){
    char c;                                         // �������͗p
    char s[32];                                     // ���M�f�[�^�p
    byte len=0;                                     // ������
    XBEE_RESULT xbee_result;                        // ��M�f�[�^(�ڍ�)

    xbee_init( 0 );                                 // XBee�pCOM�|�[�g�̏�����
    printf("Example 70 UART (ESC key to Exit)\n");
    s[0]='\0';                                      // ������̏�����
    printf("TX-> ");                                // �҂��󂯒��̕\��
    if( xbee_ping(dev)==00 ){
        xbee_myaddress(dev_my);                     // PC�̃A�h���X��ݒ肷��
        xbee_ratd_myaddress(dev);                   // �q�@��PC�̃A�h���X��ݒ肷��
        xbee_rat(dev,"ATAP00");                     // XBee API������(UART���[�h�ɐݒ�)
        while(1){

            /* �f�[�^���M */
            if( kbhit() ){
                c=getchar();                        // �L�[�{�[�h����̕�������
                if( c == 0x1B ){                    // ESC�L�[��������
                    printf("E");                    // ESC E(���s)�����s
                    break;                          // while�𔲂���
                }
                if( isprint( (int)c ) ){            // �\���\�ȕ��������͂��ꂽ��
                    s[len]=c;                       // ������ϐ�s�ɓ��͕�����������
                    len++;                          // ������������₷
                    s[len]='\0';                    // ������̏I����\��\0��������
                }
                if( c == '\n' || len >= 31 ){       // ���s�������͕�������31�����̎�
                    xbee_uart( dev , s );           // �ϐ�s�̕����𑗐M
                    xbee_uart( dev,"\r");           // �q�@�ɉ��s�𑗐M
                    len=0;                          // ��������0�Ƀ��Z�b�g����
                    s[0]='\0';                      // ������̏�����
                    printf("TX-> ");                // �҂��󂯒��̕\��
                }
            }

            /* �f�[�^��M(�҂��󂯂Ď�M����) */
            xbee_rx_call( &xbee_result );           // XBee Wi-Fi����̃f�[�^����M
            if( xbee_result.MODE == MODE_UART){     // UART�V���A���f�[�^����M������
                printf("\n");                       // �҂��󂯒������uTX�v�̍s�����s
                printf("RX<- ");                    // ��M�����ʂ��邽�߂̕\��
                printf("%s\n", xbee_result.DATA );  // ��M����(�e�L�X�g)��\��
                printf("TX-> %s",s );               // �������͗��Ɠ��͒��̕����̕\��
            }
        }
    }
    printf("done\n");
    return(0);
}
