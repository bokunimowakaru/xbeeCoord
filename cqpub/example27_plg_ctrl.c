/***************************************************************************************
�Â��Ȃ�����Smart Plug�̉Ɠd�̓d����OFF�ɂ���

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // �f�[�^�v���Ԋu(��10�`20ms�̔{��)

void set_ports(byte *dev){
    xbee_gpio_config( dev, 1 , AIN );           // XBee�q�@�̃|�[�g1���A�i���O����AIN��
    xbee_gpio_config( dev, 2 , AIN );           // XBee�q�@�̃|�[�g2���A�i���O����AIN��
    xbee_gpio_config( dev, 3 , AIN );           // XBee�q�@�̃|�[�g3���A�i���O����AIN��
    xbee_gpio_config( dev, 4 , DOUT_H );        // XBee�q�@�̃|�[�g3���f�W�^���o�͂�
}

int main(int argc,char **argv){

    byte com=0;                                     // �V���A��COM�|�[�g�ԍ�
    byte dev[8];                                    // XBee�q�@�f�o�C�X�̃A�h���X
    byte trig=0xFF;                                 // �f�[�^�v������^�C�~���O�����p
    byte id=0;                                      // �p�P�b�g���M�ԍ�
    float value;                                    // ��M�f�[�^�̑���p
    XBEE_RESULT xbee_result;                        // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);            // ����������Εϐ�com�ɑ������
    xbee_init( com );                               // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                              // �e�@�Ɏq�@�̃W���C������ݒ�
    printf("Waiting for XBee Commissoning\n");      // �҂��󂯒��̕\��
    
    while(1){
        if( trig == 0){
            id = xbee_force( dev );                 // �q�@�փf�[�^�v���𑗐M
            trig = FORCE_INTERVAL;
        }
        if( trig != 0xFF ) trig--;                  // �ϐ�trig��0xFF�ȊO�̎��ɒl��1���Z

        xbee_rx_call( &xbee_result );               // �f�[�^����M
        switch( xbee_result.MODE ){                 // ��M�����f�[�^�̓��e�ɉ�����
            case MODE_RESP:                         // xbee_force�ɑ΂��鉞���̎�
                if( id == xbee_result.ID ){         // ���M�p�P�b�gID����v
                    value = xbee_sensor_result( &xbee_result, LIGHT);
                    printf("%.1f Lux, " , value );
                    if( value < 10 ){               // �Ɠx��10Lux�ȉ��̎�
                        xbee_gpo(dev , 4 , 0 );     // �q�@XBee�̃|�[�g4��L�ɐݒ�
                    }
                    value = xbee_sensor_result( &xbee_result,WATT);
                    printf("%.1f Watts, " , value );
                    if( xbee_gpi( dev , 4 ) == 0 ){ // �|�[�g4�̏�Ԃ�ǂ݂Ƃ�A
                        printf("OFF\n");            // 0�̎���OFF�ƕ\������
                    }else{
                        printf("ON\n");             // 1�̎���ON�ƕ\������
                    }
                }
                break;
            case MODE_IDNT:                         // �V�����f�o�C�X�𔭌�
                printf("Found a New Device\n");
                bytecpy(dev, xbee_result.FROM, 8);  // ���������A�h���X��dev�ɃR�s�[����
                xbee_atnj(0);                       // �e�@XBee�Ɏq�@�̎󂯓��ꐧ��
                xbee_ratnj(dev,0);                  // �q�@�ɑ΂��đ��@�̎󂯓���𐧌�
                set_ports( dev );                   // �q�@��GPIO�|�[�g�̐ݒ�
                trig = 0;                           // �q�@�փf�[�^�v�����J�n
                break;
        }
    }
}
