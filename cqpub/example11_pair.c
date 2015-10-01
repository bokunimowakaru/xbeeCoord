/***************************************************************************************
�e�@XBee�Ǝq�@XBee�Ƃ̃y�A�����O�Ə�Ԏ擾

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){
    
    byte i;                             // �J��Ԃ��i���[�v�j�񐔕ێ��p
    byte com=0;                         // �V���A��COM�|�[�g�ԍ�
    byte value;                         // �����[�g�q�@����̓��͒l
    byte dev[8];                        // XBee�q�@�f�o�C�X�̃A�h���X

    if(argc==2) com=(byte)atoi(argv[1]);// ����������Εϐ�com�ɑ������
    xbee_init( com );                   // XBee�pCOM�|�[�g�̏�����(�����̓|�[�g�ԍ�)
    
    printf("XBee in Commissioning\n");  // �҂��󂯒��̕\��
    if(xbee_atnj(30) == 0){             // �f�o�C�X�̎Q���󂯓�����J�n�i�ő�30�b�ԁj
        printf("No Devices\n");         // �G���[���̕\��
        exit(-1);                       // �ُ�I��
    }else{
        printf("Found a Device\n");     // XBee�q�@�f�o�C�X�̔����\��
        xbee_from( dev );               // �������f�o�C�X�̃A�h���X��ϐ�dev�Ɏ捞��
        xbee_ratnj(dev,0);              // �q�@�ɑ΂��đ��@�̎󂯓��ꐧ����ݒ�
        for(i=0;i<8;i++){
            printf("%02X ",dev[i]);    // �A�h���X�̕\��
        }
        printf("\n");
    }
    // �����̈��iXBee�q�@�̃|�[�g�P�`�R�̏�Ԃ��擾���ĕ\������j
    while(1){
        for(i=1;i<=3;i++){
            value=xbee_gpi(dev,i);       // XBee�q�@�̃|�[�gi�̃f�W�^���l���擾
            printf("D%d:%d ",i,value);  // �\��
        }
        printf("\n");
        delay(1000);
    }
}
