/*********************************************************************
サンプルアプリ１　リモート先のLEDをON/OFFする。
起動後５秒くらいすると「input」の表示が現れるので0～2の数字を入れて
リターンキーを押す。
１を入れるとLED1が点灯、２を入れるとLED2が点灯、0で両方が消えます。
リモート先XBee子機は、LED1をDIO11(XBee_pin 7)に、LED2をDIO12(XBee_
pin 4)に接続しておきます。

解説：下記のサイトにも情報が掲載されています。
http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-tutorial_2.html

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port指定	IO名	ピン番号			USB評価ボード(XBIB-U-Dev)
			port=11		DIO11	XBee_pin  7			    LED2	※port11がLED2
			port=12		DIO12	XBee_pin  4			    LED1	※port12がLED1
*/

#define	H3694			//重要：H3694をdefineするとH8マイコン用になります。
//#define ERRLOG			//ERRLOGをdefineするとエラーログが出力されます。
#include "../libs/xbee.c"

int main(int argc,char **argv){
	char s[3];
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
	while(1){									// 永久に受信する
		#ifdef H3694
			if( s[0] == '1'){
				while( button() == 1 );
				s[0]= '0';
			}else{
				while( button() == 0 );
				s[0]='1';
			}
		#else
			printf("input[0-2]:");
			gets( s );
		#endif
		switch( s[0] ){
			case '0':
				xbee_gpo(dev_gpio,12,1);		// GPOポート12をHレベル(消灯)へ
				xbee_gpo(dev_gpio,11,1);		// GPOポート11をHレベル(消灯)へ
				break;
			case '1':
				xbee_gpo(dev_gpio,12,0);		// GPOポート12をLレベル(点灯)へ
				break;
			case '2':
				xbee_gpo(dev_gpio,11,0);		// GPOポート11をLレベル(点灯)へ
				break;
		}
	}
}
