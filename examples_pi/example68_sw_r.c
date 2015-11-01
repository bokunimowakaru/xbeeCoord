/***************************************************************************************
XBee Wi-Fi�̃X�C�b�`�ω��ʒm����M����

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"                      // XBee���C�u�����̃C���|�[�g
#include "../libs/kbhit.c"

// ���莝����XBee���W���[����IP�A�h���X�ɕύX���Ă�������(��؂�̓J���})
byte dev_gpio[] = {192,168,0,135};                  // �q�@XBee
byte dev_my[]   = {192,168,0,255};                  // �e�@�p�\�R��

int main(void){
    byte value;                                     // ��M�l
    XBEE_RESULT xbee_result;                        // ��M�f�[�^(�ڍ�)

    xbee_init( 0 );                                 // XBee�̏�����
    printf("Example 68 SW_R (Any key to Exit)\n");
    if( xbee_ping(dev_gpio)==00 ){
        xbee_myaddress(dev_my);                     // �����̃A�h���X��ݒ肷��
        xbee_gpio_init(dev_gpio);                   // �f�o�C�Xdev_gpio��IO�ݒ���s��
        while(1){
            xbee_rx_call( &xbee_result );           // �f�[�^����M
            if( xbee_result.MODE == MODE_GPIN){     // �q�@XBee��DIO����
                value = xbee_result.GPI.PORT.D1;    // D1�|�[�g�̒l��ϐ�value�ɑ��
                printf("Value =%d\n",value);        // �ϐ�value�̒l��\��
                xbee_gpo(dev_gpio,4,value);         // �q�@XBee��DIO�|�[�g4�֏o��
            }
            if( kbhit() ) break;                    // PC�̃L�[��������while�𔲂���
        }
    }
    printf("\ndone\n");
    return(0);
}
