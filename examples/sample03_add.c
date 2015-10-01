/*********************************************************************
�T���v���A�v���R�@�X�C�b�`

�V����XBee�q�@�̃R�~�b�V�����{�^��DIO0(XBee_pin 20)����������(�M�����x
����H��L��H�ɐ��ڂ���)�ƁA�q�@����Q���v���M�������s����A�{�v���O����
�����삷��e�@���q�@�𔭌����A�܂��q�@��GPIO��ݒ肵�܂��B
���̌�A�q�@�̃X�C�b�`2�`4(Port1�`3)�����������ɁA�X�C�b�`��Ԃ���ʂ�
�\�����܂��B
�q�@�������䂠���Ă��A���ꂼ��̏�Ԃ���M���邱�Ƃ��o���܂��B

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port�w��	IO��	�s���ԍ�			USB�]���{�[�h(XBIB-U-Dev)
			port= 0		DIO0	XBee_pin 20 (Cms)	    SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	    SW2
			port= 2		DIO2	XBee_pin 18 (AD2)	    SW3
			port= 3		DIO3	XBee_pin 17 (AD3)	    SW4
*/





#include "../libs/xbee.c"

int main(int argc,char **argv){
	byte i;
	XBEE_RESULT xbee_result;

	byte port=0;
	byte dev_gpio[8];
	
	// ����������
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	xbee_atnj( 0xFF );					// �f�o�C�X����ɎQ���󂯓���(�e�X�g�p)
	
	// ���C������
	printf("Receiving\n");
	while(1){							// �i�v�Ɏ�M����
		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;			// ������M���[�h�ɐݒ�
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_GPIN:				// GPIO���͂����o(�����[�g�@�̃{�^���������ꂽ)
				// �X�C�b�`�̏�Ԃ�\��
				for( i=1;i<=3;i++ ){
					printf("SW%d:%d ",i,(xbee_result.GPI.BYTE[0]>>i)&0x01);
				}
				// ���M���A�h���X��\��
				printf("from:");
				for( i=0;i<4;i++ ) printf("%02X",xbee_result.FROM[i] );
				printf(".");
				for( i=4;i<8;i++ ) printf("%02X",xbee_result.FROM[i] );
				printf("\n");
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				for(i=0;i<8;i++) dev_gpio[i]=xbee_result.FROM[i];
										// ���������A�h���X��dev_gpio�ɓǂݍ���
				xbee_gpio_init( dev_gpio );
				printf("found a new device\n");
				break;
			default:
				break;
		}
	}
}
