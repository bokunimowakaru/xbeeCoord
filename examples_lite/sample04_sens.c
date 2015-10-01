/*********************************************************************
BeeBee Lite sample04_sens by 蘭
*********************************************************************/

/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
#include "../libs/xbee.c"

void set_sensor_adc(byte *dev){
	printf("Found a Device\n");
	xbee_from( dev );					// 受信したアドレスをdev_sensへ格納
	xbee_gpio_config( dev, 1 , AIN );	// 接続相手のポート1をAIN入力へ
	xbee_gpio_config( dev, 2 , AIN );	// 接続相手のポート2をAIN入力へ
	xbee_gpio_config( dev, 11 , DIN );	// Digi純正センサのバッテリ信号
}

int main(int argc,char **argv){
	byte trig=0;
	byte dev_en = 0;	// センサー発見の有無(0:未発見)
	XBEE_RESULT xbee_result;
	float value;
	byte port=0;
	byte dev_sens[8];	// センサデバイス
	
	// 初期化処理
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );					// COMポート初期化(引数はポート番号)
	
	// デバイス探索
	printf("子機ペアリングボタン押してね\n");
	if( xbee_atnj(10) ){				// 10秒間の参加受け入れ
		dev_en = 1;						// sensor発見済
		set_sensor_adc(dev_sens);
	}else printf("Failed:no devices\n");
	
	// メイン処理
	printf("Receiving\n");
	while(1){							// 永久に受信する
		if( dev_en && trig == 0){
			xbee_force( dev_sens );		// デバイスdev_sensへデータ要求(100回に1回の頻度で)
			trig = 10;
		}
		trig--;

		/* データ受信(待ち受けて受信する) */
		xbee_rx_call( &xbee_result );	// データを受信します。
		switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
			case MODE_RESP:				// sensorからの応答の場合に照度と温度を表示
				if( bytecmp( dev_sens , xbee_result.FROM ,8 ) == 0 &&
				xbee_result.STATUS == STATUS_OK ){
					// 照度測定結果
					value = xbee_sensor_result( &xbee_result,LIGHT);
					printf("%.0f Lux, ",value);
					// 温度測定結果
					value = xbee_sensor_result( &xbee_result,TEMP);
					printf("%.1f degC, ",value);
					//
					printf("%d\n",xbee_result.GPI.PORT.D11);
				}
				break;
			case MODE_IDNT:				// 新しいデバイスを発見
				dev_en = 1;				// sensor発見済
				set_sensor_adc(dev_sens);
				break;
		}
	}
}
