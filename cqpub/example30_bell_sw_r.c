/***************************************************************************************
XBee�X�C�b�`��XBee�u�U�[�Ō��֌ė�𐻍삷��

                                                  Copyright (c) 2013-2014 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define FORCE_INTERVAL  250                     // �f�[�^�v���Ԋu(��10�`20ms�̔{��)

void bell(byte *dev, byte c){
    byte i;
    for(i=0;i<c;i++){
        xbee_gpo( dev , 4 , 1 );                // �q�@ dev�̃|�[�g4��1�ɐݒ�(���M)
        xbee_gpo( dev , 4 , 0 );                // �q�@ dev�̃|�[�g4��0�ɐݒ�(���M)
    }
    xbee_gpo( dev , 4 , 0 );                    // �q�@ dev�̃|�[�g4��0�ɐݒ�(���M)
}

int main(int argc,char **argv){
    
    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte dev_bell[8];                           // XBee�q�@(�u�U�[)�f�o�C�X�̃A�h���X
    byte dev_sw[8];                             // XBee�q�@(�X�C�b�`)�f�o�C�X�̃A�h���X
    byte trig=0;                                // �q�@�փf�[�^�v������^�C�~���O�����p
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    printf("Waiting for XBee Bell\n");
    xbee_atnj(60);                              // �f�o�C�X�̎Q���󂯓���
    xbee_from( dev_bell );                      // �������q�@�̃A�h���X��ϐ�dev��
    bell(dev_bell,3);                           // �x����3��炷
    xbee_end_device(dev_bell, 1, 0, 0);         // �N���Ԋu1�b,��������OFF,SLEEP�[�q����
    printf("Waiting for XBee Switch\n");
    xbee_atnj(60);                              // �f�o�C�X�̎Q���󂯓���
    xbee_from( dev_sw );                        // �������q�@�̃A�h���X��ϐ�dev��
    bell(dev_bell,3);                           // �u�U�[��3��炷
    xbee_gpio_init( dev_sw );                   // �q�@��DIO��IO�ݒ���s��
    xbee_end_device(dev_sw, 3, 0, 1);           // �N���Ԋu3�b,�������MOFF,SLEEP�[�q�L��
    printf("done\n");

    while(1){
        if( trig == 0){
            xbee_force( dev_sw );                       // �q�@�փf�[�^�v���𑗐M
            trig = FORCE_INTERVAL;
        }
        trig--;
        xbee_rx_call( &xbee_result );                   // �f�[�^����M
        switch( xbee_result.MODE ){                     // ��M�����f�[�^�̓��e�ɉ�����
            case MODE_RESP:                             // �f�[�^�擾�w���ɑ΂��鉞��
            case MODE_GPIN:                             // �q�@XBee�̎������M�̎�M
                if(xbee_result.GPI.PORT.D1 == 0){       // DIO�|�[�g1��L���x���̎�
                    printf("D1=0 Ring\n");              // �\��
                    bell(dev_bell,3);                   // �u�U�[��3��炷
                }else printf("D1=1\n");                 // �\��
                bell(dev_bell,0);                       // �u�U�[��������
                break;
        }
    }
}
