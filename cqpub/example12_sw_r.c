/***************************************************************************************
�q�@XBee�̃X�C�b�`�ω��ʒm����M����

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){
    
    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte value;                                 // ��M�l
    byte dev[8];                                // XBee�q�@�f�o�C�X�̃A�h���X
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    printf("Waiting for XBee Commissoning\n");  // �҂��󂯒��̕\��
    if(xbee_atnj(30) != 0){                     // �f�o�C�X�̎Q���󂯓�����J�n
        printf("Found a Device\n");             // XBee�q�@�f�o�C�X�̔����\��
        xbee_from( dev );                       // �������q�@�̃A�h���X��ϐ�dev��
        xbee_ratnj(dev,0);                      // �q�@�ɑ΂��đ��@�̎󂯓��ꐧ����ݒ�
        xbee_gpio_init( dev );                  // �q�@��DIO��IO�ݒ���s��(���M)
    }else printf("no Devices\n");               // �q�@��������Ȃ�����
    
    while(1){
        /* �f�[�^��M(�҂��󂯂Ď�M����) */
        xbee_rx_call( &xbee_result );           // �f�[�^����M
        if( xbee_result.MODE == MODE_GPIN){     // �q�@XBee��DIO����
            value = xbee_result.GPI.PORT.D1;    // D1�|�[�g�̒l��ϐ�value�ɑ��
            printf("Value =%d ",value);         // �ϐ�value�̒l��\��
            value = xbee_result.GPI.BYTE[0];    // D7�`D0�|�[�g�̒l��ϐ�value�ɑ��
            lcd_disp_bin( value );              // value�ɓ������l���o�C�i���ŕ\��
            printf("\n");                       // ���s
        }
    }
}
