/*********************************************************************
// BeeBee Lite sample01_led by 蘭
*********************************************************************/

/*********************************************************************
サンプルアプリ１　リモート先のLEDをON/OFFする。

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

/*********************************************************************
インポート処理						※ライブラリ XBeeに必要な処理です
*********************************************************************/

#include <xbee_lite.h>

	// コンパイル後のサイズ比較
	// V1.90β0906 xbee.h		12,894 bytes
	// V1.90β0906 xbee_lite.h	12,048 bytes

/*********************************************************************
アプリケーション					※以下を編集してアプリを作成します
*********************************************************************/

#define ADAFRUIT	// 使用する液晶が Adafruit LCD Sheild の時に定義する

#ifndef ADAFRUIT
	// 通常のキャラクタLCDの場合
	#include <LiquidCrystalDFR.h>
	LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#else
	// Adafruit I2C接続LCDの場合(Adafruit_RGBLCDShieldライブラリが必要)
	#include <Wire.h>
	#include <Adafruit_MCP23017.h>
	#include <Adafruit_RGBLCDShield.h>
	Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
#endif

#define BUTTON_PIN	14		// Digital 14 ピン(Analog 0 ピン)にボタンを接続

void setup(){
	pinMode(BUTTON_PIN, INPUT);   		// Digital 14 ピン (Analog 0 ピン)を入力に
	lcd.begin(16, 2);
	lcd.clear(); lcd.print("Sample 1 LED");
	xbee_init( 0x00 );					// XBee用COMポートの初期化(引数はポート番号)
	lcd.clear(); lcd.print("ATNJ");
	xbee_atnj( 0xFF );					// デバイスを常に参加受け入れ(テスト用)
}

void loop(){
	byte c = 0;
	byte dev_gpio[8];
	XBEE_RESULT xbee_result;
	
	lcd.clear(); lcd.print("Koki botan push!");
	do{
		xbee_rx_call(&xbee_result);
	}while(xbee_result.MODE != MODE_IDNT);
	for(int i=0;i<8;i++) dev_gpio[i]=xbee_result.FROM[i];
	lcd.clear(); lcd.print("gpio init");
	xbee_gpio_init(dev_gpio);			// デバイスdev_gpioにIO設定を行うための送信
	delay(1000);

	while(1){
		c = lcd.readButtons();
		switch( c ){
			case BUTTON_DOWN:
				xbee_gpo(dev_gpio,12,1);		// GPOポート12をHレベル(消灯)へ
				xbee_gpo(dev_gpio,11,1);		// GPOポート11をHレベル(消灯)へ
				break;
			case BUTTON_LEFT:
				xbee_gpo(dev_gpio,12,0);		// GPOポート12をLレベル(点灯)へ
				break;
			case BUTTON_RIGHT:
				xbee_gpo(dev_gpio,11,0);		// GPOポート11をLレベル(点灯)へ
				break;
			case BUTTON_UP:
				xbee_gpo(dev_gpio,12,0);		// GPOポート12をHレベル(消灯)へ
				xbee_gpo(dev_gpio,11,0);		// GPOポート11をHレベル(消灯)へ
				break;
			default:
				break;
		}
		lcd.clear(); lcd.print("Oyaki key push!");
		lcd.setCursor(0,1); lcd.print( c , BIN );
		while( c == lcd.readButtons() );
	}
}
