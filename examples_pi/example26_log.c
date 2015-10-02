/***************************************************************************************
�擾���������t�@�C���ɕۑ����郍�K�[�̐���

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define S_MAX   255                             // ������ϐ�s�̍ő�e��(������-1)���`

int main(int argc,char **argv){

    byte com=0;                                 // �V���A��COM�|�[�g�ԍ�
    byte dev[8];                                // XBee�q�@�f�o�C�X�̃A�h���X
    float value;                                // ��M�f�[�^�̑���p
    XBEE_RESULT xbee_result;                    // ��M�f�[�^(�ڍ�)

    FILE *fp;                                   // �o�̓t�@�C���p�̃|�C���^�ϐ�fp���`
    char filename[] = "data.csv";               // �t�@�C����
    time_t timer;                               // �^�C�}�[�ϐ��̒�`
    struct tm *time_st;                         // �^�C�}�[�ɂ�鎞���i�[�p�̍\���̒�`
    char s[S_MAX];                              // ������p�̕ϐ�

    if(argc==2) com=(byte)atoi(argv[1]);        // ����������Εϐ�com�ɑ������
    xbee_init( com );                           // XBee�pCOM�|�[�g�̏�����
    xbee_atnj( 0xFF );                          // �q�@XBee�f�o�C�X����ɎQ���󂯓���
    printf("Waiting for XBee Commissoning\n");  // �҂��󂯒��̕\��

    while(1){
        
        time(&timer);                               // ���݂̎�����ϐ�timer�Ɏ擾����
        time_st = localtime(&timer);                // timer�l�������ɕϊ�����time_st��
        
        xbee_rx_call( &xbee_result );               // �f�[�^����M
        switch( xbee_result.MODE ){                 // ��M�����f�[�^�̓��e�ɉ�����
            case MODE_GPIN:                         // �q�@XBee�̎������M�̎�M
                value = (float)xbee_result.ADCIN[1] * 3.55;
                strftime(s,S_MAX,"%Y/%m/%d, %H:%M:%S", time_st);    // ������������ϊ�
                sprintf(s,"%s, %.1f", s , value );                  // ���茋�ʂ�s�ɒǉ�
                printf("%s Lux\n" , s );                            // ������s��\��
                if( (fp = fopen(filename, "a")) ) {                 // �t�@�C���I�[�v��
                    fprintf(fp,"%s\n" , s );                        // ������s����������
                    fclose(fp);                                     // �t�@�C���N���[�Y
                }else printf("fopen Failed\n");
                break;
            case MODE_IDNT:                         // �V�����f�o�C�X�𔭌�
                printf("Found a New Device\n");
                xbee_atnj(0);                       // �q�@XBee�f�o�C�X�̎Q���𐧌�����
                bytecpy(dev, xbee_result.FROM, 8);  // ���������A�h���X��dev�ɃR�s�[����
                xbee_ratnj(dev,0);                  // �q�@�ɑ΂��đ��@�̎󂯓���𐧌�
                xbee_gpio_config( dev, 1 , AIN );   // XBee�q�@�̃|�[�g1���A�i���O���͂�
                xbee_end_device( dev, 3, 3, 0);     // �N���Ԋu3�b,��������3�b,S�[�q����
                break;
        }
    }
}
