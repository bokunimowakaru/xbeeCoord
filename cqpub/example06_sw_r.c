/***************************************************************************************
�X�C�b�`��Ԃ������[�g�擾����A�ω��ʒm

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

// ���莝����XBee���W���[���q�@��IEEE�A�h���X�ɕύX���遫
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){
    
    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte value;                                 // ��M�l
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                          // �e�@XBee����ɃW���C������Ԃɂ���
    xbee_gpio_init( dev );                      // �q�@��DIO��IO�ݒ���s��(���M)
    
    while(1){
        /* �f�[�^��M(�҂��󂯂Ď�M����) */
        xbee_rx_call( &xbee_result );           // �f�[�^����M
        if( xbee_result.MODE == MODE_GPIN){     // �q�@XBee�����DIO���͂̎��i�������j
            value = xbee_result.GPI.PORT.D1;    // D1�|�[�g�̒l��ϐ�value�ɑ��
            printf("Value =%d\n",value);        // �ϐ�value�̒l��\��
        }
    }
}
