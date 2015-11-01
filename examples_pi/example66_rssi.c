/***************************************************************************************
XBee Wi-Fi��RSSI LED��DIO4�ɐڑ�����LED��_��

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"          // XBee���C�u�����̃C���|�[�g
#include "../libs/kbhit.c"

// ���莝����XBee���W���[����IP�A�h���X�ɕύX����(��؂�̓J���})
byte dev[] = {192,168,0,135};

int main(void){
    xbee_init( 0 );                     // XBee�̏�����
    printf("Example 66 RSSI (Any key to Exit)\n");
    while( xbee_ping(dev)==00 ){        // �J��Ԃ�����
        xbee_rat(dev,"ATP005");         // �����[�gAT�R�}���hATP0(DIO10�ݒ�)=05(�o��'H')
        xbee_rat(dev,"ATD405");         // �����[�gAT�R�}���hATD4(DIO 4�ݒ�)=05(�o��'H')
        delay( 1000 );                  // ��1000ms(1�b��)�̑҂�
        xbee_rat(dev,"ATP004");         // �����[�gAT�R�}���hATP0(DIO10�ݒ�)=04(�o��'L')
        xbee_rat(dev,"ATD404");         // �����[�gAT�R�}���hATD4(DIO 4�ݒ�)=04(�o��'L')
        delay( 1000 );                  // ��1000ms(1�b��)�̑҂�
        if( kbhit() ) break;            // �L�[��������Ă�������while���[�v�𔲂���
    }
    printf("\ndone\n");
    return(0);
}
