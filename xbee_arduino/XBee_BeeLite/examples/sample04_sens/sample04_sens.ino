/*********************************************************************
BeeBee Lite sample04_sens by 蘭
*********************************************************************/

/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

/*********************************************************************
インポート処理						※ライブラリ XBeeに必要な宣言です
*********************************************************************/

#include <xbee_lite.h>

	// コンパイル後サイズ比較
	// V1.90β0906 xbee.h		13,998 bytes
	// V1.90β0906 xbee_lite.h	13,186 bytes

/*********************************************************************
アプリケーション					※以下を編集してアプリを作成します
*********************************************************************/

#define ADAFRUIT	// 使用する液晶が Adafruit LCD Sheild の時に定義する

#ifndef ADAFRUIT
	// 通常のキャラクタLCDの場合
	#include <LiquidCrystal.h>
	LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#else
	// Adafruit I2C接続LCDの場合(Adafruit_RGBLCDShieldライブラリが必要)
	#include <Wire.h>
	#include <Adafruit_MCP23017.h>
	#include <Adafruit_RGBLCDShield.h>
	Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
#endif

byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x40,0x00,0x00,0x00};	// 仮設定
byte dev_en = 0;	// センサー発見の有無(0:未発見)

void setup(){
	lcd.begin(16, 2);
	lcd.clear(); lcd.print("Sample 4 SENS");
	xbee_init( 0 );						// XBeeの初期化(引数はリトライ回数)
}

void loop(){
	byte i;
	byte trig=0;
	float value;

	XBEE_RESULT xbee_result;			// 受信データ用の構造体

	// デバイス探索
	lcd.clear(); lcd.print("Searching:SENSOR");
	if( xbee_atnj(10) ){				// 10秒間の参加受け入れ
		lcd.clear(); lcd.print("Found a device");
		xbee_from( dev_sens );			// 受信したアドレスをdev_sensへ格納
		dev_en = 1;						// sensor発見済
		xbee_gpio_config( dev_sens, 1 , AIN ); // 接続相手のポート1をAIN入力へ
		xbee_gpio_config( dev_sens, 2 , AIN ); // 接続相手のポート2をAIN入力へ
		// xbee_gpio_config( dev_sens, 3 , AIN ); // 接続相手のポート3をAIN入力へ
	}else{
		lcd.clear(); lcd.print("Failed:no dev.");
	}
	delay(500);
	lcd.clear(); lcd.print("DONE");
	
	// メイン処理
	lcd.clear(); lcd.print("Receiving");
	while(1){							// 永久に受信する
		if( dev_en && trig == 0){
			xbee_force( dev_sens );		// デバイスdev_sensへデータ要求(100回に1回の頻度で)
			trig = 100;
		}
		trig--;

		/* データ受信(待ち受けて受信する) */
		xbee_rx_call( &xbee_result );
										// データを受信します。
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_RESP:				// sensorからの応答の場合に照度と温度を表示
			case MODE_GPIN:				// 周期方式でデータ受信した場合も想定
				if( bytecmp( dev_sens , &(xbee_result.FROM[0]) ,8 ) == 0 && xbee_result.STATUS == STATUS_OK ){
					// 照度測定結果
					value = xbee_sensor_result( &xbee_result, LIGHT);
					lcd.clear(); lcd.print( value , 1 );	lcd.print(" Lux ");
					// 温度測定結果
					value = xbee_sensor_result( &xbee_result, TEMP);
					lcd.setCursor(0,1); lcd.print( value , 1 ); lcd.print(" C");
				}
				break;
			case MODE_IDNT:				// 新しいデバイスを発見
				lcd.clear(); lcd.print("found a new dev");
				for( i=0;i<8;i++ ) dev_sens[i]=xbee_result.FROM[i];
										// 発見したアドレスをdev_sensに読み込み
				dev_en = 1;				// sensor発見済
				xbee_gpio_config( dev_sens, 1 , AIN ); // 接続相手のポート1をAIN入力へ
				xbee_gpio_config( dev_sens, 2 , AIN ); // 接続相手のポート2をAIN入力へ
				// xbee_gpio_config( dev_sens, 3 , AIN ); // 接続相手のポート3をAIN入力へ
				break;
			default:
				break;
		}
	}
}
