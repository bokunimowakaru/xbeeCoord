/***************************************************************************************
����u���b�h�{�[�h�Z���T�ŏƓx������s��

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                         // �f�[�^�v���Ԋu(��10�`20ms�̔{��)

void set_ports(byte *dev){
    xbee_gpio_config( dev, 1 , AIN );               // XBee�q�@�̃|�[�g1���A�i���O���͂�
    xbee_end_device( dev, 1, 0, 0);                 // �N���Ԋu1�b,�������薳��
}

int main(int argc,char **argv){

    byte com=0;                                     // �V���A��COM�|�[�g�ԍ�
    byte dev[8];                                    // XBee�q�@�f�o�C�X�̃A�h���X
    byte id=0;                                      // �p�P�b�g���M�ԍ�
    byte trig=0xFF;                                 // �f�[�^�v������^�C�~���O�����p
    float value;                                    // ��M�f�[�^�̑���p
    XBEE_RESULT xbee_result;                        // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);            // ����������Εϐ�com�ɑ������
    xbee_init( com );                               // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                              // �e�@�Ɏq�@�̃W���C������ݒ�
    printf("Waiting for XBee Commissoning\n");      // �҂��󂯒��̕\��
    
    while(1){
        /* �f�[�^���M */
        if( trig == 0){
            id = xbee_force( dev );                 // �q�@�փf�[�^�v���𑗐M
            trig = FORCE_INTERVAL;
        }
        if( trig != 0xFF ) trig--;                  // �ϐ�trig��0xFF�ȊO�̎��ɒl��1���Z

        /* �f�[�^��M(�҂��󂯂Ď�M����) */
        xbee_rx_call( &xbee_result );               // �f�[�^����M
        switch( xbee_result.MODE ){                 // ��M�����f�[�^�̓��e�ɉ�����
            case MODE_RESP:                         // xbee_force�ɑ΂��鉞���̎�
                if( id == xbee_result.ID ){         // ���M�p�P�b�gID����v
                    // �Ɠx���茋�ʂ�value�ɑ������printf�ŕ\������
                    value = (float)xbee_result.ADCIN[1] * 3.55;
                    printf("%.1f Lux\n" , value );
                }
                break;
            case MODE_IDNT:                         // �V�����f�o�C�X�𔭌�
                printf("Found a New Device\n");
                bytecpy(dev, xbee_result.FROM, 8);  // ���������A�h���X��dev�ɃR�s�[����
                xbee_atnj(0);                       // �e�@XBee�Ɏq�@�̎󂯓��ꐧ��
                set_ports( dev );                   // �q�@��GPIO�|�[�g�̐ݒ�
                trig = 0;                           // �q�@�փf�[�^�v�����J�n
                break;
        }
    }
}
