/***************************************************************************************
�l�b�g���[�N�ݒ�����Z�b�g����

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){

    byte port=0;                                // �V���A��COM�|�[�g�ԍ�
    byte ret=0;

    if( argc==2 ){
        if( atoi(argv[1]) < 0 ){
            port = 0x9F + (byte)(-atoi(argv[1])) ;
        }else  if( ( argv[1][0]=='b' || argv[1][0]=='B' )&& argv[1][1]!='\0' ){
            port = 0xB0 + ( argv[1][1] - '0');
        }else  if( ( argv[1][0]=='a' || argv[1][0]=='A' )&& argv[1][1]!='\0' ){
            port = 0xA0 + ( argv[1][1] - '0');
        }else port = (byte)(atoi(argv[1]));
    }
    xbee_init( port );                          // XBee�pCOM�|�[�g�̏�����
    printf("Restore Network Settings\n");                    // �G���[�\��
    ret = xbee_atcb(4);                         // �l�b�g���[�N�ݒ�����Z�b�g����
    if( ret==00 ){                              // �Í������I�t�ɂ���
        printf("SUCCESS\n");                    // �\��
    }else printf("Error\n");                    // �G���[�\��
    return( ret );
}
