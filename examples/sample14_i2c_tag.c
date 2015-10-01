/*********************************************************************
サンプルアプリ14 秋月 小型LCD I2C Router

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

小型液晶に表示されるまで「約２分」ほどかかります。
子機XBee モジュールは「Router」に設定します。(End Deviceでは動きません)

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
XBeeハードウェア
	Sensirion SHT-11またはSHT-71をXBeeの1,4,7,10番ピンに接続する。
	ピンはちょうど3つおきなので、引き出しやすいと思います。
			XBee 1 -> LCD 2 (VCC)
			XBee 4 -> LCD 4 (SDA)
			XBee 7 -> LCD 3 (SCL)
			XBee10 -> LCD 5 (GND)

処理時間
	XBee Coord	起動		約10秒
	LCD Init	初期化 		約30秒
	LCD Data 1	1行目表示	約45秒
	LCD Data 1	1行目表示	約45秒
	
消費電流
	初期化時	24mA ×120秒				＝  0.8 mAh
	書き換え時	24mA × 90秒				＝  0.6 mAh
	
	表示時		0.8mA×24時間				＝ 19.2 mAh
	間欠通信時	40mA ×0.5秒／60秒×24時間	＝  8.0 mAh
	表示時の消費電流量の合計 19.2mAh+24mAh	＝ 27.2 mAh
*/
//#define DEBUG
#include "../libs/xbee.c"
#include "../libs/kbhit.c"
#ifdef LITE	// BeeBee Lite by 蘭
	int main(){
		printf("Lite mode dewaugokimasen. gomenne.");
		return 0;
	}
#else

//　　　　　　　↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
byte dev[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};

void print_timer(){						// 時計用
	time_t timer;
	struct tm *time_st;
	
	time(&timer);
	time_st = localtime(&timer);
	printf("%4d/%02d/%02d %02d:%02d:%02d ",
		time_st->tm_year+1900,
		time_st->tm_mon+1,
		time_st->tm_mday,
		time_st->tm_hour,
		time_st->tm_min,
		time_st->tm_sec );
}

void print_data(byte *dev, char *lcd, byte y, char *s){
	byte data[2];
	byte i;
	print_timer();printf("%s\tDisplay '%s'\n",s,lcd);
	xbee_rat(dev,"ATSTFFFE");	// Time before Sleep 約65秒間
	data[0]=0x00;
	if(y==0) data[1]=0x80; else data[1]=0xC0;
	xbee_i2c_write(dev,0x7C,data,2);
	for(i=0;i<8;i++){
		data[0]=0x40;
		data[1]=(byte)lcd[i];
		xbee_i2c_write(dev,0x7C,data,2);
	}
}

void wakeup(byte *dev, char *s){
	print_timer();printf("%s\tWake XBee Up\n",s);
	xbee_rat(dev,"ATSTFFFE");	// Time before Sleep 約65秒間
	xbee_rat(dev,"ATSP20");		// Sleep Period 
	xbee_rat(dev,"ATSN01");		// Sleep Number
	xbee_rat(dev,"ATIR00");		// Periodic IO Sampling OFF
	xbee_rat(dev,"ATSO02");		// Sleep Options = Always wake for full ST time
}

void goto_sleep(byte *dev, char *s){
	print_timer();printf("%s\tXBee Go Sleep\n",s);
	xbee_rat(dev,"ATSO00");		// Sleep Options = 通常モード
	xbee_rat(dev,"ATST01F4");	// Time before Sleep 0.5秒間
	xbee_rat(dev,"ATSP07D0");	// Sleep Period 
	xbee_rat(dev,"ATSN03");		// Sleep Number
	xbee_rat(dev,"ATIREA60");	// Periodic IO Sampling
	xbee_rat(dev,"ATSM04");		// Sleep Mode = CYCLIC SLEEP
}

int main(int argc,char **argv){
	char lcd[2][9]={"ﾎﾞｸﾆﾓﾜｶﾙ",	// 液晶1行目(8文字)
					" XBee ZB"};// 液晶2行目(8文字)
	byte lcd_x=0,lcd_y=0;		// 表示位置
	byte disp_sw=0;				// 表示変更フラグ
	byte data[2];
	byte i,j;
	XBEE_RESULT xbee_result;

	byte port=0;
	
	// setup処理
	print_timer();printf("setup\tXBee Init\n");
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );			// XBee用COMポートの初期化(引数はポート番号)
	xbee_at("ATSP07D0");		// 親機の設定 Sleep Period 
	xbee_at("ATSN03");			// 親機の設定 Sleep Number
	xbee_atnj( 0xFF );			// デバイスを常に参加受け入れ(テスト用)
	while( xbee_rat(dev,"ATSTFFFE")!=STATUS_OK ){
		print_timer();printf("setup\tWainting for XBee\n");
		delay(1000);
	}
	wakeup(dev,"setup");
	xbee_ratd_myaddress(dev);	// 親機のアドレスを子機に設定
	xbee_rat(dev,"ATD103");		// DIOポート1を入力に
	xbee_rat(dev,"ATPR1FFF");	// 全ポートにプルアップを設定
	xbee_i2c_init(dev);			// I2Cインタフェースの初期化
	
	print_timer();printf("setup\tLCD Init\n");
	data[0]=0x00; data[1]=0x39; xbee_i2c_write(dev,0x7C,data,2);	// IS=1
	data[0]=0x00; data[1]=0x7E; xbee_i2c_write(dev,0x7C,data,2);	// コントラスト	E
	data[0]=0x00; data[1]=0x5D; xbee_i2c_write(dev,0x7C,data,2);	// Power/Cont	D
	data[0]=0x00; data[1]=0x6C; xbee_i2c_write(dev,0x7C,data,2);	// FollowerCtrl	C
	delay(200);
	data[0]=0x00; data[1]=0x38; xbee_i2c_write(dev,0x7C,data,2);	// IS=0
	data[0]=0x00; data[1]=0x0C; xbee_i2c_write(dev,0x7C,data,2);	// DisplayON	C

	print_data(dev,lcd[0],0,"setup");
	print_data(dev,lcd[1],1,"setup");
	
	goto_sleep(dev,"setup");
	print_timer();printf("setup\tDone\n");
	xbee_clear_cache();			// 受信キャッシュのクリア

	// loop 処理
	while(1){
		if ( kbhit() ){
			j = (byte)getchar();
			if(j == (byte)'\n'){
				for(i=lcd_x;i<8;i++) lcd[lcd_y][i]=' ';
				printf("\nDisplay(%d) '%s'\n",lcd_y+1,lcd[lcd_y]);
				if( disp_sw < 2 ) disp_sw++;
				print_timer();printf("loop\tWaiting for XBee\n");
				lcd_x=0;
				lcd_y++;
				if(lcd_y > 1) lcd_y=0;
			}else if(j >= 0x20 && lcd_x<8){
				lcd[lcd_y][lcd_x]=(char)j;
				lcd_x++;
			}
		}
		
		xbee_rx_call( &xbee_result );	// データを受信します。
		
		// 受信時の処理
		if(bytecmp(dev, xbee_result.FROM ,8 )==0){
			print_timer();printf("loop\tFound XBee Packet\n");
			if( disp_sw ){
				wakeup(dev,"resp");
				if(disp_sw==1){
					if(lcd_y) lcd_y=0; else lcd_y=1;
					print_data(dev,lcd[lcd_y],lcd_y,"resp");
					if(lcd_y) lcd_y=0; else lcd_y=1;
				}else{	// disp_sw==2
					print_data(dev,lcd[0],0,"resp");
					print_data(dev,lcd[1],1,"resp");
				}
				goto_sleep(dev,"resp");
				disp_sw=0;
			}
			xbee_clear_cache();			// 受信キャッシュのクリア
		}
	}
}

#endif
