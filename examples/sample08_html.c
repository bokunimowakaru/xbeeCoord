/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

※注意：パソコンやファンヒータなどAC電源の途絶えると故障や事故の発生する
　機器をSmart Plugに接続しないこと。
　
※このソースはPC用です。■■■H8では動作しません■■■

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
//#define ERRLOG			//ERRLOGをdefineするとエラーログが出力されます。

#define CTSENSER_MLTPL 1.17302 / 0.3536		// 負荷抵抗100Ω 最大3kW
/*
#define CTSENSER_MLTPL 1.17302 / 3.536		// 負荷抵抗1kΩ 最大300W
#define CTSENSER_MLTPL 1.17302 / 1.167		// 負荷抵抗330Ω 最大900W
#define CTSENSER_MLTPL 1.17302 / 0.3536		// 負荷抵抗100Ω 最大3kW
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

#define BER_GH_NUM	64	// 棒グラフの表示数

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
		fprintf(fp_html, "<tr><td rowspan=2>屋外センサ</td>\r\n");
		fprintf(fp_html, "<td>気温</td>\r\n<td align=right>%2.1f [℃]</td>\r\n",sens_temp);
		fprintf(fp_html, "<tr><td>気圧</td>\r\n<td align=right>%4.2f [hPa]</td>\r\n",sens_press);
		fprintf(fp_html, "<tr><td rowspan=4><a href=\"ct.csv.html\">電力メータ</a></td>\r\n");
		fprintf(fp_html, "<td rowspan=2>電力</td>\r\n<td align=right>%4.0f [W]\r\n",ct_watts);
		if( ct_watts > 1000 ) bar = 100; else bar = (int)(ct_watts / 10 + 1);
		fprintf(fp_html, "<tr><td><img src=\"graph.gif\" height=12 width=%d><br><img src=\"graph_bg.gif\"></td>",bar);
		fprintf(fp_html, "<tr><td>気温</td>\r\n<td align=right>%2.1f [℃]</td>\r\n",ct_temp);
		fprintf(fp_html, "<tr><td>照度</td>\r\n<td align=right>%4.0f [lux]</td>\r\n",ct_lux);
		fprintf(fp_html, "<tr><td rowspan=4><a href=\"plug.csv.html\">エアコン</a></td>\r\n");
		fprintf(fp_html, "<td rowspan=2>電力</td>\r\n<td align=right>%4.0f [W]",aircon_watts);
		if( aircon_watts > 1000 ) bar = 100; else bar = (int)(aircon_watts / 10 + 1);
		fprintf(fp_html, "<tr><td><img src=\"graph.gif\" height=12 width=%d><br><img src=\"graph_bg.gif\"></td>\r\n",bar);
		fprintf(fp_html, "<tr><td>気温</td>\r\n<td align=right>%2.1f [℃]</td>\r\n",aircon_temp);
		fprintf(fp_html, "<tr><td>照度</td>\r\n<td align=right>%4.0f [lux]</td>\r\n",aircon_lux);
		fprintf(fp_html, "</table><br>\r\n");
		fprintf(fp_html, "<a href=\"graph_ct.html\">電力メータグラフ</a> <a href=\"graph_aircon.html\">エアコングラフ</a><br>\r\n");
		fprintf(fp_html, "</body></html>\r\n");
		fclose(fp_html);
	}
	if( (fp_html = fopen("htdocs/graph_ct.html", "w")) != NULL ){
		if( (fp_ct = fopen("ct.csv", "r")) != NULL ){
			fprintf(fp_html, "<head><title>XBee Sensors - Graph</title>\r\n");
			fprintf(fp_html, "<meta http-equiv=\"refresh\" content=10></head>\r\n");
			fprintf(fp_html, "</head>\r\n");
			fprintf(fp_html, "<body>\r\n");
			fprintf(fp_html, "<h3>電力メータグラフ</h3>\r\n");

			i = BER_GH_NUM - 1;
			while( feof(fp_ct) == 0 ){
				i++;
				if(i > BER_GH_NUM - 1) i=0;
				fscanf(fp_ct,"%d/%d/%d,%d:%d, %f, %f, %f\n",&y,&m,&d,&hh[i],&mm[i],&dummy,&dummy,&graph[i]);
			}
			fprintf(fp_html, "<div align=right>%d/%d/%d<br><a href=\"index.html\">戻る</a></div><br>\r\n",y,m,d);
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
			fprintf(fp_html, "<a href=\"index.html\">戻る</a><br>\r\n");
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
			fprintf(fp_html, "<h3>エアコングラフ</h3>\r\n");

			i = BER_GH_NUM - 1;
			while( feof(fp_ct) == 0 ){
				i++;
				if(i > BER_GH_NUM - 1) i=0;
				fscanf(fp_ct,"%d/%d/%d,%d:%d, %f, %f, %f\n",&y,&m,&d,&hh[i],&mm[i],&dummy,&dummy,&graph[i]);
			}
			fprintf(fp_html, "<div align=right>%d/%d/%d<br><a href=\"index.html\">戻る</a></div><br>\r\n",y,m,d);
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
			fprintf(fp_html, "<a href=\"index.html\">戻る</a><br>\r\n");
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
	byte dev_1_en = 0;	// センサー発見の有無(0:未発見)
	byte dev_2_en = 0;
	byte dev_new_en = 0;
	byte dev_tmp_en = 0;
	XBEE_RESULT xbee_result;
	unsigned int value_i;

	byte port=0;		// ↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev_1[]   = {0x00,0x13,0xA2,0x00,0x40,0x11,0x22,0x33};	// Smart Plug
	byte dev_2[]   = {0x00,0x13,0xA2,0x00,0x40,0x11,0x22,0x44};	// CTセンサ
	byte dev_3[]   = {0x00,0x13,0xA2,0x00,0x40,0x11,0x22,0x55};	// pressセンサ
	byte dev_new[] = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// 新デバイス
	byte dev_tmp[] = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// シリアル受信用
	
	// 初期化処理
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	xbee_atnj( 0 );				// ジョイン禁止
	
	wait_millisec( 1000 );
	
	dev_1_en = 1;
	dev_2_en = 1;
	xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
	xbee_gpio_config( dev_1, 1 , AIN ); // 接続相手のポート1をAIN入力へ
	xbee_gpio_config( dev_1, 2 , AIN ); // 接続相手のポート2をAIN入力へ
	xbee_gpio_config( dev_1, 3 , AIN ); // 接続相手のポート3をAIN入力へ
	xbee_gpio_config( dev_2, 1 , AIN ); // 接続相手のポート1をAIN入力へ
	xbee_gpio_config( dev_2, 2 , AIN ); // 接続相手のポート2をAIN入力へ
	xbee_gpio_config( dev_2, 3 , AIN ); // 接続相手のポート3をAIN入力へ
	/*
	// デバイス探索
	lcd_disp("Searching:Smart Plug");
	if( xbee_ping( dev_1 ) != DEV_TYPE_PLUG  ){	// DEV_TYPE_PLUG 0x0F (Smart Plug)
		if( xbee_atnj(10) ){				// 10秒間の参加受け入れ
			lcd_disp("Found device");
			xbee_from( dev_1 );			// 受信したアドレスをdev_1へ格納
			dev_1_en = 1;
		}else{
			lcd_disp("Failed:no dev.");
			// exit(-1);
		}
	} else dev_1_en = 1;
	if( dev_1_en ){
		xbee_ratnj( dev_1, 0 );		// ジョイン禁止
		xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
	}
	
	lcd_disp("Searching:CT Sensor");
	if( xbee_ping( dev_2 ) != DEV_TYPE_XBEE  ){	// DEV_TYPE_PLUG 0x0F (Smart Plug)
		if( xbee_atnj(30) ){				// 30秒間の参加受け入れ
			lcd_disp("Found device");
			xbee_from( dev_1 );			// 受信したアドレスをdev_1へ格納
			dev_2_en = 1;
		}else{
			lcd_disp("Failed:no dev.");
			// exit(-1);
		}
	} else dev_2_en = 1;
	*/
	
	wait_millisec( 1000 );
	
	// メイン処理
	lcd_disp("Receiving");
	while(1){							// 永久に受信する
		/* キーボードからの入力 */
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

		/* タイマーによる定期処理 */
		time(&timer);
		time_st = localtime(&timer);
		if( time_st->tm_min != trig ){	// 「分」に変化があった時
			xbee_force( dev_1 );		// デバイスdev_1へデータ要求
			trig = time_st->tm_min;
			/* 日付の取得 */
			sprintf(today_s, "%4d/%02d/%02d",
				time_st->tm_year+1900,
				time_st->tm_mon+1,
				time_st->tm_mday
			);
			/* データ圧縮(1分ごとのデータを1時間ごとに変換しcsvとhtmlで出力) */
			if( strncmp(today_s,today_s_b,sizeof(today_s)) != 0 ){
				printf("compress %s-%s\n",today_s_b,today_s);
				compress_data( today_s_b , "plug.csv",		LIGHT,	TEMP,	WATT);	// Smart Plug
				compress_data( today_s_b , "ct.csv",		LIGHT,	TEMP,	WATT);	// CTセンサ
				sprintf(today_s_b,"%s",today_s);
			}
		}
		
		/* データ受信(待ち受けて受信する) */
		xbee_result.MODE = MODE_AUTO;	// 自動受信モードに設定
		data = xbee_rx_call( &xbee_result );	// データを受信します。(処理に50msかかります)
										// data：受信結果の代表値
										// xbee_result：詳細な受信結果
										// xbee_result.MODE：受信モード
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_RESP:				// sensorからの応答の場合に照度と温度を表示
				/* 純正Smart Plugからのデータを受信 */
				if( xbee_result.AT[0]=='I' &&
					xbee_result.AT[1]=='S' &&
					bytecmp( dev_1 , xbee_result.FROM ,8 ) == 0 &&
					xbee_result.STATUS == STATUS_OK
				){
					lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("recieved from a Smart Plug");
					// 照度測定結果
					aircon_lux = xbee_sensor_result( &xbee_result,LIGHT);
					lcd_goto(LCD_ROW_2); lcd_disp_5( (unsigned int)aircon_lux ); lcd_putstr(" Lux");
					/* 暗いときは電源を切り、明るいときは電源を入れる
					if( value < 1.0 ){
						xbee_gpo(dev_1 , 4 , 0 );	// Turns outlet off (Port 4 = 0)
					}else{
						xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
					}
					*/
					// 温度測定結果(測定誤差7.12度を補正しています)
					aircon_temp = xbee_sensor_result( &xbee_result,TEMP) - 7.12;
					
					i = (byte)aircon_temp; lcd_goto(LCD_ROW_3); lcd_disp_2( i ); lcd_putstr(".");
					i = (byte)((aircon_temp-(float)i)*100); lcd_disp_2( i ); lcd_putstr(" degC");
					// 電力測定結果
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
				/* 自作Smart Plugからのデータを受信*/
				if( bytecmp( dev_2 , xbee_result.FROM ,8 ) == 0 ){
					lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("recieved from a CT Sensor");
				/*	for(i=0;i<8;i++){
						printf("data[%d]=%x \n",i+2,xbee_result.DATA[i+2]);
					}
					for(i=0;i<4;i++){
						printf("ADC[%d]=%d \n",i,(int)(xbee_result.ADC[i]));
					}*/
					// 照度測定結果
					ct_lux = (float)(xbee_result.ADCIN[1]) * 1.17302 / 3.3;
					lcd_goto(LCD_ROW_2); lcd_disp_5( (unsigned int)ct_lux ); lcd_putstr(" Lux");
					// 温度測定結果
					ct_temp = (float)xbee_result.ADCIN[2] * 1.17302 / 10. - 60.;
					value_i = (byte)ct_temp; lcd_goto(LCD_ROW_3); lcd_disp_2( value_i ); lcd_putstr(".");
					value_i = (byte)((ct_temp-(float)value_i)*100); lcd_disp_2( value_i ); lcd_putstr(" degC");
					// 電力測定結果
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
				/* シリアルデータを受信 */
				xbee_from( dev_tmp );	// アドレスをdev_tmpに読み込み
				dev_tmp_en = 1;			// フラグ
				lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("data[");
				lcd_disp_hex(data); lcd_putch(']');		// データ長を表示
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
						fputc( xbee_result.DATA[i], fp);	// 文字の場合に書き出し
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
			case MODE_IDNT:				// 新しいデバイスを発見
				xbee_from( dev_tmp );	// 発見したアドレスをdev_tmpに読み込み
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
