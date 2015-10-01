/*********************************************************************
�T���v���A�v���T�@�X�}�[�g�v���O

XBee Smart Plug(Digi �����i)�𔭌�����ƁA���̃f�o�C�X����Ɠx�Ə���
�d�͂𓾂܂��B�Â��Ȃ��Smart Plug�ɐڑ������@��̓d�����؂�܂��B

�V����XBee�q�@�̃R�~�b�V�����{�^��DIO0(XBee_pin 20)����������(�M�����x
����H��L��H�ɐ��ڂ���)�ƁA�q�@����Q���v���M�������s����āAPC�܂���
H8�e�@���q�@�𔭌����A�ufound a device�v�ƕ\�����܂��B

�������A������ZigBee�l�b�g���[�N�ɑΉ����邽�߂ɁA�y�A�����O�͋N�����
30�b�Ԃ����󂯕t���܂���BPC/H8�e�@���N�����āA�R�~�b�V�����{�^����
�����ăy�A�����O�����{���Ă����A�ēx�A�R�~�b�V�����{�^���Ńf�o�C�X����
���܂��B�f�o�C�X������30�b�̃y�A�����O���Ԍ�ł��s���܂��B

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

�����ӁF�p�\�R����t�@���q�[�^�Ȃ�AC�d���̓r�₦��ƌ̏�⎖�̂̔�������
�@�@���Smart Plug�ɐڑ����Ȃ����ƁB
                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port�w��	IO��	�s���ԍ�			�Z���T�[�f�o�C�X
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 �Ɠx�Z���T�[
			port= 2		DIO2	XBee_pin 18 (AD2)	 ���x�Z���T�[
			port= 3		DIO3	XBee_pin 17 (AD3)	 AC�d���Z���T�[
*/





#include "../libs/xbee.c"

void set_plug_adc(byte *dev){
	printf("Found a Device\n");
	xbee_from( dev );					// ��M�����A�h���X��dev_sens�֊i�[
	xbee_gpio_config( dev, 1 , AIN );	// �ڑ�����̃|�[�g1��AIN���͂�
	xbee_gpio_config( dev, 2 , AIN );	// �ڑ�����̃|�[�g2��AIN���͂�
	xbee_gpio_config( dev, 3 , AIN );	// �ڑ�����̃|�[�g3��AIN���͂�
}

int main(int argc,char **argv){
	byte trig=0;
	byte dev_en = 0;	// �Z���T�[�����̗L��(0:������)
	XBEE_RESULT xbee_result;
	float value;
	byte port=0;
	byte dev_sens[8];
	
	// ����������
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	
	// �f�o�C�X�T��
	printf("Searching:Smart Plug\n");
	if( xbee_atnj(30) ){				// 30�b�Ԃ̎Q���󂯓���
		dev_en = 1;						// sensor������
		set_plug_adc(dev_sens);			// �ݒ�
	}else printf("Failed:no devices\n");

	// ���C������
	printf("Receiving\n");
	while(1){							// �i�v�Ɏ�M����
		if( dev_en && trig == 0){
			xbee_force( dev_sens );		// �f�o�C�Xdev_sens�փf�[�^�v��(100���1��̕p�x��)
			trig = 100;
		}
		trig--;
		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B(������50ms������܂�)
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// sensor����̉����̏ꍇ�ɏƓx�Ɖ��x��\��
			case MODE_GPIN:				// ���������Ńf�[�^��M�����ꍇ���z��
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 &&
				xbee_result.STATUS == STATUS_OK ){
					// �Ɠx���茋��
					value = xbee_sensor_result( &xbee_result,LIGHT);
					printf("%.0f Lux, ",value);
					// �Â��Ƃ��͓d����؂�A���邢�Ƃ��͓d��������
					if( value < 1.0 ){
						xbee_gpo(dev_sens , 4 , 0 );	// Turns outlet off (Port 4 = 0)
					}else{
						xbee_gpo(dev_sens , 4 , 1 );	// Turns outlet on (Port 4 = 1)
					}
					// ���x���茋�� (�����Ŋm�F�����Ƃ���7.12���̍��߂��\������܂����ADigi�Ђ̎d�l�ł��B)
					value = xbee_sensor_result( &xbee_result,TEMP);
					printf("%.1f degC, ",value);
					// �d�͑��茋��
					value = xbee_sensor_result( &xbee_result,WATT);
					printf("%.1f Watts\n",value);
				}
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				dev_en = 1;				// sensor������
				set_plug_adc(dev_sens);			// �ݒ�
				break;
		}
	}
}
