/***************************************************************************************
�A�i���O�d���������[�g�擾����C����q�@�̓����擾

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // �f�[�^�v���Ԋu(��10�`20ms�̔{��)

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    unsigned int  value;                        // �����[�g�q�@����̓��͒l
    byte dev[8];                                // XBee�q�@�f�o�C�X�̃A�h���X
    byte trig=0xFF;                             // �q�@�փf�[�^�v������^�C�~���O�����p
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                          // �e�@�Ɏq�@�̃W���C������ݒ�
    printf("Waiting for XBee Commissoning\n");  // �҂��󂯒��̕\��
    
    while(1){
        /* �f�[�^���M */
        if( trig == 0 ){
            xbee_force( dev );                  // �q�@�փf�[�^�v���𑗐M
            trig = FORCE_INTERVAL;
        }
        if( trig != 0xFF ) trig--;              // �ϐ�trig��0xFF�ȊO�̎��ɒl��1���Z

        /* �f�[�^��M(�҂��󂯂Ď�M����) */
        xbee_rx_call( &xbee_result );           // �f�[�^����M
        switch( xbee_result.MODE ){             // ��M�����f�[�^�̓��e�ɉ�����
            case MODE_RESP:                     // xbee_force�ɑ΂��鉞���̎�
                value = xbee_result.ADCIN[1];   // AD1�|�[�g�̃A�i���O�l��value�ɑ��
                printf("Value =%d\n",value);    // �ϐ�value�̒l��\��
                break;
            case MODE_IDNT:                     // �V�����f�o�C�X�𔭌�
                printf("Found a New Device\n");
                bytecpy(dev, xbee_result.FROM, 8);  // ���������A�h���X��dev�ɃR�s�[
                xbee_atnj(0);                   // �e�@XBee�Ɏq�@�̎󂯓��ꐧ����ݒ�
                xbee_ratnj(dev,0);              // �q�@�ɑ΂��đ��@�̎󂯓��ꐧ����ݒ�
                xbee_gpio_config(dev,1,AIN);    // �q�@XBee�̃|�[�g1���A�i���O���͂�
                trig = 0;                       // �q�@�փf�[�^�v�����J�n
                break;
        }
    }
}
