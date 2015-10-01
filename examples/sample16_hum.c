/*********************************************************************
�T���v���A�v��16�@����E���x�Z���T�[ HS-15��p
�V�����Z���T�[�f�o�C�X�𔭌�����ƁA���̃f�o�C�X���玼�x�𓾂܂��B

�ڍׁFhttp://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-hum.html

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
*/
//#define DEBUG
#define XBEE_ERROR_TIME
#include "../libs/xbee.c"

#define LUX_ADJ		1.0						// �Ɠx�Z���T�̕␳�l
#define TEMP_ADJ	+1.8					// ���x�Z���T�̕␳�l
#define BATT_CHECK	100						// �d���m�F�Ԋu(0�`255)

byte local_at(char *at){
	byte ret=0;
	char s[40];			// �\���p
	
	ret=xbee_at(at);
	sprintf(s,"local_at  %s ret=",at);
	xbee_log(3,s,ret);
	return(ret);
}

byte remote_at(byte *dev,char *at){
	byte ret=0;
	char s[40];			// �\���p
	
	ret=xbee_rat(dev,at);
	sprintf(s,"remote_at %s ret=",at);
	xbee_log(3,s,ret);
	return(ret);
}

byte set_ports(byte *dev){
	byte ret=0;
	/* �q�@�̐ݒ� */
	if( (ret=remote_at(dev,"ATPR1FF1"))==0 )	// �|�[�g1�`3�̃v���A�b�v������
	if( (ret=remote_at(dev,"ATD102"))==0 )		// �|�[�g1���A�i���O���͂�
	if( (ret=remote_at(dev,"ATD202"))==0 )		// �|�[�g2���A�i���O���͂�
	if( (ret=remote_at(dev,"ATD302"))==0 )		// �|�[�g3���A�i���O���͂�
	if( (ret=remote_at(dev,"ATST01F4"))==0 )	// �X���[�v�ڍs�҂�����	ST=500ms
	if( (ret=remote_at(dev,"ATSP07D0"))==0 )	// �X���[�v�Ԋu	 20�b	SP=2000�~10ms
	if( (ret=remote_at(dev,"ATSN01"))==0 )		// �X���[�v��	 1�� (���uATSN3C�v��20����)
	if( (ret=remote_at(dev,"ATIR1388"))==0 )	// �������M����			IR=5000ms
	if( (ret=remote_at(dev,"ATWH00"))==0 )		// ����҂�����			WH=0ms
	if( (ret=remote_at(dev,"ATSM04"))==0 )		// �T�C�N���b�N�X���[�v	SM=04
	if( (ret=remote_at(dev,"ATSI"))==0 )		// ���X���[�v��
//	if( (ret=remote_at(dev,"ATWR"))==0 )		// EEPROM�֏�������
	;
	if(ret) xbee_log(5,"ERROR at set_ports",ret);
	else{
		xbee_delay(1000);
		xbee_log(3,"Sleep",remote_at(dev,"ATSI") );
	}
	return(ret);
}

int main(int argc,char **argv){
	char s[40];						// �\���p
//	byte dev_en = 0;				// �Z���T�[�����̗L��(0:������)
	XBEE_RESULT xbee_result;
	float lux,temp,hum,batt=3.0;	// �Ɠx[Lux]�A���x[��]�A���x[%]�A�d�r�d��[V]
	byte trig_batt=BATT_CHECK;

	byte port=0;		// �����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// �Z���T�f�o�C�X
	
	// ����������
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_log(3,"Started",port );
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	xbee_atnj(0xFF);					// �Q���󂯓���
	local_at("ATSP0AF0");				// �e�@ SP=28000ms (28�b)
	local_at("ATSN0E10");				// �e�@ SN=3600�� (28�b*3600��=28����)	batt = 3.00;
	
	// ���C������
	xbee_log(3,"Receiving",xbee_atai() );
	while(1){							// �i�v�Ɏ�M����
		/* XBee�q�@�̓d������ */
		if( trig_batt == 0){
			xbee_batt_force(dev_sens);
			trig_batt=BATT_CHECK;
		}
		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B
										// data�F��M���ʂ̑�\�l
										// xbee_result�F�ڍׂȎ�M����
										// xbee_result.MODE�F��M���[�h
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// sensor����̉����̏ꍇ�ɏƓx�Ɖ��x��\��
				if( xbee_result.STATUS != STATUS_OK) break;
			case MODE_GPIN:				// sensor����̉����̏ꍇ�ɏƓx�Ɖ��x��\��
				sprintf(s,"Recieved %d,%d,%d, device=",xbee_result.ADCIN[1],xbee_result.ADCIN[2],xbee_result.ADCIN[3]);
				xbee_log(5,s,xbee_result.FROM[7] );
				if( bytecmp(xbee_result.FROM,dev_sens,8)==0 ){
					/* �Ɠx����
					    �Ɠx�Z���T    [+3.3V]----(C)[�Ɠx�Z���T](E)--+--[10k��]----[GND]
					    =NJL7502L                                    |
					                                                 +--(19)[XBee ADC1]
					                                                 
						lux = I(uA) / 0.3 = ADC(mV) / 10(kOhm) / 0.3
						    = ADC�l /1023 * 1200(mV) / 10(kOhm) / 0.3
						    = ADC�l * 0.391
					*/
					lux = (float)xbee_result.ADCIN[1] * 0.391 * LUX_ADJ;
					sprintf(s,"Ilum = %.1f[Lux], adc1",lux);
					xbee_log(5,s,(byte)(xbee_result.ADCIN[1]/4) );
					
					/* ���x����
					    ���x�Z���T    [+3.3V]----(1VSS)[���x�Z���T](3GND)---[GND]
					    =LM61CIZ                      (2VOUT)|
					                                         +--(18)[XBee ADC2]
					                                         
						Temp = ( ADC(mV) - 600 ) / 10 = ( ADC�l /1023 * 1200(mV) - 600 ) / 10
						     = ADC�l * 0.1173 - 60;
					*/
					temp = (float)xbee_result.ADCIN[2] * 0.1173 - 60 + TEMP_ADJ;
					sprintf(s,"Temp = %.1f[degC], adc2",temp);
					xbee_log(5,s,(byte)(xbee_result.ADCIN[2]/4) );
						
					/* ���x����
						���x�Z���T HS-15P
						[+3.3V]---[series100k]--+--[0.22uF]--+--[HS-15P]---[GND]
						           ����         |            |
						    [GND] --[shunt68k]--+            +--(17)[XBee ADC3]
						    
						���̑��̕ϊ�
							HS15���x�Z���T�[�ƊȈՉ�H�ɂ�鎼�x�v����
							http://homepage3.nifty.com/sudamiyako/zk/hs15/hs15.htm
					*/
					hum = (float)xbee_result.ADCIN[3] / batt * 3.256;
					hum = 7.4396E-05*hum*hum - 1.2391E-01*hum + 1.9979E+02;
					hum += (hum*35-13056)/64*temp/64;
					hum = (5*hum /16 *hum + 44* hum )/256 +20;
					
					sprintf(s,"Hum  = %.1f[%%], adc3",hum);
					xbee_log(5,s,(byte)(xbee_result.ADCIN[3]/4) );
					trig_batt--;
				}
				break;
			case MODE_BATT:				// �d�r�d������M
				if( bytecmp(xbee_result.FROM,dev_sens,8)==0 ){
					batt = (float)xbee_result.ADCIN[0] /1000.;
					sprintf(s,"Batt = %.3f[V], adc0",batt);
					xbee_log(5,s,(byte)(xbee_result.ADCIN[0]/15) );
					trig_batt=BATT_CHECK;
				}
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				xbee_from( dev_sens );	// ��M�����A�h���X��dev_sens�֊i�[
			//	dev_en = 1;				// sensor������
				xbee_log(3,"found a new device",dev_sens[7] );
				set_ports( dev_sens );
			//	xbee_atnj( 10 );
				trig_batt=0;
				break;
			default:
				break;
		}
	}
}
