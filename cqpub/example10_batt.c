/***************************************************************************************
�q�@XBee�̃o�b�e���d���������[�g�Ŏ擾����

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // �f�[�^�v���Ԋu(��10�`20ms�̔{��)

// ���莝����XBee���W���[���q�@��IEEE�A�h���X�ɕύX���遫
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    unsigned int value;                         // �����[�g�q�@����̓��͒l
    byte trig=FORCE_INTERVAL;                   // �q�@�փf�[�^�v������^�C�~���O�����p
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                          // �e�@XBee����ɃW���C������Ԃɂ���
    
    while(1){
        /* �f�[�^���M */
        if( trig == 0){
            xbee_batt_force( dev );             // �q�@�֓d�r�d������v���𑗐M
            trig = FORCE_INTERVAL;
        }
        trig--;

        /* �f�[�^��M(�҂��󂯂Ď�M����) */
        xbee_rx_call( &xbee_result );           // XBee�q�@����̃f�[�^����M
        if( xbee_result.MODE == MODE_BATT){     // �o�b�e���d���̎�M
            value = xbee_result.ADCIN[0];       // �d���d���l��ϐ�value�ɑ��
            printf("Value =%d\n", value );      // ��M����(�d��)��\��
        }
    }
}
