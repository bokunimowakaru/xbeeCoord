/*********************************************************************
サンプルアプリ13 秋月 小型LCD I2C

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

小型液晶に表示されるまで「約２分半」ほどかかります。

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

処理速度

	XBee Coord	起動		約10秒
	LCD Init	初期化 		約50秒
	LCD Data 1	1行目表示	約45秒
	LCD Data 1	1行目表示	約45秒
*/

#include "../libs/xbee.c"
#ifdef LITE	// BeeBee Lite by 蘭
	int main(){
		printf("Lite mode dewaugokimasen. gomenne.");
		return 0;
	}
#else

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

int main(int argc,char **argv){
	char lcd1[]="ﾎﾞｸﾆﾓﾜｶﾙ";				// 液晶1行目(8文字)
	char lcd2[]=" XBee ZB";				// 液晶2行目(8文字)
	byte data[2];
	byte i;	//　↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};	// PIO デバイス

	byte port=0;
	
	// 初期化処理
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// XBee用COMポートの初期化(引数はポート番号)
	xbee_atnj( 0xFF );					// デバイスを常に参加受け入れ(テスト用)
	xbee_i2c_init(dev);					// I2Cインタフェースの初期化
	
	// メイン処理
	
	print_timer();printf("LCD Init\n");
//	data[0]=0x00; data[1]=0x38; xbee_i2c_write(dev,0x7C,data,2);	// IS=0
	data[0]=0x00; data[1]=0x39; xbee_i2c_write(dev,0x7C,data,2);	// IS=1
//	data[0]=0x00; data[1]=0x14; xbee_i2c_write(dev,0x7C,data,2);	// OSC Freq.	4
	data[0]=0x00; data[1]=0x7E; xbee_i2c_write(dev,0x7C,data,2);	// コントラスト	E
	data[0]=0x00; data[1]=0x5D; xbee_i2c_write(dev,0x7C,data,2);	// Power/Cont	D
	data[0]=0x00; data[1]=0x6C; xbee_i2c_write(dev,0x7C,data,2);	// FollowerCtrl	C
	delay(200);
	data[0]=0x00; data[1]=0x38; xbee_i2c_write(dev,0x7C,data,2);	// IS=0
	data[0]=0x00; data[1]=0x0C; xbee_i2c_write(dev,0x7C,data,2);	// DisplayON	C
//	data[0]=0x00; data[1]=0x01; xbee_i2c_write(dev,0x7C,data,2);	// ClearDisplay
//	data[0]=0x00; data[1]=0x06; xbee_i2c_write(dev,0x7C,data,2);	// EntryMode	6
	
	print_timer();printf("LCD Data 1\n");
	data[0]=0x00; data[1]=0x80; xbee_i2c_write(dev,0x7C,data,2);	// 1行目
	for(i=0;i<8;i++){
		data[0]=0x40;
		data[1]=(byte)lcd1[i];
		xbee_i2c_write(dev,0x7C,data,2);
	}
	
	print_timer();printf("LCD Data 2\n");
	data[0]=0x00; data[1]=0xC0; xbee_i2c_write(dev,0x7C,data,2);	// 2行目
	for(i=0;i<8;i++){
		data[0]=0x40;
		data[1]=(byte)lcd2[i];
		xbee_i2c_write(dev,0x7C,data,2);
	}
	
	print_timer();printf("Done\n");
	return(0);
}

#endif
