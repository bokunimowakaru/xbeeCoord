/*********************************************************************
�T���v���A�v���S�@�Z���T�[
�V�����Z���T�[�f�o�C�X�𔭌�����ƁA���̃f�o�C�X����Ɠx�Ɖ��x�𓾂܂��B

�����ł͎q�@��XBee�Z���T�[��Digi������XBee Sensor��z�肵�Ă��܂����A
���삵���Z���T�[�ł�DIO1�`2(XBee_pin 19�`18)��0�`1.2V�̃A�i���O�d����
�^���Ă���΁A���炩�̒l��ǂ݂Ƃ邱�Ƃ��ł��܂��B

�V����XBee�q�@�̃R�~�b�V�����{�^��DIO0(XBee_pin 20)����������(�M�����x
����H��L��H�ɐ��ڂ���)�ƁA�q�@����Q���v���M�������s����āAPC�܂���
H8�e�@���q�@�𔭌����A�ufound a device�v�ƕ\�����܂��B

�������A������ZigBee�l�b�g���[�N�ɑΉ����邽�߂ɁA�y�A�����O�͋N�����
10�b�Ԃ����󂯕t���܂���BPC/H8�e�@���N�����āA�R�~�b�V�����{�^����
�����ăy�A�����O�����{���Ă����A�ēx�A�R�~�b�V�����{�^���Ńf�o�C�X����
���܂��B�f�o�C�X������10�b�̃y�A�����O���Ԍ�ł��s���܂��B

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port�w��	IO��	�s���ԍ�			�Z���T�[�f�o�C�X
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 �Ɠx�Z���T�[
			port= 2		DIO2	XBee_pin 18 (AD2)	 ���x�Z���T�[
			port= 3		DIO3	XBee_pin 17 (AD3)	 ���x�Z���T�[
			port=11		DIO11	XBee_pin  7 (DIO11)	 �d���Z���T�[
*/

#include "../libs/xbee.c"

void set_sensor_adc(byte *dev){
	printf("Found a Device\n");
	xbee_from( dev );					// ��M�����A�h���X��dev_sens�֊i�[
	xbee_gpio_config( dev, 1 , AIN );	// �ڑ�����̃|�[�g1��AIN���͂�
	xbee_gpio_config( dev, 2 , AIN );	// �ڑ�����̃|�[�g2��AIN���͂�
	xbee_gpio_config( dev, 11 , DIN );	// Digi�����Z���T�̃o�b�e���M��
}

int main(int argc,char **argv){
	byte trig=0;
	byte dev_en = 0;	// �Z���T�[�����̗L��(0:������)
	XBEE_RESULT xbee_result;
	float value;
	byte port=0;
	byte dev_sens[8];	// �Z���T�f�o�C�X
	
	// ����������
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	
	// �f�o�C�X�T��
	printf("Searching:SENSOR\n");
	if( xbee_atnj(10) ){				// 10�b�Ԃ̎Q���󂯓���
		dev_en = 1;						// sensor������
		set_sensor_adc(dev_sens);
	}else printf("Failed:no devices\n");
	
	// ���C������
	printf("Receiving\n");
	while(1){							// �i�v�Ɏ�M����
		if( dev_en && trig == 0){
			xbee_force( dev_sens );		// �f�o�C�Xdev_sens�փf�[�^�v��
			trig = 250;
		}
		trig--;

		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// sensor����̉����̏ꍇ�ɏƓx�Ɖ��x��\��
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 &&
				xbee_result.STATUS == STATUS_OK ){
					// �Ɠx���茋��
					value = xbee_sensor_result( &xbee_result,LIGHT);
					printf("%.0f Lux, ",value);
					// ���x���茋��
					value = xbee_sensor_result( &xbee_result,TEMP);
					printf("%.1f degC, ",value);
					//
					printf("%d\n",xbee_result.GPI.PORT.D11);
				}
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				dev_en = 1;				// sensor������
				set_sensor_adc(dev_sens);
				break;
		}
	}
}
