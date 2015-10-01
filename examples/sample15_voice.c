/*********************************************************************
サンプルアプリ15 音声合成出力

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

#include "../libs/xbee.c"
#include "../libs/kbhit.c"
#include <ctype.h>

#define VOICE_NUM	32					// 音声データ長

byte dev[8];							// XBee子機のアドレス

void print_timer(){						// 時計用
	time_t timer;
	struct tm *time_st;
	
	time(&timer);
	time_st = localtime(&timer);
	printf("%4d/%02d/%02d %02d:%02d:%02d ",
		time_st->tm_year+1900,
		time_st->tm_mon+1,
		time_st->tm_mday,
		time_st->tm_hour,
		time_st->tm_min,
		time_st->tm_sec );
}

int main(int argc,char **argv){
	char voice[VOICE_NUM]="konnitiwa'";
	char voice_out[VOICE_NUM+1];
	byte voice_len=0;
	byte dev_en=0;
	byte len;
	byte i,j;
	byte trig=0;
	XBEE_RESULT xbee_result;

	byte port=0;
	
	// setup処理
	print_timer();printf("setup\tXBee Init\n");
	if( argc==2 ) port = (byte)(argv[1][0]-'0');
	xbee_init( port );			// XBee用COMポートの初期化(引数はポート番号)
	xbee_atnj(0xff);

	// loop 処理
	while(1){
		if(trig==0){
			if( dev_en == 0 ){
				printf("Waiting for XBee.\n");
			}else if( dev_en == 1 ){
				printf("Waiting for Aques Talk.\n");
				xbee_uart(dev,"?konnitiwa'\r");
			}
			trig=0xFF;
		}
		trig--;
		if ( kbhit() ){
			j = (byte)getchar();
			if(j == (byte)'\n'){
				printf("Send '%s'\n",voice);
				sprintf(voice_out,"%s\r",voice);
				xbee_uart(dev,voice_out);
				voice_len=0;
			}else if(j >= 0x20 && voice_len < VOICE_NUM-2){
				voice[voice_len]=(char)j;
				voice_len++;
				voice[voice_len]='\0';
			}
		}
		
		len=xbee_rx_call( &xbee_result );	// データを受信します。
		
		// 受信時の処理
		if(xbee_result.MODE == MODE_IDNT){
			printf("Found XBee IDNT. ( ");
			for(i=0;i<8;i++){
				dev[i]=xbee_result.FROM[i];
				printf("%02X ",dev[i] );
			}
			printf(")\n");
			xbee_ratd_myaddress(dev);		// 親機のアドレスを子機に設定
			dev_en=1;
		}
		if(bytecmp(dev, xbee_result.FROM ,8 )==0 && xbee_result.MODE == MODE_UART ){
			if(	len==1 && xbee_result.DATA[0]==(byte)'>'){
				printf("Voice Device Enable.\n");
				dev_en=2;
			}else{
				for(i=0;i<len;i++) if( isprint( xbee_result.DATA[i]) ) printf("%c",(char)xbee_result.DATA[i] );
				printf(" (");
				for(i=0;i<len;i++) printf("%02X ",xbee_result.DATA[i] );
				printf(")\n");
			}
		}
	}
}
