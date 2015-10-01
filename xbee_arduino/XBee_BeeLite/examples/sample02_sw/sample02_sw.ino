/*********************************************************************
 BeeBee Lite sample02_sw by 蘭
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
	// V1.90β0906 xbee.h		12,104 bytes
	// V1.90β0906 xbee_lite.h	11,292 bytes

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

void setup(){
	lcd.begin(16, 2);
	lcd.clear(); lcd.print("Sample 2 SW");
	xbee_init( 0x00 );
	lcd.clear(); lcd.print("ATNJ");
	xbee_atnj( 0xFF );
	delay(1000);
}

void loop(){
	byte data;
	char i;
	byte dev_gpio[8];

	XBEE_RESULT xbee_result;

	// メイン処理
	lcd.clear(); lcd.print("Waiting");
	while(1){
		/* データ受信 */
		data = xbee_rx_call( &xbee_result );
		if( xbee_result.MODE == MODE_GPIN){
			lcd.clear();
			for( i=7; i>=0 ; i--){
				lcd.print( (char)( '0' + ((data>>i) & 0x01) ) );
			}
		}
		// ペアリング
		if( xbee_result.MODE == MODE_IDNT){
			lcd.clear(); lcd.print("Found");
			for(i=0;i<8;i++) dev_gpio[i]=xbee_result.FROM[i];
			i = xbee_gpio_init(dev_gpio);
			if(i){
				lcd.setCursor(0,1);
				lcd.print("Error");		// gpio_init処理の失敗
			}
		}
	}
}
