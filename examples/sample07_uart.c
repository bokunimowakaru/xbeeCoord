/*********************************************************************
複数のXBeeからシリアルで受信した内容をファイルへ保存する。(アドレス.csv)
Smart Plug で測定した結果もファイルへ保存する。(smartplug.csv)

※このソースはPC用です。■■■H8やArduinoでは動作しません。■■■

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
//#define ERRLOG			//ERRLOGをdefineするとエラーログが出力されます。

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
	byte dev_1_en = 0;		// センサー発見の有無(0:未発見)
	byte dev_tmp_en = 0;	// センサー発見の有無(0:未発見)
	XBEE_RESULT xbee_result;
	float value_lux;
	float value_temp;
	float value_watts;
	unsigned int value_i;

	byte port=0;		// ↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev_1[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// Smart Plug
	byte dev_tmp[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	// シリアル受信用(入力不要)
	
	// 初期化処理
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	xbee_atnj( 0 );				// ジョイン禁止
		
	wait_millisec( 1000 );
	
	// デバイス探索
	lcd_disp("Searching:Smart Plug");
	if( xbee_ping( dev_1 ) != DEV_TYPE_PLUG ){	// DEV_TYPE_PLUG 0x0F (Smart Plug)
		if( xbee_atnj(10) ){				// 10秒間の参加受け入れ
			lcd_disp("Found device");
			xbee_from( dev_1 );			// 受信したアドレスをdev_1へ格納
			dev_1_en = 1;
		}else{
			lcd_disp("Failed:no dev.");
		}
	}
	if( dev_1_en ){
		xbee_ratnj( dev_1, 0 );		// ジョイン禁止
		xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
	}
	
	wait_millisec( 1000 );
	
	// メイン処理
	lcd_disp("Receiving");
	while(1){							// 永久に受信する
		/* キーボードからの入力 */
		if ( kbhit() ) data = (byte)getchar(); else data = 0x00;
		switch( data ){
			case 'n':	// 10秒間、ネットワーク参加を許可する
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
		
		/* タイマーによる定期処理 */
		time(&timer);
		time_st = localtime(&timer);
		if( time_st->tm_min != trig && dev_1_en ){	// 「分」に変化があった時
			xbee_force( dev_1 );		// デバイスdev_1へデータ要求
			trig = time_st->tm_min;
		}
		
		/* データ受信(待ち受けて受信する) */
		xbee_result.MODE = MODE_AUTO;	// 自動受信モードに設定
		data = xbee_rx_call( &xbee_result );	// データを受信します。(処理に50msかかります)
										// data：受信結果の代表値
										// xbee_result：詳細な受信結果
										// xbee_result.MODE：受信モード
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_RESP:				// sensorからの応答の場合に照度と温度を表示
				/* Smart Plugからのデータを受信 */
				if( xbee_result.AT[0]=='I' &&
					xbee_result.AT[1]=='S' &&
					bytecmp( dev_1 , xbee_result.FROM ,8 ) == 0 &&
					xbee_result.STATUS == STATUS_OK
				){
					// 照度測定結果
					value_lux = xbee_sensor_result( &xbee_result,LIGHT);	
					lcd_cls(); lcd_goto(LCD_ROW_1); lcd_disp_5( (unsigned int)value_lux ); lcd_putstr(" Lux");
					/* 暗いときは電源を切り、明るいときは電源を入れる
					if( value < 1.0 ){
						xbee_gpo(dev_1 , 4 , 0 );	// Turns outlet off (Port 4 = 0)
					}else{
						xbee_gpo(dev_1 , 4 , 1 );	// Turns outlet on (Port 4 = 1)
					}
					*/
					// 温度測定結果
					value_temp = xbee_sensor_result( &xbee_result,TEMP);
					i = (byte)value_temp; lcd_goto(LCD_ROW_2); lcd_disp_2( i ); lcd_putstr(".");
					i = (byte)((value_temp-(float)i)*100); lcd_disp_2( i ); lcd_putstr(" degC");
					// 電力測定結果
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
				/* シリアルデータを受信 */
				xbee_from( dev_tmp );	// アドレスをdev_tmpに読み込み
				dev_tmp_en = 1;			// フラグ
				lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("data[");
				lcd_disp_hex(data); lcd_putch(']');		// データ長を表示
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
						fputc( xbee_result.DATA[i], fp);	// 文字の場合に書き出し
					}
					fputc( '\n', fp);
					fclose(fp);
				}else lcd_disp("Failed fopen");
				break;
			case MODE_IDNT:				// 新しいデバイスを発見
				xbee_from( dev_tmp );	// 発見したアドレスをdev_tmpに読み込み
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
