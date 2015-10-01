/***************************************************************************************
����u���b�h�{�[�h�Z���T�̑���l���������M����

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

void set_ports(byte *dev){
   xbee_gpio_config( dev, 1 , AIN );            // XBee�q�@�̃|�[�g1���A�i���O���͂�
   xbee_end_device( dev, 3, 3, 0);              // �N���Ԋu3�b,��������3�b,S�[�q����
}

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte dev[8];                                // XBee�q�@�f�o�C�X�̃A�h���X
    float value;                                // ��M�f�[�^�̑���p
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                          // �q�@XBee�f�o�C�X����ɎQ���󂯓���
    printf("Waiting for XBee Commissoning\n");  // �҂��󂯒��̕\��

    while(1){
        /* �f�[�^��M(�҂��󂯂Ď�M����) */
        xbee_rx_call( &xbee_result );               // �f�[�^����M
        switch( xbee_result.MODE ){                 // ��M�����f�[�^�̓��e�ɉ�����
            case MODE_GPIN:                         // �q�@XBee�̎������M�̎�M
                // �Ɠx���茋�ʂ�value�ɑ������printf�ŕ\������
                value = (float)xbee_result.ADCIN[1] * 3.55;
                printf("%.1f Lux\n" , value );
                break;
            case MODE_IDNT:                         // �V�����f�o�C�X�𔭌�
                printf("Found a New Device\n");
                xbee_atnj(0);                       // �q�@XBee�f�o�C�X�̎Q����s����
                bytecpy(dev, xbee_result.FROM, 8);  // ���������A�h���X��dev�ɃR�s�[����
                set_ports( dev );                   // �q�@��GPIO�|�[�g�̐ݒ�
                break;
        }
    }
}
