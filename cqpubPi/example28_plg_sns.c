/***************************************************************************************
���ւ����邭�Ȃ����烊�r���O�̉Ɠd��ON�ɂ���

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // �f�[�^�v���Ԋu(��10�`20ms�̔{��)

int main(int argc,char **argv){

    byte com=0;                                     // �V���A��COM�|�[�g�ԍ�
    byte dev_sens[8];                               // �q�@XBee(����Z���T)�̃A�h���X
    byte dev_plug[8];                               // �q�@XBee(Smart Plug)�̃A�h���X
    byte dev_sens_en=0;                             // �q�@XBee(����Z���T)�̏��
    byte dev_plug_en=0;                             // �q�@XBee(Smart Plug)�̏��
    float value;                                    // ��M�f�[�^�̑���p
    XBEE_RESULT xbee_result;                        // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);            // ����������Εϐ�com�ɑ������
    xbee_init( com );                               // XBee�pCOM�|�[�g�̏�����
    xbee_atnj(0xFF);                                // ��ɃW���C�����ɐݒ�
    printf("Waiting for XBee Commissoning\n");      // �҂��󂯒��̕\��
    
    while(1){        
        xbee_rx_call( &xbee_result );                       // �f�[�^����M
        switch( xbee_result.MODE ){
            case MODE_GPIN:                                 // �Ɠx�Z���T�������M����
                if(bytecmp(xbee_result.FROM,dev_sens,8)==0){ // IEEE�A�h���X�̊m�F
                    value=(float)xbee_result.ADCIN[1]*3.55; 
                    printf("Entrance %.1f Lux\n" , value );
                    if( dev_plug_en ){
                        if( value > 10 ){                   // �Ɠx��10Lux�ȉ��̎�
                            xbee_gpo(dev_plug,4,1);         // Smart Plug�̃|�[�g4��H��
                        }else{
                            xbee_gpo(dev_plug,4,0);         // Smart Plug�̃|�[�g4��L��
                        }
                        xbee_force( dev_plug );             // Smart Plug�̏Ɠx�擾�w��
                    }
                }
                break;
            case MODE_RESP:                                 // xbee_force�ɑ΂��鉞����
                if(bytecmp(xbee_result.FROM,dev_plug,8)==0){ // IEEE�A�h���X�̊m�F
                    value=xbee_sensor_result(&xbee_result,WATT);
                    printf("LivingRoom %.1f Watts, ",value);
                    if( xbee_gpi(dev_plug ,4)==0 ){         // �|�[�g4�̏�Ԃ�ǂ݂Ƃ�A
                        printf("OFF\n");                    // 0�̎���OFF�ƕ\������
                    }else{
                        printf("ON\n");                     // 1�̎���ON�ƕ\������
                    }
                }
                break;
            case MODE_IDNT:                                 // �V�����f�o�C�X�𔭌�
                if( xbee_ping( xbee_result.FROM ) != DEV_TYPE_PLUG){
                    printf("Found a Sensor\n");             // �Ɠx�Z���T�̎��̏���
                    bytecpy(dev_sens,xbee_result.FROM,8);   // �A�h���X��dev_sens�ɑ��
                    xbee_gpio_config(dev_sens,1,AIN);       // �|�[�g1���A�i���O���͂�
                    xbee_end_device(dev_sens,3,3,0);        // �X���[�v�ݒ�
                    dev_sens_en=1;                          // �Ɠx�Z���T�̏�Ԃ�1����
                }else{                                      // Smart Plug�̎��̏���
                    printf("Found a Smart Plug\n");         // �����\��
                    bytecpy(dev_plug,xbee_result.FROM,8);   // �A�h���X��dev_sens�ɑ��
                    xbee_ratnj(dev_plug,0);                 // �W���C���s���ɐݒ�
                    xbee_gpio_config(dev_plug, 1 , AIN );   // �|�[�g1���A�i���O���͂�
                    xbee_gpio_config(dev_plug, 2 , AIN );   // �|�[�g2���A�i���O���͂�
                    xbee_gpio_config(dev_plug, 3 , AIN );   // �|�[�g3���A�i���O���͂�
                    xbee_gpio_config(dev_plug, 4 , DOUT_H); // �|�[�g4���f�W�^���o�͂�
                    dev_plug_en=1;                          // Smart Plug�̏�Ԃ�1����
                }
                if( dev_sens_en * dev_plug_en > 0 ){
                    xbee_atnj(0);                           // �e�@XBee�Ɏ󂯓��ꐧ��
                }
                break;
        }
    }
}
