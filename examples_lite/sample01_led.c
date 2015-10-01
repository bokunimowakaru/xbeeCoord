/*********************************************************************
BeeBee Lite sample01_led by 蘭
*********************************************************************/

/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/


#include "../libs/xbee_lite.c"

int main(int argc,char **argv){
	byte i;
	char s[3];
	byte port=0;
	byte dev_gpio[8];
	XBEE_RESULT xbee_result;
	
	// 初期化処理
	if( argc==2 ) port = (byte)atoi(argv[1]);
	xbee_init( port );
	xbee_atnj( 0xFF );
	
	// ペアリング
	printf("子機ペアリングボタン押してね\n");
	do{
		xbee_rx_call(&xbee_result);
	}while(xbee_result.MODE != MODE_IDNT);
	for(i=0;i<8;i++) dev_gpio[i]=xbee_result.FROM[i];
	
	// メイン処理
	printf("キーボード数字で子機LEDコントロール\n");
	while(1){
		printf("input[0-2]:");
		gets( s );
		switch( s[0] ){
			case '0':
				xbee_gpo(dev_gpio,12,1);		// GPOポート12をHレベル(3.3V)へ
				xbee_gpo(dev_gpio,11,1);		// GPOポート11をHレベル(3.3V)へ
				break;
			case '1':
				xbee_gpo(dev_gpio,12,0);		// GPOポート12をLレベル(0V)へ
				break;
			case '2':
				xbee_gpo(dev_gpio,11,0);		// GPOポート11をLレベル(0V)へ
				break;
		}
	}
}
