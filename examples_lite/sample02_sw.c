/*********************************************************************
BeeBee Lite sample02_sw by 蘭
*********************************************************************/

/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

#include "../libs/xbee_lite.c"

int main(int argc,char **argv){
	byte i;
	byte data;
	XBEE_RESULT xbee_result;

	byte port=0;
	byte dev_gpio[8];
	
	// 初期化処理
	if( argc==2 ) port = (byte)atoi(argv[1]);
	xbee_init( port );
	xbee_atnj( 0xFF );
	printf("子機ペアリングボタン押してね\n");
	
	// メイン処理
	while(1){
		data = xbee_rx_call( &xbee_result );
		if( xbee_result.MODE == MODE_GPIN){
			printf("受信値＝");
			if( data & 0b00001000 ){
				printf("1");
			}else{
				printf("0");
			}
			if( data & 0b00000100 ){
				printf("1");
			}else{
				printf("0");
			}
			if( data & 0b00000010 ){
				printf("1");
			}else{
				printf("0");
			}
			printf(" 16進数＝%02X\n",data);
		}
		// ペアリング
		if( xbee_result.MODE == MODE_IDNT){
			printf("子機みつけた\n");
			for(i=0;i<8;i++) dev_gpio[i]=xbee_result.FROM[i];
			xbee_gpio_init(dev_gpio);
		}
	}
}
