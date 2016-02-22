/*********************************************************************
サンプルアプリ１　リモート先のLEDをON/OFFする。
起動後５秒くらいすると「input」の表示が現れるので0～2の数字を入れて
リターンキーを押す。
１を入れるとLED1が点灯、２を入れるとLED2が点灯、0で両方が消えます。
リモート先XBee子機は、LED1をDIO11(XBee_pin 7)に、LED2をDIO12(XBee_
pin 4)に接続しておきます。

XBee Wi-Fiに関しては、一切のサポートをいたしません。
より簡単に扱えるXBee ZBをお奨めいたします。
トラブル時のマニュアルは下記を参照ください。
http://www.geocities.jp/bokunimowakaru/download/xbee/README_wifi.txt

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
// #define DEBUG			// デバッグ用
// #define DEBUG_TX		// 送信パケットの確認用
// #define DEBUG_RX		// 受信パケットの確認用

#include "../libs/xbee_wifi.c"

int main(int argc,char **argv){
	char s[5];
	byte port=0;		//　↓お手持ちのXBeeモジュールのIPアドレスに変更する
	byte dev_gpio[] = {192,168,0,135};                  // 子機XBee
	byte dev_my[]   = {192,168,0,255};                  // 親機パソコン
	
	xbee_init( port );					// XBee用COMポートの初期化(引数はポート番号)
	xbee_myaddress(dev_my);				// 自分のアドレスを設定する
	xbee_gpio_init(dev_gpio);			// デバイスdev_gpioにIO設定を行うための送信
	// メイン処理
	while(1){									// 永久に受信する
		printf("input[0-2]:");
		fgets( s ,5 , stdin);
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
