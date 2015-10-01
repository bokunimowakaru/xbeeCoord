/*********************************************************************
�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

�����ӁF�p�\�R����t�@���q�[�^�Ȃ�AC�d���̓r�₦��ƌ̏�⎖�̂̔�������
�@�@���Smart Plug�ɐڑ����Ȃ����ƁB
�@
�����̃\�[�X��PC�p�ł��B������H8�ł͓��삵�܂��񁡁���

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
//#define ERRLOG			//ERRLOG��define����ƃG���[���O���o�͂���܂��B

#define CTSENSER_MLTPL 1.17302 / 0.3536		// ���ג�R100�� �ő�3kW
/*
#define CTSENSER_MLTPL 1.17302 / 3.536		// ���ג�R1k�� �ő�300W
#define CTSENSER_MLTPL 1.17302 / 1.167		// ���ג�R330�� �ő�900W
#define CTSENSER_MLTPL 1.17302 / 0.3536		// ���ג�R100�� �ő�3kW
*/

#include "../libs/lcd_pc.c"
#include "../libs/xbee.c"
#include "../libs/kbhit.c"
#include "../libs/hex2a.c"
#include "../libs/compress_csv.c"

float ct_watts		=-1;
float ct_temp		=-1;
float ct_lux		=-1;
float aircon_watts	=-1;
float aircon_temp	=-1;
float aircon_lux	=-1;
float sens_press	=-1;
float sens_temp		=-1;

#define BER_GH_NUM	64	// �_�O���t�̕\����

void html(){
	FILE *fp_html;
	FILE *fp_ct;
	float graph[BER_GH_NUM];
	int bar;
	byte i,j;
	int hh[BER_GH_NUM],mm[BER_GH_NUM];
	int y,m,d;
	float dummy;
	
	if( (fp_html = fopen("htdocs/index.html", "w")) != NULL ){
		fprintf(fp_html, "<head><title>XBee Sensors</title>\r\n");
		fprintf(fp_html, "<meta charset=\"Shift_JIS\" content=10>\r\n");
		fprintf(fp_html, "<meta http-equiv=\"refresh\" content=10></head>\r\n");
		fprintf(fp_html, "<body>\r\n");
		fprintf(fp_html, "<h3>XBee Sensors</h3>\r\n<table border=1>\r\n");
		fprintf(fp_html, "<tr><td rowspan=2>���O�Z���T</td>\r\n");
		fprintf(fp_html, "<td>�C��</td>\r\n<td align=right>%2.1f [��]</td>\r\n",sens_temp);
		fprintf(fp_html, "<tr><td>�C��</td>\r\n<td align=right>%4.2f [hPa]</td>\r\n",sens_press);
		fprintf(fp_html, "<tr><td rowspan=4><a href=\"ct.csv.html\">�d�̓��[�^</a></td>\r\n");
		fprintf(fp_html, "<td rowspan=2>�d��</td>\r\n<td align=right>%4.0f [W]\r\n",ct_watts);
		if( ct_watts > 1000 ) bar = 100; else bar = (int)(ct_watts / 10 + 1);
		fprintf(fp_html, "<tr><td><img src=\"graph.gif\" height=12 width=%d><br><img src=\"graph_bg.gif\"></td>",bar);
		fprintf(fp_html, "<tr><td>�C��</td>\r\n<td align=right>%2.1f [��]</td>\r\n",ct_temp);
		fprintf(fp_html, "<tr><td>�Ɠx</td>\r\n<td align=right>%4.0f [lux]</td>\r\n",ct_lux);
		fprintf(fp_html, "<tr><td rowspan=4><a href=\"plug.csv.html\">�G�A�R��</a></td>\r\n");
		fprintf(fp_html, "<td rowspan=2>�d��</td>\r\n<td align=right>%4.0f [W]",aircon_watts);
		if( aircon_watts > 1000 ) bar = 100; else bar = (int)(aircon_watts / 10 + 1);
		fprintf(fp_html, "<tr><td><img src=\"graph.gif\" height=12 width=%d><br><img src=\"graph_bg.gif\"></td>\r\n",bar);
		fprintf(fp_html, "<tr><td>�C��</td>\r\n<td align=right>%2.1f [��]</td>\r\n",aircon_temp);
		fprintf(fp_html, "<tr><td>�Ɠx</td>\r\n<td align=right>%4.0f [lux]</td>\r\n",aircon_lux);
		fprintf(fp_html, "</table><br>\r\n");
		fprintf(fp_html, "<a href=\"graph_ct.html\">�d�̓��[�^�O���t</a> <a href=\"graph_aircon.html\">�G�A�R���O���t</a><br>\r\n");
		fprintf(fp_html, "</body></html>\r\n");
		fclose(fp_html);
	}
	if( (fp_html = fopen("htdocs/graph_ct.html", "w")) != NULL ){
		if( (fp_ct = fopen("ct.csv", "r")) != NULL ){
			fprintf(fp_html, "<head><title>XBee Sensors - Graph</title>\r\n");
			fprintf(fp_html, "<meta http-equiv=\"refresh\" content=10></head>\r\n");
			fprintf(fp_html, "</head>\r\n");
			fprintf(fp_html, "<body>\r\n");
			fprintf(fp_html, "<h3>�d�̓��[�^�O���t</h3>\r\n");

			i = BER_GH_NUM - 1;
			while( feof(fp_ct) == 0 ){
				i++;
				if(i > BER_GH_NUM - 1) i=0;
				fscanf(fp_ct,"%d/%d/%d,%d:%d, %f, %f, %f\n",&y,&m,&d,&hh[i],&mm[i],&dummy,&dummy,&graph[i]);
			}
			fprintf(fp_html, "<div align=right>%d/%d/%d<br><a href=\"index.html\">�߂�</a></div><br>\r\n",y,m,d);
			fprintf(fp_html, "<table border=0>\r\n");
			fprintf(fp_html, "<tr><td align=right><font size=1>0[W]</font></td><td valign=bottom><img src=\"graph_bg.gif\" height=2 width=101></td><td align=left><font size=1>1000[W]</font></td>\r\n");
			for( j = i ; j != 0xFF ; j-- ){
				if( graph[j] > 1000 ) bar = 101; else bar = (int)( graph[j] / 10 + 1);
				fprintf(fp_html, "<tr><td><font size=2>%02d:%02d</font></td><td><img src=\"graph.gif\" height=12 width=%d><br><img src=\"graph_bg.gif\" height=2 width=101></td><td align=right><font size=2>%4.0f[W]</font></td>\r\n",hh[j],mm[j],bar,graph[j] );
			}
			if( i != BER_GH_NUM - 1 ){
				for( j = BER_GH_NUM - 1 ; j > i ; j-- ){
					if( graph[j] > 1000 ) bar = 101; else bar = (int)( graph[j] / 10 + 1);
					fprintf(fp_html, "<tr><td><font size=2>%02d:%02d</font></td><td><img src=\"graph.gif\" height=12 width=%d><br><img src=\"graph_bg.gif\" height=2 width=101></td><td align=right><font size=2>%4.0f[W]</font></td>\r\n",hh[j],mm[j],bar,graph[j] );
				}
			}
			fclose(fp_ct);
			fprintf(fp_html, "</table><br>\r\n");
			fprintf(fp_html, "<a href=\"index.html\">�߂�</a><br>\r\n");
			fprintf(fp_html, "</body></html>\r\n");
		}
		fclose(fp_html);
	}
	if( (fp_html = fopen("htdocs/graph_aircon.html", "w")) != NULL ){
		if( (fp_ct = fopen("plug.csv", "r")) != NULL ){
			fprintf(fp_html, "<head><title>XBee Sensors - Graph</title>\r\n");
			fprintf(fp_html, "<meta http-equiv=\"refresh\" content=10></head>\r\n");
			fprintf(fp_html, "</head>\r\n");
			fprintf(fp_html, "<body>\r\n");
			fprintf(fp_html, "<h3>�G�A�R���O���t</h3>\r\n");

			i = BER_GH_NUM - 1;
			while( feof(fp_ct) == 0 ){
				i++;
				if(i > BER_GH_NUM - 1) i=0;
				fscanf(fp_ct,"%d/%d/%d,%d:%d, %f, %f, %f\n",&y,&m,&d,&hh[i],&mm[i],&dummy,&dummy,&graph[i]);
			}
			fprintf(fp_html, "<div align=right>%d/%d/%d<br><a href=\"index.html\">�߂�</a></div><br>\r\n",y,m,d);
			fprintf(fp_html, "<table border=0>\r\n");
			fprintf(fp_html, "<tr><td align=right><font size=1>0[W]</font></td><td valign=bottom><img src=\"graph_bg.gif\" height=2 width=101></td><td align=left><font size=1>1000[W]</font></td>\r\n");
			for( j = i ; j != 0xFF ; j-- ){
				if( graph[j] > 1000 ) bar = 101; else bar = (int)( graph[j] / 10 + 1);
				fprintf(fp_html, "<tr><td><font size=2>%02d:%02d</font></td><td><img src=\"graph.gif\" height=12 width=%d><br><img src=\"graph_bg.gif\" height=2 width=101></td><td align=right><font size=2>%4.0f[W]</font></td>\r\n",hh[j],mm[j],bar,graph[j] );
			}
			if( i != BER_GH_NUM - 1 ){
				for( j = BER_GH_NUM - 1 ; j > i ; j-- ){
					if( graph[j] > 1000 ) bar = 101; else bar = (int)( graph[j] / 10 + 1);
					fprintf(fp_html, "<tr><td><font size=2>%02d:%02d</font></td><td><img src=\"graph.gif\" height=12 width=%d><br><img src=\"graph_bg.gif\" height=2 width=101></td><td align=right><font size=2>%4.0f[W]</font></td>\r\n",hh[j],mm[j],bar,graph[j] );
				}
			}
			fclose(fp_ct);
			fprintf(fp_html, "</table><br>\r\n");
			fprintf(fp_html, "<a href=\"index.html\">�߂�</a><br>\r\n");
			fprintf(fp_html, "</body></html>\r\n");
		}
		fclose(fp_html);
	}
}


int main(int argc,char **argv){
	FILE *fp;
	char filename[] = "01234567.csv";

	time_t timer;
	struct tm *time_st;

	char today_s[]     = "2000/01/01";
	char today_s_b[]   = "2000/01/01";
	
	byte i,j;
	byte data;
	byte trig=0;
	byte dev_1_en = 0;	// �Z���T�[�����̗L��(0:������)
	byte dev_2_en = 0;
	byte dev_new_en = 0;
	byte dev_tmp_en = 0;
	XBEE_RESULT xbee_result;
	unsigned int value_i;

	byte port=0;		// �����莝����XBee���W���[����IEEE�A�h���X�ɕύX����
	byte dev_1[]   = {0x00,0x13,0xA2,0x00,0x40,0x11,0x22,0x33};	// Smart Plug
	byte dev_2[]   = {0x00,0x13,0xA2,0x00,0x40,0x11,0x22,0x44};	// CT�Z���T
	byte dev_3[]   = {0x00,0x13,0xA2,0x00,0x40,0x11,0x22,0x55};	// press�Z���T
	byte dev_new[] = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// �V�f�o�C�X
	byte dev_tmp[] = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// �V���A����M�p
	
	// ����������
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// COM�|�[�g������(�����̓|�[�g�ԍ�)
	xbee_atnj( 0 );				// �W���C���֎~
	
	wait_millisec( 1000 );
	
	dev_1_en = 1;
	dev_2_en = 1;
	xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
	xbee_gpio_config( dev_1, 1 , AIN ); // �ڑ�����̃|�[�g1��AIN���͂�
	xbee_gpio_config( dev_1, 2 , AIN ); // �ڑ�����̃|�[�g2��AIN���͂�
	xbee_gpio_config( dev_1, 3 , AIN ); // �ڑ�����̃|�[�g3��AIN���͂�
	xbee_gpio_config( dev_2, 1 , AIN ); // �ڑ�����̃|�[�g1��AIN���͂�
	xbee_gpio_config( dev_2, 2 , AIN ); // �ڑ�����̃|�[�g2��AIN���͂�
	xbee_gpio_config( dev_2, 3 , AIN ); // �ڑ�����̃|�[�g3��AIN���͂�
	/*
	// �f�o�C�X�T��
	lcd_disp("Searching:Smart Plug");
	if( xbee_ping( dev_1 ) != DEV_TYPE_PLUG  ){	// DEV_TYPE_PLUG 0x0F (Smart Plug)
		if( xbee_atnj(10) ){				// 10�b�Ԃ̎Q���󂯓���
			lcd_disp("Found device");
			xbee_from( dev_1 );			// ��M�����A�h���X��dev_1�֊i�[
			dev_1_en = 1;
		}else{
			lcd_disp("Failed:no dev.");
			// exit(-1);
		}
	} else dev_1_en = 1;
	if( dev_1_en ){
		xbee_ratnj( dev_1, 0 );		// �W���C���֎~
		xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
	}
	
	lcd_disp("Searching:CT Sensor");
	if( xbee_ping( dev_2 ) != DEV_TYPE_XBEE  ){	// DEV_TYPE_PLUG 0x0F (Smart Plug)
		if( xbee_atnj(30) ){				// 30�b�Ԃ̎Q���󂯓���
			lcd_disp("Found device");
			xbee_from( dev_1 );			// ��M�����A�h���X��dev_1�֊i�[
			dev_2_en = 1;
		}else{
			lcd_disp("Failed:no dev.");
			// exit(-1);
		}
	} else dev_2_en = 1;
	*/
	
	wait_millisec( 1000 );
	
	// ���C������
	lcd_disp("Receiving");
	while(1){							// �i�v�Ɏ�M����
		/* �L�[�{�[�h����̓��� */
		if ( kbhit() ) data = (byte)getchar(); else data = 0x00;
		switch( data ){
			case 'n':
				lcd_disp("atnj");
				if( xbee_atnj(10) ){
					lcd_disp("Found device");
					xbee_from( dev_new );
				}
				lcd_disp("done");
				break;
			case 'b':
				lcd_disp("batt");
				if( dev_1_en ) printf("dev_1  :%d[mV]\n", xbee_batt(dev_1) );
				if( dev_2_en ) printf("dev_2  :%d[mV]\n", xbee_batt(dev_2) );
				if( dev_new_en ) printf("dev_new:%d[mV]\n", xbee_batt(dev_new) );
				if( dev_tmp_en ) printf("dev_tmp:%d[mV]\n", xbee_batt(dev_tmp) );
				lcd_disp("done");
				break;
			case 'd':
				lcd_disp("devices");
				if( dev_1_en ){	lcd_goto(LCD_ROW_1); lcd_putstr("dev 1  :");
								for(i=4; i<8 ; i++) lcd_disp_hex(dev_1[i]);}
				if( dev_2_en ){	lcd_goto(LCD_ROW_2); lcd_putstr("dev 2  :");
								for(i=4; i<8 ; i++) lcd_disp_hex(dev_2[i]);}
				if( dev_new_en ){lcd_goto(LCD_ROW_3); lcd_putstr("dev new:");
								for(i=4; i<8 ; i++) lcd_disp_hex(dev_new[i]);}
				if( dev_tmp_en ){lcd_goto(LCD_ROW_4); lcd_putstr("dev tmp:");
								for(i=4; i<8 ; i++) lcd_disp_hex(dev_tmp[i]);}
				wait_millisec(1000);
				lcd_cls();
				lcd_disp("done");
				break;
			default:
				break;
		}

		/* �^�C�}�[�ɂ�������� */
		time(&timer);
		time_st = localtime(&timer);
		if( time_st->tm_min != trig ){	// �u���v�ɕω�����������
			xbee_force( dev_1 );		// �f�o�C�Xdev_1�փf�[�^�v��
			trig = time_st->tm_min;
			/* ���t�̎擾 */
			sprintf(today_s, "%4d/%02d/%02d",
				time_st->tm_year+1900,
				time_st->tm_mon+1,
				time_st->tm_mday
			);
			/* �f�[�^���k(1�����Ƃ̃f�[�^��1���Ԃ��Ƃɕϊ���csv��html�ŏo��) */
			if( strncmp(today_s,today_s_b,sizeof(today_s)) != 0 ){
				printf("compress %s-%s\n",today_s_b,today_s);
				compress_data( today_s_b , "plug.csv",		LIGHT,	TEMP,	WATT);	// Smart Plug
				compress_data( today_s_b , "ct.csv",		LIGHT,	TEMP,	WATT);	// CT�Z���T
				sprintf(today_s_b,"%s",today_s);
			}
		}
		
		/* �f�[�^��M(�҂��󂯂Ď�M����) */
		xbee_result.MODE = MODE_AUTO;	// ������M���[�h�ɐݒ�
		data = xbee_rx_call( &xbee_result );	// �f�[�^����M���܂��B(������50ms������܂�)
										// data�F��M���ʂ̑�\�l
										// xbee_result�F�ڍׂȎ�M����
										// xbee_result.MODE�F��M���[�h
		switch( xbee_result.MODE ){		// ��M�����f�[�^�̓��e(MODE�l)�ɉ�����
			case MODE_RESP:				// sensor����̉����̏ꍇ�ɏƓx�Ɖ��x��\��
				/* ����Smart Plug����̃f�[�^����M */
				if( xbee_result.AT[0]=='I' &&
					xbee_result.AT[1]=='S' &&
					bytecmp( dev_1 , xbee_result.FROM ,8 ) == 0 &&
					xbee_result.STATUS == STATUS_OK
				){
					lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("recieved from a Smart Plug");
					// �Ɠx���茋��
					aircon_lux = xbee_sensor_result( &xbee_result,LIGHT);
					lcd_goto(LCD_ROW_2); lcd_disp_5( (unsigned int)aircon_lux ); lcd_putstr(" Lux");
					/* �Â��Ƃ��͓d����؂�A���邢�Ƃ��͓d��������
					if( value < 1.0 ){
						xbee_gpo(dev_1 , 4 , 0 );	// Turns outlet off (Port 4 = 0)
					}else{
						xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
					}
					*/
					// ���x���茋��(����덷7.12�x��␳���Ă��܂�)
					aircon_temp = xbee_sensor_result( &xbee_result,TEMP) - 7.12;
					
					i = (byte)aircon_temp; lcd_goto(LCD_ROW_3); lcd_disp_2( i ); lcd_putstr(".");
					i = (byte)((aircon_temp-(float)i)*100); lcd_disp_2( i ); lcd_putstr(" degC");
					// �d�͑��茋��
					aircon_watts = xbee_sensor_result( &xbee_result,WATT);
					if( aircon_watts < 0 ) aircon_watts = 0;
					value_i = (unsigned int)aircon_watts;
					lcd_goto(LCD_ROW_4); lcd_disp_3( value_i ); lcd_putstr(" Watts"); lcd_putch('\n');
					if( (fp = fopen("plug.csv", "a")) != NULL ) {
						fprintf(fp, "%d/%d/%d,%d:%d, ",
							time_st->tm_year+1900,
							time_st->tm_mon+1,
							time_st->tm_mday,
							time_st->tm_hour,
							time_st->tm_min
						);
						fprintf(fp, "%f, %f, %f\n",aircon_lux,aircon_temp,aircon_watts);
						fclose(fp);
					}else lcd_disp("Failed fopen plug.csv");
					html();
				}else lcd_disp("recieved RAT_RESP");
				break;
			case MODE_GPIN:
				/* ����Smart Plug����̃f�[�^����M*/
				if( bytecmp( dev_2 , xbee_result.FROM ,8 ) == 0 ){
					lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("recieved from a CT Sensor");
				/*	for(i=0;i<8;i++){
						printf("data[%d]=%x \n",i+2,xbee_result.DATA[i+2]);
					}
					for(i=0;i<4;i++){
						printf("ADC[%d]=%d \n",i,(int)(xbee_result.ADC[i]));
					}*/
					// �Ɠx���茋��
					ct_lux = (float)(xbee_result.ADCIN[1]) * 1.17302 / 3.3;
					lcd_goto(LCD_ROW_2); lcd_disp_5( (unsigned int)ct_lux ); lcd_putstr(" Lux");
					// ���x���茋��
					ct_temp = (float)xbee_result.ADCIN[2] * 1.17302 / 10. - 60.;
					value_i = (byte)ct_temp; lcd_goto(LCD_ROW_3); lcd_disp_2( value_i ); lcd_putstr(".");
					value_i = (byte)((ct_temp-(float)value_i)*100); lcd_disp_2( value_i ); lcd_putstr(" degC");
					// �d�͑��茋��
					ct_watts = (float)xbee_result.ADCIN[3] * CTSENSER_MLTPL;
					if( ct_watts < 1.0 ) value_i = 0;
					else value_i = (unsigned int)ct_watts;
					lcd_goto(LCD_ROW_4); lcd_disp_3( value_i ); lcd_putstr(" Watts"); lcd_putch('\n');
					if( (fp = fopen("ct.csv", "a")) != NULL ) {
						fprintf(fp, "%d/%d/%d,%d:%d, ",
							time_st->tm_year+1900,
							time_st->tm_mon+1,
							time_st->tm_mday,
							time_st->tm_hour,
							time_st->tm_min
						);
						fprintf(fp, "%f, %f, %f\n",ct_lux,ct_temp,ct_watts);
						fclose(fp);
					}else lcd_disp("Failed fopen ct.csv");
					html();
				}
				break;
			case MODE_UART:
				/* �V���A���f�[�^����M */
				xbee_from( dev_tmp );	// �A�h���X��dev_tmp�ɓǂݍ���
				dev_tmp_en = 1;			// �t���O
				lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("data[");
				lcd_disp_hex(data); lcd_putch(']');		// �f�[�^����\��
				for(i=0;i<data;i++)lcd_putch( xbee_result.DATA[i] );lcd_putch( '\n' );
				// printf("dev_tmp:%d[mV]\n", xbee_batt(dev_tmp) );
				for(i=0;i<4;i++) hex2a( dev_tmp[i+4], &(filename[i*2]) );
				if( (fp = fopen( filename, "a")) != NULL ) {
					fprintf(fp, "%d/%d/%d,%d:%d, ",
						time_st->tm_year+1900,
						time_st->tm_mon+1,
						time_st->tm_mday,
						time_st->tm_hour,
						time_st->tm_min
					);
					for(i=0;i<data;i++){
						if( xbee_result.DATA[i] >=(byte)' ' && xbee_result.DATA[i] <= 126)
						fputc( xbee_result.DATA[i], fp);	// �����̏ꍇ�ɏ����o��
					}
					fputc( '\n', fp);
					fclose(fp);
				}else lcd_disp("Failed fopen");
				if( bytecmp( dev_3 , xbee_result.FROM ,8 ) == 0 ){
					j=0; sens_press=0.0; sens_temp=0.0;
					for(i=0;i<data;i++){
						if( (char)xbee_result.DATA[i] != ' ' &&
							(char)xbee_result.DATA[i] != '\n' &&
							(char)xbee_result.DATA[i] != '\r' ){
							if( (char)xbee_result.DATA[i] == ',' ){
								j++;
							} else {
								if( j == 0 ){
									if( (char)xbee_result.DATA[i]=='.' ){
										i++;
										sens_temp += (float)((xbee_result.DATA[i]-(byte)'0')/10);
									}else{
										sens_temp *= 10.0;
										sens_temp += (float)(xbee_result.DATA[i]-(byte)'0');
									}
								}
								if( j == 1 ){
									sens_press *= 10.0;
									sens_press += (float)(xbee_result.DATA[i]-(byte)'0');
								}
							}
						}
					}
					sens_press /=100;
					html();
				}
				break;
			case MODE_IDNT:				// �V�����f�o�C�X�𔭌�
				xbee_from( dev_tmp );	// ���������A�h���X��dev_tmp�ɓǂݍ���
				if( bytecmp( dev_1 , dev_tmp ,8 ) == 0){
					dev_1_en = 1;
					lcd_disp("found Smart Plug for dev_1");
				}else if( bytecmp( dev_2 , dev_tmp ,8 ) == 0){
					dev_2_en = 1;
					lcd_disp("found CT Sensor for dev_2");
				}else if ( xbee_ping( dev_tmp ) == DEV_TYPE_PLUG ) {
					for(i=0;i<8;i++) dev_1[i]=dev_tmp[i];
					dev_1_en = 1;
					lcd_disp("REDISTED NEW Smart Plug for dev_1");
				}else{
					dev_new_en = 1;
					for(i=0;i<8;i++) dev_new[i]=dev_tmp[i];
					lcd_disp("REDISTED NEW device for dev_new");
				}
				lcd_goto(LCD_ROW_1); lcd_putstr("dev    :");
				for(i=4; i<8 ; i++) lcd_disp_hex( dev_tmp[i] );
				break;
			default:
				break;
		}
	}
}
