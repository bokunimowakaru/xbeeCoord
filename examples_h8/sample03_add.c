/*********************************************************************
サンプルアプリ３　スイッチ
子機のスイッチ2～4(Port1～3)を押した時に、その状態を液晶に表示します。
新しいデバイスを発見すると、そのデバイスのGPIOの設定を変更します。
（同時に２つ以上のデバイスのスイッチ状態を検出できます）
新しいXBee子機のコミッションボタンDIO0(XBee_pin 20)を押下する(信号レベ
ルがH→L→Hに推移する)と、子機から参加要求信号が発行されて、Arduino親機
が子機を発見し、「found a device」と表示します。
始めは、一度、ペアリングをしてから、再度コミッションボタンを押す必要が
あります。

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
	byte data;
	XBEE_RESULT xbee_result;

	byte port=0;		//　↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev_gpio[]   = {0x00,0x13,0xA2,0x00,0x40,0x30,0xC1,0x6F};	// PIO デバイス
	
	// 初期化処理
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	xbee_atnj( 0xFF );					// デバイスを常に参加受け入れ(テスト用)
	xbee_gpio_init(dev_gpio);			// デバイスdev_gpioにIO設定を行うための送信
	
	// メイン処理
	lcd_disp("Receiving");
	while(1){							// 永久に受信する
		/* データ受信(待ち受けて受信する) */
		xbee_result.MODE = MODE_AUTO;			// 自動受信モードに設定
		data = xbee_rx_call( &xbee_result );	// データを受信します。
										// data：受信結果の代表値
										// xbee_result：詳細な受信結果
										// xbee_result.MODE：受信モード
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_GPIN:				// GPIO入力を検出(リモート機のボタンが押された)
				lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("SW ");
				for( i=1;i<=3;i++ ){					// スイッチの状態を表示
					lcd_disp_1(i); lcd_putch(':');
					lcd_disp_1( (data>>i)&0x01 ); lcd_putch(' ');
				}
				/* 以下は詳細な受信結果の例 */
				lcd_goto(LCD_ROW_2); lcd_putstr("D ");	// GPIO入力値(デジタル)を表示
				lcd_disp_bin( xbee_result.GPI.BYTE[1] ); lcd_putch(' ');
				lcd_disp_bin( xbee_result.GPI.BYTE[0] ); 
				lcd_goto(LCD_ROW_3); lcd_putstr("A ");	// ADC入力値(アナログ)を表示
				for( i=0;i<3;i++ ){ lcd_disp_5( xbee_result.ADCIN[i] ); // 但し、要設定
					lcd_putch(' ');	}
				lcd_goto(LCD_ROW_4); lcd_putstr("@ ");	// 送信元アドレスを表示
				for( i=0;i<4;i++ ) lcd_disp_hex( xbee_result.FROM[i] );
				lcd_putch(' ');
				for( i=4;i<8;i++ ) lcd_disp_hex( xbee_result.FROM[i] );
				lcd_putch('\n');
				break;
			case MODE_IDNT:				// 新しいデバイスを発見
				xbee_from( dev_gpio );	// 発見したアドレスをdev_gpioに読み込み
				xbee_gpio_init( dev_gpio );
				lcd_disp("found a new device");
				break;
			default:
				break;
		}
	}
}
