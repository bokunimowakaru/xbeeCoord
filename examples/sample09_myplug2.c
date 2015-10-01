/*********************************************************************
サンプルアプリ９　Myスマートプラグ２（電力センサPIC用）

このサンプルは、電力センサPICを搭載した自作の改良版スマートプラグ専用です。

自作 XBee Smart Plugを発見すると、そのデバイスから照度と温度、消費電力を得ます。
ご注意：測定値は目安です。環境によって変動します。

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
			port= 3		DIO3	XBee_pin 17 (AD3)	 電流センサー 
			
センサーデバイスの型番と情報

	電流センサー
			30Aクランプ型電流センサ SEN-11005 (sparkfun)
			ECHUN Electronic ECS1030-L72
			http://www.sparkfun.com/products/11005
			http://strawberry-linux.com/catalog/items?code=18214
			
			Turnn ratio Np:Ns=1:2000
			Current Ratio 30A/15mA
			
			PIC_ADC入力電圧(@1ch)
			
				PIC_ADCpp[mV] ＝ AC[mA]/2000*RL[Ohm]*2*sqrt(2)
			∴	AC[A] = PIC_ADCpp[mV]/(RL*sqrt(2))
				AC[W] = PIC_ADCpp[mV]/(RL/100*sqrt(2))
			
			(RL = 1kΩ)の時(@1ch)
				AC[A] = ADCpp[mV]/707.1	
				AC[W] = ADCpp[mV]/7.071		最大467W(=3300/7.071@1ch)まで
				
			(RL = 75Ω)の時(1@ch)
				AC[A] = ADCpp[mV]/106.07
				AC[W] = ADCpp[mV]/1.0607		最大3111W(=3300/1.0607@1ch)まで
				
			16 bit 入力時(@2ch) 有効ビット数は 6 + 5 bit = 11bit
			
				655536 段階 3300mV 
				
				value	= (PIC_ADC1[mV]+PIC_ADC2[mV])/3300*65536
						= (I1[mA]+I2[mA])/2[ch]/(V[V]*sqrt2)/2000*RL[Ohm]/3300*65536
				
				AC12[W] = value(16bit) * 2 * 100 * sqrt(2) * 2 / RL * 3300 / 65536
				
				(RL = 75Ω)の時(2@ch)
				AC12[W] = value(16bit) * 0.37980;
								
				*/
				#define CTSENSER_MLTPL 0.37980				// 負荷抵抗75Ω 最大3000W x 2ch
				/*
				#define CTSENSER_MLTPL 0.37980				// 負荷抵抗75Ω 最大3000W x 2ch
			
				最大 6222.3W入力時 value = 16384 より 上位2ビットが不要
			
			PWM 8 bit 出力の ADC入力時(開発中)
			
				最大 6222W時に256
				AC12[W] = PWM_value(8bit) * 2 * 100 * sqrt(2) * 2 / RL * 3300 / 65536 * 2^6
				
				XBee_value = ( 3300 * PWM_value / 256 ) * 10k / (10k + 18k) / 1200 * 1024;
				
				AC12[W] = XBee_value/3300*256/10*28*1200/1024 * 2*100*sqrt(2)*2/RL*3300/65536*64
				        = XBee_value * 6.187184325
				
				(RL = 75Ω)の時(2@ch)

				*/
				#define CTSENSER_ADC_MLTPL 6.187184325		// 負荷抵抗75Ω 電圧分割 10k/(10k+18k)
				/*
				#define CTSENSER_ADC_MLTPL 6.187184325		// 負荷抵抗75Ω 電圧分割 10k/(10k+18k)
				
				
				2090W 1580h 5504
				3767W 609
				
							
	ADC[mV] = (float)ADC /1023.0 * 1200.0
			= (float)ADC*1.17302;
			
	確認
			測定値 電力メータの表示値 =  [W}
			
			電力メータ(400 Rev/kWh)1回転時間 = 秒
			
			消費電力 = 3600(秒)÷1回転時間(秒)÷0.400(Rev/Wh)



*/

//#define	H3694			//重要：H3694をdefineするとH8マイコン用になります。
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
	byte trig=0;
	byte dev_en = 0;	// センサー発見の有無(0:未発見)
	XBEE_RESULT xbee_result;
	float value;
	unsigned int value_i;

	byte port=0;		// ↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// PIO デバイス
	
	// 初期化処理
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	
	// デバイス探索
	lcd_disp("Searching:Smart Plug");
	if( xbee_atnj(30) ){				// 30秒間の参加受け入れ
		lcd_disp("Found device");
		xbee_from( dev_sens );			// 受信したアドレスをdev_sensへ格納
		dev_en = 1;						// sensor発見済
		xbee_gpio_config( dev_sens, 1 , AIN ); // 接続相手のポート1をAIN入力へ
		xbee_gpio_config( dev_sens, 2 , AIN ); // 接続相手のポート2をAIN入力へ
		xbee_gpio_config( dev_sens, 3 , AIN ); // 接続相手のポート3をAIN入力へ
	}else{
		lcd_disp("Failed:no dev.");
		exit(-1);
	}
	wait_millisec( 1000 );
	
	// メイン処理
	lcd_disp("Receiving");
	while(1){							// 永久に受信する

		/* 親機からスマートプラグへデータを要求する場合は以下5行を使用する */
		if( dev_en && trig == 0){
//			xbee_force( dev_sens );		// デバイスdev_sensへデータ要求(100回に1回の頻度で)
			trig = 100;
		}
		trig--;

		// 参考情報
		// 電池駆動するスマートプラグの省電力化の方法として「サンプリング要求方式」と
		// 「周期サンプリング方式」があり、前者の場合は上記の５行を有効にします。
		// 周期サンプリングはATIS<時間>で設定します。時間はmsで16進数2桁までです。
		// この場合のxbee_rx_callの応答は「MODE_RESP」になります。
		// 後者の場合はこのままで動作します。この場合の応答は「MODE_GPIN」になります。
		// http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-sleep_cyclic.html

		/* データ受信(待ち受けて受信する) */
		xbee_result.MODE = MODE_AUTO;	// 自動受信モードに設定
		data = xbee_rx_call( &xbee_result );	// データを受信します。(処理に50msかかります)
										// data：受信結果の代表値
										// xbee_result：詳細な受信結果
										// xbee_result.MODE：受信モード
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_RESP:				// スマートプラグへのデータ要求に対する応答
			case MODE_GPIN:				// スマートプラグから周期方式でデータ受信する
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 ){
					// 電力測定結果
					value = (float)xbee_result.ADCIN[3] * CTSENSER_ADC_MLTPL;
					if( value < 1.0 ) value_i = 0;
					else value_i = (unsigned int)value;
					lcd_cls(); lcd_goto(LCD_ROW_1);lcd_putstr("ADC="); lcd_disp_5( (unsigned int)xbee_result.ADCIN[3] );
					lcd_goto(LCD_ROW_3); lcd_disp_5( value_i ); lcd_putstr(" Watts"); lcd_putch('\n');
				}
				break;
			case MODE_UART:
				/* シリアルデータを受信 */
				lcd_cls(); lcd_goto(LCD_ROW_1); lcd_putstr("UART[");
				lcd_disp_hex(data); lcd_putch(']');		// データ長を表示
				value = 0.;
				for(i=0;i<data;i++){
					if( xbee_result.DATA[i] >= (byte)'0' &&  xbee_result.DATA[i] <= (byte)'9' ){
						value *= 16;
						value += xbee_result.DATA[i] - (byte)'0' ;
					} else if ( xbee_result.DATA[i] >= (byte)'A' &&  xbee_result.DATA[i] <= (byte)'F' ){
						value *= 16;
						value += xbee_result.DATA[i] - (byte)'A' + 10 ;
					}
					if( xbee_result.DATA[i] >= 32 && xbee_result.DATA[i] <= 126 ) lcd_putch( xbee_result.DATA[i] );
				}
				// lcd_putch( '=' );
				// lcd_disp_5( (unsigned int) value );
				// lcd_putch( '\n' );
				value *= CTSENSER_MLTPL;
				if( value < 0.0 ) value_i = 0;
				else value_i = (unsigned int)value;
				lcd_goto(LCD_ROW_3); lcd_disp_5( value_i ); lcd_putstr(" Watts"); lcd_putch('\n');
				break;
			default:
				break;
		}
		// lcd_goto(LCD_ROW_4); lcd_disp_hex(trig);
	}
}
