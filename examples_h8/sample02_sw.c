/*********************************************************************
サンプルアプリ２　スイッチ
子機のスイッチ2～4(Port1～3)を押した時に、その状態を液晶に表示します。
XBee子機のDIO1～3(XBee_pin 19～17)をHigh や Low にすると PCまたはH8の
親機にスイッチの状態が表示されます。

玄関チャイム（子機）などの検出用に使用します。

解説：下記のサイトにも情報が掲載されています。
http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-tutorial_2.html

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port指定	IO名	ピン番号			USB評価ボード(XBIB-U-Dev)
			port= 0		DIO0	XBee_pin 20 (Cms)	    SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	    SW2 
			port= 2		DIO2	XBee_pin 18 (AD2)	    SW3 
			port= 3		DIO3	XBee_pin 17 (AD3)	    SW4 
			
	子機をEnd Deviceにする場合、X-CTUを使用して子機XBeeに以下を設定しておきます。
	SPとSNは親機にも設定しておきます。
		
			DH: 親機(宛先)の上位アドレス
			DL: 親機(宛先)の下位アドレス
			SM: Sleep Mode			= 05   hex (CYCLIC SLEEP PIN-WAKE)
			SP: Cyclic Sleep Period	= 07D0 hex (20秒)
			SN: Sleep Number 		= 0E10 hex (3600回)
			D1: AD1/DIO1 Config.	= 03   hex (DIGITAL INPUT)
			PR: Pull-up Resistor	= 1FFF hex (プルアップ有効)
			IC: I/O Change Detection= 000E hex (port 1～3を検出設定)
			
			また、起動用のボタンをSLEEP_RQ(XBee_pin 9)に接続します。
			例えば、ボタンport=1とする場合、SLEEP_RQ(XBee_pin 9)と
			DIO1(XBee_pin 19)と間を接続しておきます。
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
	byte data;
	XBEE_RESULT xbee_result;

	byte port=0;		//　↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev_gpio[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};	// PIO デバイス
	
	// 初期化処理
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// XBee用COMポートの初期化(引数はポート番号)
	xbee_atnj( 0xFF );					// デバイスを常に参加受け入れ(テスト用)
	xbee_gpio_init(dev_gpio);			// デバイスdev_gpioにIO設定を行うための送信
	
	// メイン処理
	lcd_disp("Waiting"); lcd_home();
	while(1){
		/* データ受信(待ち受けて受信する) */
		xbee_result.MODE = MODE_AUTO;			// 自動受信モードに設定
		data = xbee_rx_call( &xbee_result );	// データを受信します。
												// data：受信結果が代入されます
		if( xbee_result.MODE == MODE_GPIN){		// PIO入力(リモート機のボタンが押された)の時
			lcd_disp_bin( data );				// dataに入った値をバイナリで表示
			lcd_home();
		}
	}
}
