/*********************************************************************
������XBee����V���A���Ŏ�M�������e���t�@�C���֕ۑ�����B(�A�h���X.csv)
Smart Plug �ő��肵�����ʂ��t�@�C���֕ۑ�����B(smartplug.csv)

�����̃\�[�X��PC�p�ł��B������H8��Arduino�ł͓��삵�܂���B������

�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
//#define ERRLOG			//ERRLOG��define����ƃG���[���O���o�͂���܂��B

#include "../libs/lcd_pc.c"
#include "../libs/xbee.c"
#include "../libs/kbhit.c"
#include "../libs/hex2a.c"

int main(int argc,char **argv){
	FILE *fp;
	char filename[] = "01234567.csv";

	time_t timer;
	struct tm *time_st;

	byte i;
	byte data;
	byte trig=0;
	byte dev_1_en = 0;		// �Z���T�[�����̗L��(0:������)
	byte dev_tmp_en = 0;	// �Z���T�[�����̗L��(0:������)
	XBEE_RESULT xbee_result;
	float value_lux;
	float value_temp;
	float value_watts;
	unsigned int value_i;

	byte port=0;		// �����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_1[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// Smart Plug
	byte dev_tmp[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	// �V���A����M�p(���͕s�v)
	
	// ����������
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	xbee_atnj( 0 );				// �W���C���֎~
		
	wait_millisec( 1000 );
	
	// �f�o�C�X�T��
	lcd_disp("Searching:Smart Plug");
	if( xbee_ping( dev_1 ) != DEV_TYPE_PLUG ){	// DEV_TYPE_PLUG 0x0F (Smart Plug)
		if( xbee_atnj(10) ){				// 10�b�Ԃ̎Q���󂯓���
			lcd_disp("Found device");
			xbee_from( dev_1 );			// ��M�����A�h���X��dev_1�֊i�[
			dev_1_en = 1;
		}else{
			lcd_disp("Failed:no dev.");
		}
	}
	if( dev_1_en ){
		xbee_ratnj( dev_1, 0 );		// �W���C���֎~
		xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
	}
	
	wait_millisec( 1000 );
	
	// ���C������
	lcd_disp("Receiving");
	while(1){							// �i�v�Ɏ�M����
		/* �L�[�{�[�h����̓��� */
		if ( kbhit() ) data = (byte)getchar(); else data = 0x00;
		switch( data ){
			case 'n':	// 10�b�ԁA�l�b�g���[�N�Q����������
				lcd_disp("atnj 10 seconds");
				if( xbee_atnj(10) ){
					lcd_disp("Found device");
				}
				lcd_disp("done");
				break;
			case 'b':
				lcd_disp("batt");
				if( dev_1_en ) printf("dev_1  :%d[mV]\n", xbee_batt(dev_1) );
				if( dev_tmp_en ) printf("dev_tmp:%d[mV]\n", xbee_batt(dev_tmp) );
				lcd_disp("done");
				break;
			case 'd':
				lcd_disp("devices");
				lcd_goto(LCD_ROW_1); lcd_putstr("dev 1  :");
				for(i=4; i<8 ; i++) lcd_disp_hex(dev_1[i]);
				lcd_goto(LCD_ROW_4); lcd_putstr("dev tmp:");
				for(i=4; i<8 ; i++) lcd_disp_hex(dev_tmp[i]);
				lcd_disp("done");
				break;
			default:
				break;
		}
		
		/* �^�C�}�[�ɂ�������� */
		time(&timer);
		time_st = localtime(&timer);
		if( time_st->tm_min != trig && dev_1_en ){	// �u���v�ɕω�����������
			xbee_force( dev_1 );		// �f�o�C�Xdev_1�փf�[�^�v��
			trig = time_st->tm_min;
		}
		
		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;	// ������M���[�h�ɐݒ�
		data = xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B(������50ms������܂�)
										// data�F��M���ʂ̑�\�l
										// xbee_result�F�ڍׂȎ�M����
										// xbee_result.MODE�F��M���[�h
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// sensor����̉����̏ꍇ�ɏƓx�Ɖ��x��\��
				/* Smart Plug����̃f�[�^����M */
				if( xbee_result.AT[0]=='I' &&
					xbee_result.AT[1]=='S' &&
					bytecmp( dev_1 , xbee_result.FROM ,8 ) == 0 &&
					xbee_result.STATUS == STATUS_OK
				){
					// �Ɠx���茋��
					value_lux = xbee_sensor_result( &xbee_result,LIGHT);	
					lcd_cls(); lcd_goto(LCD_ROW_1); lcd_disp_5( (unsigned int)value_lux ); lcd_putstr(" Lux");
					/* �Â��Ƃ��͓d����؂�A���邢�Ƃ��͓d��������
					if( value < 1.0 ){
						xbee_gpo(dev_1 , 4 , 0 );	// Turns outlet off (Port 4 = 0)
					}else{
						xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
					}
					*/
					// ���x���茋��
					value_temp = xbee_sensor_result( &xbee_result,TEMP);
					i = (byte)value_temp; lcd_goto(LCD_ROW_2); lcd_disp_2( i ); lcd_putstr(".");
					i = (byte)((value_temp-(float)i)*100); lcd_disp_2( i ); lcd_putstr(" degC");
					// �d�͑��茋��
					value_watts = xbee_sensor_result( &xbee_result,WATT);
					if( value_watts < 0 ) value_watts = 0;
					value_i = (unsigned int)value_watts;
					lcd_goto(LCD_ROW_3); lcd_disp_3( value_i ); lcd_putstr(" Watts"); lcd_putch('\n');
					if( (fp = fopen("smartplug.csv", "a")) ) {
						fprintf(fp, "%d/%d/%d,%d:%d, ",
							time_st->tm_year+1900,
							time_st->tm_mon+1,
							time_st->tm_mday,
							time_st->tm_hour,
							time_st->tm_min
						);
						fprintf(fp, "%f, %f, %f\n",value_lux,value_temp,value_watts);
						fclose(fp);
					}else lcd_disp("Failed fopen");
				}else lcd_disp("recieved RAT_RESP");
				break;
			case MODE_UART:
				/* �V���A���f�[�^����M */
				xbee_from( dev_tmp );	// �A�h���X��dev_tmp�ɓǂݍ���
				dev_tmp_en = 1;			// �t���O
				lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("data[");
				lcd_disp_hex(data); lcd_putch(']');		// �f�[�^����\��
				for(i=0;i<data;i++)lcd_putch( xbee_result.DATA[i] );lcd_putch( '\n' );
				for(i=0;i<4;i++) hex2a( dev_tmp[i+4], &(filename[i*2]) );
				if( (fp = fopen( filename, "a")) ) {
					fprintf(fp, "%d/%d/%d,%d:%d, ",
						time_st->tm_year+1900,
						time_st->tm_mon+1,
						time_st->tm_mday,
						time_st->tm_hour,
						time_st->tm_min
					);
					for(i=0;i<data;i++){
						if( xbee_result.DATA[i] >=' ' && xbee_result.DATA[i] <= 126)
						fputc( xbee_result.DATA[i], fp);	// �����̏ꍇ�ɏ����o��
					}
					fputc( '\n', fp);
					fclose(fp);
				}else lcd_disp("Failed fopen");
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				xbee_from( dev_tmp );	// ���������A�h���X��dev_tmp�ɓǂݍ���
				if( bytecmp( dev_1 , dev_tmp ,8 ) ){
					lcd_disp("found a new device");
					if ( xbee_ping( dev_tmp ) == DEV_TYPE_PLUG ) {
						for(i=0;i<8;i++) dev_1[i]=dev_tmp[i];
						dev_1_en = 1;
						xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
						lcd_disp("registed Smart Plug");
					}
				}
				lcd_goto(LCD_ROW_1); lcd_putstr("dev_tmp:");
				for(i=4; i<8 ; i++) lcd_disp_hex( dev_tmp[i] );
				break;
			default:
				break;
		}
	}
}
