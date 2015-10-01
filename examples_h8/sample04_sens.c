/*********************************************************************
サンプルアプリ４　センサー
新しいセンサーデバイスを発見すると、そのデバイスから照度と温度を得ます。

ここでは子機のXBeeセンサーにDigi純正のXBee Sensorを想定していますが、
自作したセンサーでもDIO1～2(XBee_pin 19～18)に0～1.2Vのアナログ電圧を
与えていれば、何らかの値を読みとることができます。

新しいXBee子機のコミッションボタンDIO0(XBee_pin 20)を押下する(信号レベ
ルがH→L→Hに推移する)と、子機から参加要求信号が発行されて、PCまたは
H8親機が子機を発見し、「found a device」と表示します。

ただし、複数のZigBeeネットワークに対応するために、ペアリングは起動後の
10秒間しか受け付けません。PC/H8親機を起動して、コミッションボタンを
押してペアリングを実施しておき、再度、コミッションボタンでデバイス発見
します。デバイス発見は10秒のペアリング期間後でも行えます。

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port指定	IO名	ピン番号			センサーデバイス
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 照度センサー
			port= 2		DIO2	XBee_pin 18 (AD2)	 温度センサー
			port= 3		DIO3	XBee_pin 17 (AD3)	 湿度センサー
*/

#define	H3694			//重要：H3694をdefineするとH8マイコン用になります。
//#define ERRLOG			//ERRLOGをdefineするとエラーログが出力されます。

#ifdef H3694
	#ifndef __3694_H__
		#include <3694.h>
	#endif
	#include "../libs/lcd_h8.c"
#else
	#include "../libs/lcd_pc.c"
#endif
#include "../libs/xbee.c"

int main(int argc,char **argv){
	byte i;
	byte trig=0;
	byte dev_en = 0;	// センサー発見の有無(0:未発見)
	XBEE_RESULT xbee_result;
	float value;

	byte port=0;		// ↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// センサデバイス
	
	// 初期化処理
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	
	// デバイス探索
	lcd_disp("Searching:SENSOR");
	if( xbee_atnj(10) ){				// 10秒間の参加受け入れ
		lcd_disp("Found device");
		xbee_from( dev_sens );			// 受信したアドレスをdev_sensへ格納
		dev_en = 1;						// sensor発見済
		xbee_gpio_config( dev_sens, 1 , AIN ); // 接続相手のポート1をAIN入力へ
		xbee_gpio_config( dev_sens, 2 , AIN ); // 接続相手のポート2をAIN入力へ
		// xbee_gpio_config( dev_sens, 3 , AIN ); // 接続相手のポート3をAIN入力へ
	}else{
		lcd_disp("Failed:no dev.");
	}
	
	// メイン処理
	lcd_disp("Receiving");
	while(1){							// 永久に受信する
		if( dev_en && trig == 0){
			xbee_force( dev_sens );		// デバイスdev_sensへデータ要求(100回に1回の頻度で)
			trig = 250;
		}
		trig--;

		/* データ受信(待ち受けて受信する) */
		xbee_result.MODE = MODE_AUTO;	// 自動受信モードに設定
		xbee_rx_call( &xbee_result );	// データを受信します。
										// data：受信結果の代表値
										// xbee_result：詳細な受信結果
										// xbee_result.MODE：受信モード
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_RESP:				// sensorからの応答の場合に照度と温度を表示
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 &&
				xbee_result.STATUS == STATUS_OK ){
					// 照度測定結果
					value = xbee_sensor_result( &xbee_result,LIGHT);
					lcd_cls();	lcd_goto(LCD_ROW_1);lcd_disp_5( (unsigned int)value );	lcd_putstr("Lux ");
					// 温度測定結果
					value = xbee_sensor_result( &xbee_result,TEMP);
					i = (byte)value; lcd_disp_2( i ); lcd_putstr(".");
					i = (byte)((value-(float)i)*100); lcd_disp_2( i ); lcd_putstr("degC");
				}
				break;
			case MODE_IDNT:				// 新しいデバイスを発見
				lcd_disp("found a new device");
				xbee_from( dev_sens );	// 受信したアドレスをdev_sensへ格納
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
