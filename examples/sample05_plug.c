/*********************************************************************
サンプルアプリ５　スマートプラグ

XBee Smart Plug(Digi 純正品)を発見すると、そのデバイスから照度と消費
電力を得ます。暗くなるとSmart Plugに接続した機器の電源が切れます。

新しいXBee子機のコミッションボタンDIO0(XBee_pin 20)を押下する(信号レベ
ルがH→L→Hに推移する)と、子機から参加要求信号が発行されて、PCまたは
H8親機が子機を発見し、「found a device」と表示します。

ただし、複数のZigBeeネットワークに対応するために、ペアリングは起動後の
30秒間しか受け付けません。PC/H8親機を起動して、コミッションボタンを
押してペアリングを実施しておき、再度、コミッションボタンでデバイス発見
します。デバイス発見は30秒のペアリング期間後でも行えます。

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

※注意：パソコンやファンヒータなどAC電源の途絶えると故障や事故の発生する
　機器をSmart Plugに接続しないこと。
                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
	port:	port指定	IO名	ピン番号			センサーデバイス
			port= 0		DIO0	XBee_pin 20 (Cms)	 SW1 Commision
			port= 1		DIO1	XBee_pin 19 (AD1)	 照度センサー
			port= 2		DIO2	XBee_pin 18 (AD2)	 温度センサー
			port= 3		DIO3	XBee_pin 17 (AD3)	 AC電流センサー
*/





#include "../libs/xbee.c"

void set_plug_adc(byte *dev){
	printf("Found a Device\n");
	xbee_from( dev );					// 受信したアドレスをdev_sensへ格納
	xbee_gpio_config( dev, 1 , AIN );	// 接続相手のポート1をAIN入力へ
	xbee_gpio_config( dev, 2 , AIN );	// 接続相手のポート2をAIN入力へ
	xbee_gpio_config( dev, 3 , AIN );	// 接続相手のポート3をAIN入力へ
}

int main(int argc,char **argv){
	byte trig=0;
	byte dev_en = 0;	// センサー発見の有無(0:未発見)
	XBEE_RESULT xbee_result;
	float value;
	byte port=0;
	byte dev_sens[8];
	
	// 初期化処理
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	
	// デバイス探索
	printf("Searching:Smart Plug\n");
	if( xbee_atnj(30) ){				// 30秒間の参加受け入れ
		dev_en = 1;						// sensor発見済
		set_plug_adc(dev_sens);			// 設定
	}else printf("Failed:no devices\n");

	// メイン処理
	printf("Receiving\n");
	while(1){							// 永久に受信する
		if( dev_en && trig == 0){
			xbee_force( dev_sens );		// デバイスdev_sensへデータ要求(100回に1回の頻度で)
			trig = 100;
		}
		trig--;
		/* データ受信(待ち受けて受信する) */
		xbee_rx_call( &xbee_result );	// データを受信します。(処理に50msかかります)
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_RESP:				// sensorからの応答の場合に照度と温度を表示
			case MODE_GPIN:				// 周期方式でデータ受信した場合も想定
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 &&
				xbee_result.STATUS == STATUS_OK ){
					// 照度測定結果
					value = xbee_sensor_result( &xbee_result,LIGHT);
					printf("%.0f Lux, ",value);
					// 暗いときは電源を切り、明るいときは電源を入れる
					if( value < 1.0 ){
						xbee_gpo(dev_sens , 4 , 0 );	// Turns outlet off (Port 4 = 0)
					}else{
						xbee_gpo(dev_sens , 4 , 1 );	// Turns outlet on (Port 4 = 1)
					}
					// 温度測定結果 (当方で確認したところ7.12℃の高めが表示されますが、Digi社の仕様です。)
					value = xbee_sensor_result( &xbee_result,TEMP);
					printf("%.1f degC, ",value);
					// 電力測定結果
					value = xbee_sensor_result( &xbee_result,WATT);
					printf("%.1f Watts\n",value);
				}
				break;
			case MODE_IDNT:				// 新しいデバイスを発見
				dev_en = 1;				// sensor発見済
				set_plug_adc(dev_sens);			// 設定
				break;
		}
	}
}
