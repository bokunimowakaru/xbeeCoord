/*********************************************************************
�T���v���A�v���X�@My�X�}�[�g�v���O�Q�i�d�̓Z���TPIC�p�j

���̃T���v���́A�d�̓Z���TPIC�𓋍ڂ�������̉��ǔŃX�}�[�g�v���O��p�ł��B

���� XBee Smart Plug�𔭌�����ƁA���̃f�o�C�X����Ɠx�Ɖ��x�A����d�͂𓾂܂��B
�����ӁF����l�͖ڈ��ł��B���ɂ���ĕϓ����܂��B

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

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

	�d���Z���T�[
			30A�N�����v�^�d���Z���T SEN-11005 (sparkfun)
			ECHUN Electronic ECS1030-L72
			http://www.sparkfun.com/products/11005
			http://strawberry-linux.com/catalog/items?code=18214
			
			Turnn ratio Np:Ns=1:2000
			Current Ratio 30A/15mA
			
			PIC_ADC���͓d��(@1ch)
			
				PIC_ADCpp[mV] �� AC[mA]/2000*RL[Ohm]*2*sqrt(2)
			��	AC[A] = PIC_ADCpp[mV]/(RL*sqrt(2))
				AC[W] = PIC_ADCpp[mV]/(RL/100*sqrt(2))
			
			(RL = 1k��)�̎�(@1ch)
				AC[A] = ADCpp[mV]/707.1	
				AC[W] = ADCpp[mV]/7.071		�ő�467W(=3300/7.071@1ch)�܂�
				
			(RL = 75��)�̎�(1@ch)
				AC[A] = ADCpp[mV]/106.07
				AC[W] = ADCpp[mV]/1.0607		�ő�3111W(=3300/1.0607@1ch)�܂�
				
			16 bit ���͎�(@2ch) �L���r�b�g���� 6 + 5 bit = 11bit
			
				655536 �i�K 3300mV 
				
				value	= (PIC_ADC1[mV]+PIC_ADC2[mV])/3300*65536
						= (I1[mA]+I2[mA])/2[ch]/(V[V]*sqrt2)/2000*RL[Ohm]/3300*65536
				
				AC12[W] = value(16bit) * 2 * 100 * sqrt(2) * 2 / RL * 3300 / 65536
				
				(RL = 75��)�̎�(2@ch)
				AC12[W] = value(16bit) * 0.37980;
								
				*/
				#define CTSENSER_MLTPL 0.37980				// ���ג�R75�� �ő�3000W x 2ch
				/*
				#define CTSENSER_MLTPL 0.37980				// ���ג�R75�� �ő�3000W x 2ch
			
				�ő� 6222.3W���͎� value = 16384 ��� ���2�r�b�g���s�v
			
			PWM 8 bit �o�͂� ADC���͎�(�J����)
			
				�ő� 6222W����256
				AC12[W] = PWM_value(8bit) * 2 * 100 * sqrt(2) * 2 / RL * 3300 / 65536 * 2^6
				
				XBee_value = ( 3300 * PWM_value / 256 ) * 10k / (10k + 18k) / 1200 * 1024;
				
				AC12[W] = XBee_value/3300*256/10*28*1200/1024 * 2*100*sqrt(2)*2/RL*3300/65536*64
				        = XBee_value * 6.187184325
				
				(RL = 75��)�̎�(2@ch)

				*/
				#define CTSENSER_ADC_MLTPL 6.187184325		// ���ג�R75�� �d������ 10k/(10k+18k)
				/*
				#define CTSENSER_ADC_MLTPL 6.187184325		// ���ג�R75�� �d������ 10k/(10k+18k)
				
				
				2090W 1580h 5504
				3767W 609
				
							
	ADC[mV] = (float)ADC /1023.0 * 1200.0
			= (float)ADC*1.17302;
			
	�m�F
			����l �d�̓��[�^�̕\���l =  [W}
			
			�d�̓��[�^(400 Rev/kWh)1��]���� = �b
			
			����d�� = 3600(�b)��1��]����(�b)��0.400(Rev/Wh)



*/

//#define	H3694			//�d�v�FH3694��define�����H8�}�C�R���p�ɂȂ�܂��B
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
	byte data;
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
		// �����T���v�����O��ATIS<����>�Őݒ肵�܂��B���Ԃ�ms��16�i��2���܂łł��B
		// ���̏ꍇ��xbee_rx_call�̉����́uMODE_RESP�v�ɂȂ�܂��B
		// ��҂̏ꍇ�͂��̂܂܂œ��삵�܂��B���̏ꍇ�̉����́uMODE_GPIN�v�ɂȂ�܂��B
		// http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-sleep_cyclic.html

		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;	// ������M���[�h�ɐݒ�
		data = xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B(������50ms������܂�)
										// data�F��M���ʂ̑�\�l
										// xbee_result�F�ڍׂȎ�M����
										// xbee_result.MODE�F��M���[�h
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// �X�}�[�g�v���O�ւ̃f�[�^�v���ɑ΂��鉞��
			case MODE_GPIN:				// �X�}�[�g�v���O������������Ńf�[�^��M����
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 ){
					// �d�͑��茋��
					value = (float)xbee_result.ADCIN[3] * CTSENSER_ADC_MLTPL;
					if( value < 1.0 ) value_i = 0;
					else value_i = (unsigned int)value;
					lcd_cls(); lcd_goto(LCD_ROW_1);lcd_putstr("ADC="); lcd_disp_5( (unsigned int)xbee_result.ADCIN[3] );
					lcd_goto(LCD_ROW_3); lcd_disp_5( value_i ); lcd_putstr(" Watts"); lcd_putch('\n');
				}
				break;
			case MODE_UART:
				/* �V���A���f�[�^����M */
				lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("UART[");
				lcd_disp_hex(data); lcd_putch(']');		// �f�[�^����\��
				value = 0.;
				for(i=0;i<data;i++){
					if( xbee_result.DATA[i] >= (byte)'0' &&  xbee_result.DATA[i] <= (byte)'9' ){
						value *= 16;
						value += xbee_result.DATA[i] - (byte)'0' ;
					} else if ( xbee_result.DATA[i] >= (byte)'A' &&  xbee_result.DATA[i] <= (byte)'F' ){
						value *= 16;
						value += xbee_result.DATA[i] - (byte)'A' + 10 ;
					}
					if( xbee_result.DATA[i] >= 32 && xbee_result.DATA[i] <= 126 ) lcd_putch( xbee_result.DATA[i] );
				}
				// lcd_putch( '=' );
				// lcd_disp_5( (unsigned int) value );
				// lcd_putch( '\n' );
				value *= CTSENSER_MLTPL;
				if( value < 0.0 ) value_i = 0;
				else value_i = (unsigned int)value;
				lcd_goto(LCD_ROW_3); lcd_disp_5( value_i ); lcd_putstr(" Watts"); lcd_putch('\n');
				break;
			default:
				break;
		}
		// lcd_goto(LCD_ROW_4); lcd_disp_hex(trig);
	}
}
