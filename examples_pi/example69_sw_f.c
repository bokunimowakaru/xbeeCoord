/***************************************************************************************
XBee Wi-Fi�̃X�C�b�`��Ԃ������[�g�Ŏ擾���X�C�b�`�ω��ʒm�ł��擾����

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"                      // XBee���C�u�����̃C���|�[�g
#include "../libs/kbhit.c"
#define FORCE_INTERVAL  200                         // �f�[�^�v���Ԋu(���悻30ms�̔{��)

// ���莝����XBee���W���[����IP�A�h���X�ɕύX���Ă�������(��؂�̓J���})
byte dev_gpio[] = {192,168,0,135};                  // �q�@XBee
byte dev_my[]   = {192,168,0,255};                  // �e�@�p�\�R��

int main(void){
    byte trig=0;
    byte value;                                     // ��M�l
    XBEE_RESULT xbee_result;                        // ��M�f�[�^(�ڍ�)

    xbee_init( 0 );                                 // XBee�̏�����
    printf("Example 69 SW_F (Any key to Exit)\n");
    if( xbee_ping(dev_gpio)==00 ){
        xbee_myaddress(dev_my);                     // �����̃A�h���X��ݒ肷��
        xbee_gpio_init(dev_gpio);                   // �f�o�C�Xdev_gpio��IO�ݒ���s��
        xbee_end_device(dev_gpio,28,0,0);           // �f�o�C�Xdev_gpio���ȓd�͂ɐݒ�
        while(1){
            /* �擾�v���̑��M */
            if( trig == 0){
                xbee_force( dev_gpio );             // �q�@�փf�[�^�v���𑗐M
                trig = FORCE_INTERVAL;
            }
            trig--;

            /* �f�[�^��M(�҂��󂯂Ď�M����) */
            xbee_rx_call( &xbee_result );           // �f�[�^����M
            if( xbee_result.MODE == MODE_RESP ||    // xbee_force�ɑ΂��鉞��
                xbee_result.MODE == MODE_GPIN){     // �������͎q�@XBee��DIO���͂̎�
                value = xbee_result.GPI.PORT.D1;    // D1�|�[�g�̒l��ϐ�value�ɑ��
                printf("Value =%d\n",value);        // �ϐ�value�̒l��\��
                xbee_gpo(dev_gpio,4,value);         // �q�@XBee��DIO�|�[�g4�֏o��
            }
            if( kbhit() ) break;                    // PC�̃L�[��������while�𔲂���
        }
        xbee_end_device(dev_gpio,0,0,0);            // �f�o�C�Xdev_gpio�̏ȓd�͂�����
    }
    printf("\ndone\n");
    return(0);
}
