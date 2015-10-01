/*********************************************************************
 BeeBee Lite sample46_log by 蘭
*********************************************************************/

/***************************************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

取得した情報をファイルに保存するロガーの製作
※DF ROBOT製の液晶がSDカードの信号と干渉するのでAdafruit製の液晶を使用しています。

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/
#include <xbee_lite.h>

	// コンパイル後サイズ比較
	// V1.90β0906 xbee.h		24,652 bytes
	// V1.90β0906 xbee_lite.h	23,386 bytes

#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <SD.h>

#define PIN_SDCS    4                           // SDのチップセレクトCS(SS)のポート番号

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
File file;                                      // SDファイルの定義
byte dev[8];                                    // XBee子機デバイスのアドレス

void setup() {
    lcd.begin(16, 2);                           // LCDのサイズを16文字×2桁に設定
    lcd.print("Example 46 LOG");                // 文字を表示
    while(SD.begin(PIN_SDCS)==false){           // SDカードの開始
        delay(5000);                            // 失敗時は5秒ごとに繰り返し実施
    }
    xbee_init( 0 );                             // XBee用COMポートの初期化
    lcd.setCursor(0, 1);                        // LCDのカーソル位置を2行目の先頭に
    lcd.print("Waiting for XBee");              // 待ち受け中の表示
    while( xbee_atnj(30)==0x00 );               // 子機XBeeデバイスのペアリング
    lcd.clear();
    lcd.print("Found");
    xbee_from(dev);
    xbee_gpio_config( dev, 1 , AIN );           // XBee子機のポート1をアナログ入力へ
    xbee_end_device( dev, 3, 3, 0);             // 起動間隔3秒,自動測定3秒,S端子無効
}

void loop() {
    byte i;
    
    float value;                                // 受信データの代入用
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)

    unsigned long time=millis()/1000;           // 時間[秒]の取得(約50日まで)

    xbee_rx_call( &xbee_result );               // データを受信
    if( xbee_result.MODE == MODE_GPIN){         // 受信したデータの内容に応じて
        value = (float)xbee_result.ADCIN[1] * 3.55;
        lcd.clear();
        lcd.print( time );
        lcd.setCursor(0, 1);                    // LCDのカーソル位置を2行目の先頭に
        lcd.print( value, 1 );
        lcd.print( " Lux" );
        file=SD.open("TEST.CSV", FILE_WRITE);   // 書込みファイルのオープン
        if(file == true){                       // オープンが成功した場合
            file.print( time );
            file.print( ", " );
            file.println( value, 1 );           // value値を書き込み
            file.close();                       // ファイルクローズ
        }else{
            lcd.print(" ERR");
        }
    }
}
