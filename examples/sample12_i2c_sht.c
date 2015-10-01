/*********************************************************************
サンプルアプリ12 湿度センサ Sensirion SHT用 デジタルIFサンプル

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
xbee_i2c_read(const byte *address, byte adr,byte len,byte *rx){
	
子機XBeeデバイス(リモート先)のI2Cから指定バイトの情報を読み取る関数
入力：byte adr　I2Cアドレス
入力：byte len　受信長、０の時は Sensirion SHT用２バイト受信
出力：byte *rx　受信データ用ポインタ
			port=11		DIO11	XBee_pin  7		SCL
			port=12		DIO12	XBee_pin  4		SDA

XBeeハードウェア
	Sensirion SHT-11またはSHT-71をXBeeの1,4,7,10番ピンに接続する。
	ピンはちょうど3つおきなので、引き出しやすいと思います。
			XBee 1 -> SHT 2 (VCC)
			XBee 4 -> SHT 4 (SDA)
			XBee 7 -> SHT 1 (SCL)
			XBee10 -> SHT 3 (GND)
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
	byte data[2];
	byte i;	//　↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};	// PIO デバイス
	byte port=0;
	float temp,hum,hum_;
	
	// 初期化処理
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// XBee用COMポートの初期化(引数はポート番号)
	xbee_atnj( 0xFF );					// デバイスを常に参加受け入れ(テスト用)
	xbee_i2c_init(dev);					// I2Cインタフェースの初期化
	
	// メイン処理
	while(1){									// 永久に受信する
		print_timer();
		//	i= xbee_i2c_read(dev,0x51 ,1,data);
		i=xbee_i2c_read(dev,1,data,0);	// 温度の読み取り
		if(i==2){
			temp = (float)data[0]*256. + (float)data[1];
			temp *= 0.01;
			temp -= 39.7;
			printf("SHT TEMP = %.2f ",temp);
		}else{
			printf("SHT TEMP = ERROR");
			temp=27.;
		}
		printf("(%02X,%02X)\n",data[0],data[1]);
		
		print_timer();
		i=xbee_i2c_read(dev,2,data,0);	// 湿度の読み取り
		if(i==2){
			hum_= (float)data[0]*256. + (float)data[1];
			hum = -1.5955e-6 * hum_ * hum_;
			hum += 0.0367 * hum_;
			hum -= 2.0468;
			hum += (temp - 25) * (0.01 + 0.00008 * hum_ );
			printf("SHT HUM  = %.2f ",hum);
		}else{
			printf("SHT HUM  = ERROR");
		}
		printf("(%02X,%02X)\n\n",data[0],data[1]);
	}
}

#endif
