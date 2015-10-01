/*********************************************************************
ポケットガイガー測定用

親機XBee側の測定用ソースコードです。

※このソースはPC用です。■■■H8では動作しません■■■

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。
XBeeライブラリはVer 1.5B以降を使用してください。

なお、Radiation-Watch.orgのソースコードを含みます。

※注意：パソコンやファンヒータなどAC電源の途絶えると故障や事故の発生する
　機器をSmart Plugに接続しないこと。
                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////
/*

Radiation-watch.orgプロジェクトは開発成果をクリエイティブ・コモンズ(CC 
BY 3.0)ライセンスの元でオープンソース化しています。以下では、ポケット
ガイガーの回路図、仕様、ソースコード、実験結果を、順次公開します。
これらは全世界において、商用・非商用を問わず自由に再配布・改変・公開す
ることができます。なお一部のコンテンツは英語またはオランダ語のみとなり
ます。

POCKET GEIGER by Radiation-Watch.org is licensed under a Creative 
Commons Attribution 3.0 Unported License.

*/

/*
	port:	port指定	IO名	ピン番号			センサーデバイス
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 放射線パルス入力（黄色）
			port= 2		DIO2	XBee_pin 18 (AD2)	 振動センサ入力（緑色）
			port= 3		DIO3	XBee_pin 17 (AD3)	 なし
*/

//#define	H3694			//重要：H3694をdefineするとH8マイコン用になります。
//#define ERRLOG			//ERRLOGをdefineするとエラーログが出力されます。

// #define XBEE_END_DEVICE		// XBeeにエンドデバイスを使用するときの設定

#include <math.h>

#ifdef H3694
	#ifndef __3694_H__
		#include <3694.h>
	#endif
	#include "../libs/lcd_h8.c"
#else
	#include "../libs/lcd_pc.c"
#endif
#include "../libs/xbee.c"

const float alpha=53.032; // cpm = uSv × alpha

int main(int argc,char **argv){
	byte i;
	byte trig=0;
//	byte dev_en = 0;	// センサー発見の有無(0:未発見)
	XBEE_RESULT xbee_result;

	time_t timer;
	struct tm *time_st;

	byte port=0;		// ↓お手持ちのXBeeモジュールのIEEEアドレスに変更する
	byte dev_sens[]   = {0x00,0x13,0xA2,0x00,0x00,0x00,0x00,0x00};	// PocketGeiger
	
	int signCount=0;	//信号のカウント回数
	int noiseCount=0;	//ノイズのカウント回数
	#ifdef XBEE_END_DEVICE
	byte signValue=0;	//前回のsign値 0 or 1
	#endif

	float cpm = 0; //現在のカウント数
	float cpmHistory[200]; //カウント回数の履歴
	byte cpmIndex=0;//カウント回数の現在配列の格納位置

	//cpm計算用の時間
	int cpmTimeSec=0;

	//出力演算用
	float cpmBuff;
	float uSvBuff;
//	float uSvdBuff;

	float min;

	// 初期化処理
	#ifndef H3694
		if( argc==2 ) port = (byte)(argv[1][0]-'0');
	#endif
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	
	// デバイス探索
	lcd_disp("Searching:PocketGeiger");
	if( xbee_atnj(30) ){				// 30秒間の参加受け入れ
		lcd_disp("Found device");
		xbee_from( dev_sens );			// 受信したアドレスをdev_sensへ格納
	//	dev_en = 1;						// sensor発見済
		xbee_gpio_init(dev_sens);		// デバイスdev_sensにIO設定を行うための送信(End Deviceは設定できるとは限らない)
	}else{
		lcd_disp("Failed:no dev.");
		exit(-1);
	}
	
	for(i=0; i<200;i++ ) cpmHistory[i]=0;

	lcd_disp("Measuring...");
	
	do{
		time(&timer);
		time_st = localtime(&timer);
	}while( (time_st->tm_sec)%6 != 0 );
	trig = time_st->tm_sec + 6 ;		// 現在時刻＋６秒
	if( trig >= 60 ) trig = 0;
	printf("%02d:%02d:%02d ---- uSv/h (--- cpm)\n",time_st->tm_hour,time_st->tm_min,time_st->tm_sec);
	
	// メイン処理
	while(1){							// 永久に受信する
	
		time(&timer);
		time_st = localtime(&timer);
	
		/* データ受信(待ち受けて受信する) */
		xbee_result.MODE = MODE_AUTO;	// 自動受信モードに設定
		xbee_rx_call( &xbee_result );	// データを受信します。(処理に50msかかります)
										// data：受信結果の代表値
										// xbee_result：詳細な受信結果
										// xbee_result.MODE：受信モード
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_RESP:				// sensorからの応答の場合に照度と温度を表示
			case MODE_GPIN:				// 周期方式でデータ受信した場合も想定
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 ){
				//	printf("gpin\n");	// テスト用
					#ifdef XBEE_END_DEVICE
						signCount++;		//放射線パルス(Low)をカウント
						if( xbee_result.GPI.PORT.D1 == signValue ){
							signCount++;	//パケットロス対策
							printf("detected packet loss\n");
						}
						signValue = xbee_result.GPI.PORT.D1;					// 現在の値を保存
					#else
						if( xbee_result.GPI.PORT.D1 == 0 ) signCount++;			//放射線パルス(Low)をカウント
					#endif
					if( xbee_result.GPI.PORT.D2 == 1 ) {
						noiseCount++;		//ノイズ(High)をカウント
						printf("detected noise\n");
					}
				}else{
					printf("gpin from=");
					for( i=0 ; i<8 ; i++ ) printf("%02X ",xbee_result.FROM[i]);
					printf("\nstatus=[%x]\n",xbee_result.STATUS);
				}
				break;
			case MODE_IDNT:				// 新しいデバイスを発見
				lcd_disp("found a new device");
				xbee_from( dev_sens );	// 受信したアドレスをdev_sensへ格納
			//	dev_en = 1;				// sensor発見済
				xbee_gpio_init(dev_sens);	// デバイスdev_sensにIO設定を行うための送信
			default:
				break;
		}
		
		if( (time_st->tm_sec >= trig && trig > 0) || (time_st->tm_sec < 54 && trig == 0) ){
			trig = time_st->tm_sec + 6 ;	// 現在時刻＋６秒
			if( trig >= 60 ) trig = 0;
			if(noiseCount == 0){				//ノイズが検出されないとき（ノイズが検出されたときは、処理をしない）
				cpmIndex++;
				if(cpmIndex >= 200){        //最後(20分後)まできたら元に戻す
					cpmIndex = 0;
				}
				//一周してきたときに次に格納する配列に値が詰まっていれば、
				//現在の合計しているカウント(変数cpm)から引いて、無かったことにする
				if(cpmHistory[cpmIndex] > 0){
					cpm -= cpmHistory[cpmIndex];
				}

				cpmHistory[cpmIndex] = signCount;
				cpm += signCount;				//カウント回数の蓄積
				
				if( cpmTimeSec >= 20*60 ){		//cpmを求めるときに使用する計測時間(最大20分)を加算
					cpmTimeSec = 20*60;
				}else{
					cpmTimeSec += 6 ;
				}
			}
			min = cpmTimeSec / 60.0;      //現在の計測時間(最大20分)
			if(min!=0){        //cpm、uSv/h、uSv/hの誤差をそれぞれ計算する
				cpmBuff = cpm / min;
				uSvBuff = cpm / min / alpha;
			//	uSvdBuff = sqrt(cpm) / min / alpha;
			}else{        //0割り算のときは、0にする
				cpmBuff = 0;
				uSvBuff = 0;
			//	uSvdBuff = 0;
			}
			//液晶への表示
			printf("%02d:%02d:%02d ",time_st->tm_hour,time_st->tm_min,time_st->tm_sec);
			printf("%.2f uSv/h ", uSvBuff );
			printf("(%.1f cpm) ", cpmBuff );
			printf("s=%d n=%d\n", signCount, noiseCount );
			
			//次のループのための変数の初期化
			signCount=0;
			noiseCount=0;
		}
	}
}
