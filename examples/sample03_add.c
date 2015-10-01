/*********************************************************************
サンプルアプリ３　スイッチ

新しいXBee子機のコミッションボタンDIO0(XBee_pin 20)を押下する(信号レベ
ルがH→L→Hに推移する)と、子機から参加要求信号が発行され、本プログラム
が動作する親機が子機を発見し、また子機のGPIOを設定します。
その後、子機のスイッチ2～4(Port1～3)を押した時に、スイッチ状態を画面に
表示します。
子機が複数台あっても、それぞれの状態を受信することが出来ます。

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
*/





#include "../libs/xbee.c"

int main(int argc,char **argv){
	byte i;
	XBEE_RESULT xbee_result;

	byte port=0;
	byte dev_gpio[8];
	
	// 初期化処理
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	xbee_atnj( 0xFF );					// デバイスを常に参加受け入れ(テスト用)
	
	// メイン処理
	printf("Receiving\n");
	while(1){							// 永久に受信する
		/* データ受信(待ち受けて受信する) */
		xbee_result.MODE = MODE_AUTO;			// 自動受信モードに設定
		xbee_rx_call( &xbee_result );	// データを受信します。
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_GPIN:				// GPIO入力を検出(リモート機のボタンが押された)
				// スイッチの状態を表示
				for( i=1;i<=3;i++ ){
					printf("SW%d:%d ",i,(xbee_result.GPI.BYTE[0]>>i)&0x01);
				}
				// 送信元アドレスを表示
				printf("from:");
				for( i=0;i<4;i++ ) printf("%02X",xbee_result.FROM[i] );
				printf(".");
				for( i=4;i<8;i++ ) printf("%02X",xbee_result.FROM[i] );
				printf("\n");
				break;
			case MODE_IDNT:				// 新しいデバイスを発見
				for(i=0;i<8;i++) dev_gpio[i]=xbee_result.FROM[i];
										// 発見したアドレスをdev_gpioに読み込み
				xbee_gpio_init( dev_gpio );
				printf("found a new device\n");
				break;
			default:
				break;
		}
	}
}
