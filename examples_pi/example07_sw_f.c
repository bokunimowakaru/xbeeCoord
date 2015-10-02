/***************************************************************************************
�X�C�b�`��Ԃ������[�g�擾����B�擾�w��

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // �f�[�^�v���Ԋu(��10�`20ms�̔{��)

// ���莝����XBee���W���[���q�@��IEEE�A�h���X�ɕύX���遫
byte dev[] = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte value;                                 // ��M�l
    byte trig=0;                                // �q�@�փf�[�^�v������^�C�~���O�����p
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                          // �e�@XBee����ɃW���C������Ԃɂ���
    xbee_gpio_config(dev,1,DIN);                // �q�@XBee�̃|�[�g1���f�W�^�����͂�
   
    while(1){
        /* �f�[�^���M */
        if( trig == 0){
            xbee_force( dev );                  // �q�@�փf�[�^�v���𑗐M
            trig = FORCE_INTERVAL;
        }
        trig--;

        /* �f�[�^��M(�҂��󂯂Ď�M����) */
        xbee_rx_call( &xbee_result );           // �f�[�^����M
        if( xbee_result.MODE == MODE_RESP){     // xbee_force�ɑ΂��鉞���̎�
            value = xbee_result.GPI.PORT.D1;    // D1�|�[�g�̒l��ϐ�value�ɑ��
            printf("Value =%d\n",value);        // �ϐ�value�̒l��\��
        }
    }
}
