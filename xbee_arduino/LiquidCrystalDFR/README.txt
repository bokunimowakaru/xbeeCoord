/*********************************************************************
DF ROBOT LCD Keypad Shield 用 液晶＆キーパッドドライバ

Arduino 標準ライブラリにLiquidCrystalにDF ROBOT社Keypad用関数を
追加しました。

関連	http://www.geocities.jp/bokunimowakaru/diy/arduino/lcds.html

本ソースリストおよびソフトウェアはGPL V2に基づくライセンスを有します。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2012-2013 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

/*
使用方法：

このフォルダ内の下記のファイルをArduino IDEをインストールしている
libraries/LiquidCrystalフォルダ内のものと置き換えます。

	ファイル：
		LiquidCrystal.h
		LiquidCrystal.cpp

	場所(例)：
		C:\arduino\libraries\LiquidCrystal

サンプルスケッチはlibraries/examples/HelloWorld内の下記のファイルを
置き換えます。

	ファイル：
		HelloWorld.pde

	場所(例)：
		C:\arduino\libraries\LiquidCrystal\examples\HelloWorld

*/

/***************************************
LiquidCrystal.h		60行目
****************************************/

// for button by Wataru
#define BUTTON_UP 0x08
#define BUTTON_DOWN 0x04
#define BUTTON_LEFT 0x10
#define BUTTON_RIGHT 0x02
#define BUTTON_SELECT 0x01

/***************************************
LiquidCrystal.h		106行目
****************************************/

  uint8_t readButtons();	// Wataru

/***************************************
LiquidCrystal.cpp	327行目から
****************************************/

	// Wataru
	/******************************************************

	This program will test the LCD panel and the buttons
	Mark Bramwell, July 2010 

	********************************************************/

	uint8_t LiquidCrystal::readButtons(void) {
	  int adc_key_in = analogRead(0);      // read the value from the sensor
	  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741 
	  // we add approx 50 to those values and check to see if we are close 
	  if (adc_key_in > 1000) return 0x00; // We make this the 1st option for speed reasons since it will be the most likely result 
	  if (adc_key_in < 50)   return BUTTON_RIGHT;
	  if (adc_key_in < 195)  return BUTTON_UP;
	  if (adc_key_in < 380)  return BUTTON_DOWN;
	  if (adc_key_in < 555)  return BUTTON_LEFT;
	  if (adc_key_in < 790)  return BUTTON_SELECT;
	  return 0x1F;  // when all others fail, return this...
	}

