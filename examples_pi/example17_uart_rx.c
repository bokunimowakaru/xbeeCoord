/***************************************************************************************
�q�@XBee��UART����̃V���A��������M����

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte dev[8];                                // XBee�q�@�f�o�C�X�̃A�h���X
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                          // �q�@XBee�f�o�C�X����ɎQ���󂯓���
    printf("Waiting for XBee Commissoning\n");  // �҂��󂯒��̕\��
    
    while(1){
        /* �f�[�^��M(�҂��󂯂Ď�M����) */
        xbee_rx_call( &xbee_result );           // XBee�q�@����̃f�[�^����M
        
        switch( xbee_result.MODE ){                 // ��M�����f�[�^�̓��e�ɉ�����
            case MODE_UART:                         // �q�@XBee����̃e�L�X�g��M
                printf("RX<- ");                    // ��M�����ʂ��邽�߂̕\��
                printf("%s\n", xbee_result.DATA );  // ��M����(�e�L�X�g)��\��
                break;
            case MODE_IDNT:                         // �V�����f�o�C�X�𔭌�
                printf("Found a New Device\n");
                xbee_atnj(0);                       // �q�@XBee�̎󂯓��ꐧ����ݒ�
                bytecpy(dev, xbee_result.FROM, 8);  // ���������A�h���X��dev�ɃR�s�[
                xbee_ratnj(dev,0);                  // �q�@�ɑ΂��đ��@�̎󂯓��ꐧ��
                xbee_ratd_myaddress( dev );         // �q�@�ɖ{�@�̃A�h���X�ݒ���s��
                break;
        }
    }
}

