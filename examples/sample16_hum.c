/*********************************************************************
サンプルアプリ16　自作・湿度センサー HS-15専用
新しいセンサーデバイスを発見すると、そのデバイスから湿度を得ます。

詳細：http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-hum.html

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port指定	IO名	ピン番号			センサーデバイス
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 照度センサー
			port= 2		DIO2	XBee_pin 18 (AD2)	 温度センサー
			port= 3		DIO3	XBee_pin 17 (AD3)	 湿度センサー
*/
//#define DEBUG
#define XBEE_ERROR_TIME
#include "../libs/xbee.c"

#define LUX_ADJ		1.0						// 照度センサの補正値
#define TEMP_ADJ	+1.8					// 温度センサの補正値
#define BATT_CHECK	100						// 電圧確認間隔(0～255)

byte local_at(char *at){
	byte ret=0;
	char s[40];			// 表示用
	
	ret=xbee_at(at);
	sprintf(s,"local_at  %s ret=",at);
	xbee_log(3,s,ret);
	return(ret);
}

byte remote_at(byte *dev,char *at){
	byte ret=0;
	char s[40];			// 表示用
	
	ret=xbee_rat(dev,at);
	sprintf(s,"remote_at %s ret=",at);
	xbee_log(3,s,ret);
	return(ret);
}

byte set_ports(byte *dev){
	byte ret=0;
	/* 子機の設定 */
	if( (ret=remote_at(dev,"ATPR1FF1"))==0 )	// ポート1～3のプルアップを解除
	if( (ret=remote_at(dev,"ATD102"))==0 )		// ポート1をアナログ入力へ
	if( (ret=remote_at(dev,"ATD202"))==0 )		// ポート2をアナログ入力へ
	if( (ret=remote_at(dev,"ATD302"))==0 )		// ポート3をアナログ入力へ
	if( (ret=remote_at(dev,"ATST01F4"))==0 )	// スリープ移行待ち時間	ST=500ms
	if( (ret=remote_at(dev,"ATSP07D0"))==0 )	// スリープ間隔	 20秒	SP=2000×10ms
	if( (ret=remote_at(dev,"ATSN01"))==0 )		// スリープ回数	 1回 (※「ATSN3C」で20分に)
	if( (ret=remote_at(dev,"ATIR1388"))==0 )	// 自動送信周期			IR=5000ms
	if( (ret=remote_at(dev,"ATWH00"))==0 )		// 測定待ち時間			WH=0ms
	if( (ret=remote_at(dev,"ATSM04"))==0 )		// サイクリックスリープ	SM=04
	if( (ret=remote_at(dev,"ATSI"))==0 )		// 即スリープへ
//	if( (ret=remote_at(dev,"ATWR"))==0 )		// EEPROMへ書き込み
	;
	if(ret) xbee_log(5,"ERROR at set_ports",ret);
	else{
		xbee_delay(1000);
		xbee_log(3,"Sleep",remote_at(dev,"ATSI") );
	}
	return(ret);
}

int main(int argc,char **argv){
	char s[40];						// 表示用
//	byte dev_en = 0;				// センサー発見の有無(0:未発見)
	XBEE_RESULT xbee_result;
	float lux,temp,hum,batt=3.0;	// 照度[Lux]、温度[℃]、湿度[%]、電池電圧[V]
	byte trig_batt=BATT_CHECK;

	byte port=0;		// ↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// センサデバイス
	
	// 初期化処理
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_log(3,"Started",port );
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	xbee_atnj(0xFF);					// 参加受け入れ
	local_at("ATSP0AF0");				// 親機 SP=28000ms (28秒)
	local_at("ATSN0E10");				// 親機 SN=3600回 (28秒*3600回=28時間)	batt = 3.00;
	
	// メイン処理
	xbee_log(3,"Receiving",xbee_atai() );
	while(1){							// 永久に受信する
		/* XBee子機の電圧測定 */
		if( trig_batt == 0){
			xbee_batt_force(dev_sens);
			trig_batt=BATT_CHECK;
		}
		/* データ受信(待ち受けて受信する) */
		xbee_rx_call( &xbee_result );	// データを受信します。
										// data：受信結果の代表値
										// xbee_result：詳細な受信結果
										// xbee_result.MODE：受信モード
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_RESP:				// sensorからの応答の場合に照度と温度を表示
				if( xbee_result.STATUS != STATUS_OK) break;
			case MODE_GPIN:				// sensorからの応答の場合に照度と温度を表示
				sprintf(s,"Recieved %d,%d,%d, device=",xbee_result.ADCIN[1],xbee_result.ADCIN[2],xbee_result.ADCIN[3]);
				xbee_log(5,s,xbee_result.FROM[7] );
				if( bytecmp(xbee_result.FROM,dev_sens,8)==0 ){
					/* 照度測定
					    照度センサ    [+3.3V]----(C)[照度センサ](E)--+--[10kΩ]----[GND]
					    =NJL7502L                                    |
					                                                 +--(19)[XBee ADC1]
					                                                 
						lux = I(uA) / 0.3 = ADC(mV) / 10(kOhm) / 0.3
						    = ADC値 /1023 * 1200(mV) / 10(kOhm) / 0.3
						    = ADC値 * 0.391
					*/
					lux = (float)xbee_result.ADCIN[1] * 0.391 * LUX_ADJ;
					sprintf(s,"Ilum = %.1f[Lux], adc1",lux);
					xbee_log(5,s,(byte)(xbee_result.ADCIN[1]/4) );
					
					/* 温度測定
					    温度センサ    [+3.3V]----(1VSS)[温度センサ](3GND)---[GND]
					    =LM61CIZ                      (2VOUT)|
					                                         +--(18)[XBee ADC2]
					                                         
						Temp = ( ADC(mV) - 600 ) / 10 = ( ADC値 /1023 * 1200(mV) - 600 ) / 10
						     = ADC値 * 0.1173 - 60;
					*/
					temp = (float)xbee_result.ADCIN[2] * 0.1173 - 60 + TEMP_ADJ;
					sprintf(s,"Temp = %.1f[degC], adc2",temp);
					xbee_log(5,s,(byte)(xbee_result.ADCIN[2]/4) );
						
					/* 湿度測定
						湿度センサ HS-15P
						[+3.3V]---[series100k]--+--[0.22uF]--+--[HS-15P]---[GND]
						           分圧         |            |
						    [GND] --[shunt68k]--+            +--(17)[XBee ADC3]
						    
						その他の変換
							HS15湿度センサーと簡易回路による湿度計実験
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
			case MODE_BATT:				// 電池電圧を受信
				if( bytecmp(xbee_result.FROM,dev_sens,8)==0 ){
					batt = (float)xbee_result.ADCIN[0] /1000.;
					sprintf(s,"Batt = %.3f[V], adc0",batt);
					xbee_log(5,s,(byte)(xbee_result.ADCIN[0]/15) );
					trig_batt=BATT_CHECK;
				}
				break;
			case MODE_IDNT:				// 新しいデバイスを発見
				xbee_from( dev_sens );	// 受信したアドレスをdev_sensへ格納
			//	dev_en = 1;				// sensor発見済
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
