/*********************************************************************
�T���v���A�v���U�@My�X�}�[�g�v���O
���� XBee Smart Plug�𔭌�����ƁA���̃f�o�C�X����Ɠx�Ɖ��x�A����d�͂𓾂܂��B
�����ӁF����l�͖ڈ��ł��B���ɂ���ĕϓ����܂��B

�g�p���@�F
http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-plug.html

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
			port= 3		DIO3	XBee_pin 17 (AD3)	 �d���Z���T�[ 
			
�Z���T�[�f�o�C�X�̌^�ԂƏ��

	�Ɠx�Z���T�[
			�Ɠx�Z���T�i�t�H�g�g�����W�X�^�j�V���{�����m�i�k�V�T�O�Q�k
			http://akizukidenshi.com/catalog/g/gI-02325/
			
			���d���R�R�ʂ`�@�i���FLED, 100Lux�j �A��A �������S�U�ʂ`

				AIN[mV] = LUX * 0.33e-3[mA] * RL[��]
			�� LUX[lux] = AIN[mV]/3.3		(RL = 10k��)�̎�

	���x�Z���T�[
			�����x�h�b���x�Z���T National �k�l�U�P�b�h�y
			http://akizukidenshi.com/catalog/g/gI-02726/
			
				AIN[mV] ���i�{�P�O���u�^���~�s���j�{�U�O�O���u
			�� T[deg C] = AIN[mV]/10-60

	�d���Z���T�[
			30A�N�����v�^�d���Z���T SEN-11005 (sparkfun)
			ECHUN Electronic ECS1030-L72
			http://www.sparkfun.com/products/11005
			http://strawberry-linux.com/catalog/items?code=18214
			
			Turnn ratio Np:Ns=1:2000
			Current Ratio 30A/15mA
			AIN���͂̕�����R��DIV = 47k/(47k+47k)
			
				AIN[mV] �� AC[mA]/2000*RL*DIV*sqrt(2)[Ohm]
			��	AC[A] = AIN[mV]/(RL*DIV/2*sqrt(2))
				AC[W] = AIN[mV]/(RL*DIV/200*sqrt(2))
				
			(RL = 1k�� DIV=0.5)�̎�
				AC[A] = AIN[mV]/353.6
				AC[W] = AIN[mV]/3.536		�A�� �ő�339W�܂�
											OPA��Voh�Ŏ��ۂɂ�300W���炢�܂�
			(RL = 330�� DIV=0.5)�̎�
				AC[A] = AIN[mV]/116.7
				AC[W] = AIN[mV]/1.167		�A�� �ő�1028W�܂�
											OPA��Voh�Ŏ��ۂɂ�900W���炢�܂�
			(RL = 100�� DIV=0.5)�̎�
				AC[A] = AIN[mV]/35.36
				AC[W] = AIN[mV]/0.3536		�A�� �ő�3390W�܂�
											OPA��Voh�Ŏ��ۂɂ�3kW���炢�܂�
											
			*/
			#define CTSENSER_MLTPL 1.17302 / 3.536		// ���ג�R1k�� �ő�300W
			/*
			#define CTSENSER_MLTPL 1.17302 / 3.536		// ���ג�R1k�� �ő�300W
			#define CTSENSER_MLTPL 1.17302 / 1.167		// ���ג�R330�� �ő�900W
			#define CTSENSER_MLTPL 1.17302 / 0.3536		// ���ג�R100�� �ő�3kW
			
	AIN[mV] = (float)AIN /1023.0 * 1200.0
			= (float)AIN*1.17302;
			
	�m�F
			����l �d�̓��[�^��N�o�� 835W
			�d�̓��[�^(400 Rev/kWh)1��]���� 42.9/4 =10.7�b
			����d�� = 3600(�b)��1��]����(�b)��0.400(Rev/Wh)
			�� 841 W
			�قڈ�v OK
*/

#define	H3694			//�d�v�FH3694��define�����H8�}�C�R���p�ɂȂ�܂��B
//#define ERRLOG			//ERRLOG��define����ƃG���[���O���o�͂���܂��B

#ifdef H3694
	#ifndef __3694_H__
		#include <3694.h>
	#endif
	#include "../libs/lcd_h8.c"
#else
	#include "../libs/lcd_pc.c"
#endif
#include "../libs/xbee.c"

int main(int argc,char **argv){
	byte i;
	byte trig=0;
	byte dev_en = 0;	// �Z���T�[�����̗L��(0:������)
	XBEE_RESULT xbee_result;
	float value;
	unsigned int value_i;

	byte port=0;		// �����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// PIO �f�o�C�X
	
	// ����������
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	
	// �f�o�C�X�T��
	lcd_disp("Searching:Smart Plug");
	if( xbee_atnj(30) ){				// 30�b�Ԃ̎Q���󂯓���
		lcd_disp("Found device");
		xbee_from( dev_sens );			// ��M�����A�h���X��dev_sens�֊i�[
		dev_en = 1;						// sensor������
		xbee_gpio_config( dev_sens, 1 , AIN ); // �ڑ�����̃|�[�g1��AIN���͂�
		xbee_gpio_config( dev_sens, 2 , AIN ); // �ڑ�����̃|�[�g2��AIN���͂�
		xbee_gpio_config( dev_sens, 3 , AIN ); // �ڑ�����̃|�[�g3��AIN���͂�
	}else{
		lcd_disp("Failed:no dev.");
		exit(-1);
	}
	wait_millisec( 1000 );
	
	// ���C������
	lcd_disp("Receiving");
	while(1){							// �i�v�Ɏ�M����

		/* �e�@����X�}�[�g�v���O�փf�[�^��v������ꍇ�͈ȉ�5�s���g�p���� */
		if( dev_en && trig == 0){
//			xbee_force( dev_sens );		// �f�o�C�Xdev_sens�փf�[�^�v��(100���1��̕p�x��)
			trig = 100;
		}
		trig--;

		// �Q�l���
		// �d�r�쓮����X�}�[�g�v���O�̏ȓd�͉��̕��@�Ƃ��āu�T���v�����O�v�������v��
		// �u�����T���v�����O�����v������A�O�҂̏ꍇ�͏�L�̂T�s��L���ɂ��܂��B
		// ���̏ꍇ��xbee_rx_call�̉����́uMODE_RESP�v�ɂȂ�܂��B
		// ��҂̏ꍇ�͂��̂܂܂œ��삵�܂��B���̏ꍇ�̉����́uMODE_GPIN�v�ɂȂ�܂��B
		// http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-sleep_cyclic.html

		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;	// ������M���[�h�ɐݒ�
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B(������50ms������܂�)
										// data�F��M���ʂ̑�\�l
										// xbee_result�F�ڍׂȎ�M����
										// xbee_result.MODE�F��M���[�h
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// �X�}�[�g�v���O�ւ̃f�[�^�v���ɑ΂��鉞��
			case MODE_GPIN:				// �X�}�[�g�v���O������������Ńf�[�^��M����
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 ){
					// �Ɠx���茋��
					value = (float)xbee_result.ADCIN[1] * 1.17302 / 3.3;
					lcd_cls(); lcd_goto(LCD_ROW_1); lcd_disp_5( (unsigned int)value ); lcd_putstr(" Lux");
					// ���x���茋��
					value = (float)xbee_result.ADCIN[2] * 1.17302 / 10. - 60.;
					i = (byte)value; lcd_goto(LCD_ROW_2); lcd_disp_2( i ); lcd_putstr(".");
					i = (byte)((value-(float)i)*100); lcd_disp_2( i ); lcd_putstr(" degC");
					// �d�͑��茋��
					value = (float)xbee_result.ADCIN[3] * CTSENSER_MLTPL;
					if( value < 1.0 ) value_i = 0;
					else value_i = (unsigned int)value;
					lcd_goto(LCD_ROW_3); lcd_disp_3( value_i ); lcd_putstr(" Watts"); lcd_putch('\n');
				}
				break;
			default:
				break;
		}
		// lcd_goto(LCD_ROW_4); lcd_disp_hex(trig);
	}
}
