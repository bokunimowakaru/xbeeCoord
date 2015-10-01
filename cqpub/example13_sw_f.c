/***************************************************************************************
�q�@XBee�̃X�C�b�`��Ԃ������[�g�Ŏ擾����

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // �f�[�^�v���Ԋu(��10�`20ms�̔{��)

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte value;                                 // ��M�l
    byte dev[8];                                // XBee�q�@�f�o�C�X�̃A�h���X
    byte trig=0xFF;                             // �q�@�փf�[�^�v������^�C�~���O�����p
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                          // �q�@XBee�f�o�C�X����ɎQ���󂯓���
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
                value = xbee_result.GPI.PORT.D1;// D1�|�[�g�̒l��ϐ�value�ɑ��
                printf("Value =%d ",value);     // �ϐ�value�̒l��\��
                value = xbee_result.GPI.BYTE[0];// D7�`D0�|�[�g�̒l��ϐ�value�ɑ��
                lcd_disp_bin( value );          // value�ɓ������l���o�C�i���ŕ\��
                printf("\n");                   // ���s
                break;
            case MODE_IDNT:                     // �V�����f�o�C�X�𔭌�
                printf("Found a New Device\n");
                bytecpy(dev, xbee_result.FROM, 8);  // ���������A�h���X��dev�ɃR�s�[
                xbee_atnj(0);                   // �e�@XBee�Ɏq�@�̎󂯓��ꐧ����ݒ�
                xbee_ratnj(dev,0);              // �q�@�ɑ΂��đ��@�̎󂯓��ꐧ����ݒ�
                xbee_gpio_config(dev,1,DIN);    // �q�@XBee�̃|�[�g1���f�W�^�����͂�
                xbee_gpio_config(dev,2,DIN);    // �q�@XBee�̃|�[�g2���f�W�^�����͂�
                xbee_gpio_config(dev,3,DIN);    // �q�@XBee�̃|�[�g3���f�W�^�����͂�
                trig = 0;                       // �q�@�փf�[�^�v�����J�n
                break;
        }
    }
}
