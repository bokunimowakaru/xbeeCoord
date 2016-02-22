/*********************************************************************
AT／リモートATコマンド解析ツール for PC
(リモートATコマンドの応答値確認用) 

■■■■ PC ＆ Raspberry Pi専用 ■■■■■

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2015 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

/*********************************************************************
AT／リモートATコマンド解析ツール説明書

●概要●

	APIモードの親機のXBeeにATコマンドを送ったり、子機などからの受信デー
	タを表示するためのテスト用プログラムです。

	XBeeコマンドの詳細を解析するために作成しました。(xbee.cの開発用)

●起動方法●

	Cygwinでexeファイルのあるディレクトリ内でCOM番号(数字のみ)を指定して
	起動します。
		
		./xbee_test [COM番号]
		
	数秒するとATプロンプト待ちとなります。
		
		AT>

●ＡＴコマンド入力●

	ここでatコマンドが入力できます。例えばネットワークアドレスを確認する
	には「atop」と入力して[return]を押すとATコマンドの応答が表示されます。
		
		AT>atop
		
	キーボードの入力中にデータの受信があると(タイミングによって)入力した
	文字が表示されない場合がありますが、正しく入力されています。
	
●リモートＡＴコマンド入力●

	他のZigBee(子機)にリモートATコマンドを送り、その応答を受信することが
	できます。
	まずは、子機のアドレスを登録するためにリモート側の子機のコミッション
	ボタンを押します。(押さないと相手先がブロードキャストになります。)
	するとPCに下記のようなメッセージが表示されます。
	
		--------------------
		recieved IDNT
		--------------------
		from   :0013A200 11223344
		dev    :0013A200 11223344
		status :02
	
	リモート先のDIO(GPIO)の値を読み取るには「ISコマンド」を使用します。
	通常のATコマンド「at」の代わりにリモートAT「rat」を用意しましたので、
	「rat」+「is」で「ratis」と入力します。
		
		AT>ratis
		
	相手(子機)が即座に応答した場合は、受信したパケットデータが以下のよう
	に表示されます。
	
		xbee_tx:RATIS[0]
		xbee_rx:RATIS[29]
		data[0] = 0x7e '~' (126)
		data[1] = 0x 0 ' ' (0)
		～省略～
		data[15] = 0x49 'I' (73)
		data[16] = 0x53 'S' (83)
		～省略～
		data[26] = 0x 1 ' ' (1)
		data[27] = 0xe3 ' ' (227)
		data[28] = 0x4d 'M' (77)
		checksum = 0x5a     (90)
		
		--------------------
		recieved RAT_RESP
		--------------------
		from   :0013A200 11223344
		id     :09
		status :00
		gpi1-0 :00000000 00000000
		adc1-3 :551 534 469
		done
	
	相手がEnd Device等ですぐの応答が無い場合は、応答なしのエラーが表示さ
	れますが、応答があるとAPI受信機能で以下のように表示されます。
	
		--------------------
		recieved RAT_RESP
		--------------------
		from   :0013A200 11223344
		id     :16
		status :04
		gpi1-0 :00000001 00000001
		adc1-3 :44803 0 65535
	
	こちらからコマンドを送らなくても、何らかのデータを受信すると受信した
	内容について概要を表示します。
	シリアルデータを受信した場合、以下のような表示となります。
	
		--------------------
		recieved UART
		--------------------
		from   :0013A200 11223344
		id     :00
		status :41
		length :[08]
		data   :ABCDEFGH

●リモートＡＴコマンドでDIO出力●

	リモートATコマンドでGPIOへ出力DIO4(XBee_pin=16)をＨレベルに設定する
	には、「rat」+「d4」+「=05」で「ratd4=05」と入力します。
	
		AT>ratd4=05
	
	また、Ｌレベルに設定するには「ratd4=04」と入力します。
	
		AT>ratd4=04
	
	設定値「05」がＨレベル、「04」がＬレベルとなります。
	
*/

//#define DEBUG			//DEBUGをdefineするとライブラリの動作ログが出力されます。
//#define DEBUG_TX		//送信データ表示
//#define		ERRLOG
#define		XBEE_ERROR_TIME

#include "../libs/xbee.c"
#include "../libs/kbhit.c"

#define AT_LEN	32
#define FO_LEN	32		// ファイル送信の文字列長

void print_status(const byte in ){
	/*
	#define		STATUS_OK		0x00		// ATコマンドの結果がOK
	#define		STATUS_ERR		0x01		// ATコマンドの結果がERROR
	#define		STATUS_ERR_AT	0x02		// 指定されたATコマンドに誤りがある
	#define		STATUS_ERR_PARM	0x03		// 指定されたパラメータに誤りがある
	#define		STATUS_ERR_AIR	0x04		// リモートATコマンドの送信の失敗(相手が応答しない)
	*/
	
	switch( in ){
		case STATUS_OK:
			printf(" OK");
			break;
		case STATUS_ERR:
			printf(" ERROR");
			break;
		case STATUS_ERR_AT:
			printf(" AT COMMAND ERROR");
			break;
		case STATUS_ERR_PARM:
			printf(" PARAMETER ERROR");
			break;
		case STATUS_ERR_AIR:
			printf(" COMMUNICATION FAILED");
			break;
		default:
			printf(" UNKNOWN ERROR");
			break;
	}
}

void print_packet(const byte i){
	switch( i){
		case 0x01:	printf("Packet Acknowledged\n");	break;
		case 0x02:	printf("Packet was a broadcast packet\n");	break;
		case 0x20:	printf("Packet encrypted with APS encryption\n");	break;
		case 0x21:	printf("Encrypted Acknowledged packet\n");	break;
		case 0x22:	printf("Encrypted broadcast packet\n");	break;
		case 0x41:	printf("Acknowledged from an end device\n");	break;
		case 0x42:	printf("Broadcast from an end device\n");	break;
		case 0x60:	printf("Encrypted from an end device\n");	break;
		case 0x61:	printf("Encrypted Ack from an end device\n");	break;
		case 0x62:	printf("Encrypted Broadcast from end device\n");	break;
		default: printf("Recieved Unknown Packet\n"); break;
	}
}

void print_atai(const byte i){
	switch(i){
		case	0x00:	printf("OK (Successfully formed or joined a network).\n"); break;
		case	0x21:	printf("Scan found no PANs\n"); break;
		case	0x22:	printf("Scan found no valid PANs based on current SC and ID settings\n"); break;
		case	0x23:	printf("Valid Coordinator or Routers found, but they are not allowing joining\n"); break;
		case	0x24:	printf("No joinable beacons were found\n"); break;
		case	0x25:	printf("Unexpected state, node should not be attempting to join at this time\n"); break;
		case	0x27:	printf("Node Joining attempt failed (typically due to incompatible security settings)\n"); break;
		case	0x2A:	printf("Coordinator Start attempt failed\n"); break;
		case	0x2B:	printf("Checking for an existing coordinator\n"); break;
		case	0x2C:	printf("Attempt to leave the network failed\n"); break;
		case	0xAB:	printf("Attempted to join a device that did not respond.\n"); break;
		case	0xAC:	printf("Secure join error - network security key received unsecured\n"); break;
		case	0xAD:	printf("Secure join error - network security key not received\n"); break;
		case	0xAF:	printf("Secure join error - joining device does not have the right preconfigured link key\n"); break;
		case	0xFF:	printf("Scanning for a ZigBee network\n"); break;
		default:printf("Unknown indication\n"); break;
	}
}

void print_atvr(const byte i){
	switch(i){
		case	ZB_TYPE_COORD:	printf("ZB_TYPE_COORD\n"); break;
		case	ZB_TYPE_ROUTER:	printf("ZB_TYPE_ROUTER\n"); break;
		case	ZB_TYPE_ENDDEV:	printf("ZB_TYPE_ENDDEV\n"); break;
		case	XB_TYPE_WIFI10:	printf("XB_TYPE_WIFI10\n"); break;
	}
}

byte hex2byte(char *in){
	byte ret = 0x00;
	byte i;
	for(i=0;i<=1;i++){
		if( 		in[i] >= 'A' && in[i] <= 'F') in[i] += 10 - 'A';
		else if(	in[i] >= 'a' && in[i] <= 'f') in[i] += 10 - 'a';
		else in[i] -= '0';
	}
	ret= 16 * in[0] + in[1];
	return(ret);
}

char byte2a(byte in, byte i){
	char ret;
	if(i) in /= 16;
	ret = (char)(in%16);
	if(ret<10) ret += '0';
	else ret += 'A' -10;
	return( ret );
}

int main(int argc,char **argv){
	byte i,j,kb,rx,ret=1;
	char c;
	char at[AT_LEN];
	char at_bk[AT_LEN];
	byte data[API_SIZE];
	byte at_len;
	byte val_len;		// ATコマンド内の値の長さ
	byte res_len=0;		// 応答値の長さ
	
	XBEE_RESULT xbee_result;
	byte return_MODE = 0x00;
	
	byte err,retry;
	byte r_dat = 10;	// AT=0、RAT=10、TX=未定
	byte r_at = 1;		// AT=0、RAT=1
	unsigned int wait_add = 0;
	byte port=0;
//	byte coord[]  	=	 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	byte dev[]  	=	 {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};
	byte dev_tmp[]  =	 {0x00,0x13,0xA2,0x00,0x40,0x00,0x00,0x00};
	
	/* ZCL */
	byte zcl_dest[2]={0xFF,0xFE};
	byte zcl_sep	=0x00;
	byte zcl_dep	=0x00;
	byte zcl_clid[2]={0x00,0x05};
	byte zcl_prid[2]={0x00,0x00};
	byte zcl_radi	=0x00;
	byte zcl_fc		=0x08;
	byte zcl_seq	=0x01;
	byte zcl_command=0x00;	// read
	byte zcl_att[3] ={0x00,0x00,0x00};	// XB_AT_SIZEが32の時は31文字なので最長3バイト
	byte zcl_att_len=0;					// zcl_attの長さ ※00値をとるので必ず使うこと
	
	FILE *fp;
	char filename[AT_LEN];
	char s[FO_LEN];
		
	// 初期化処理
	lcd_disp("Initializing");
	if( argc==2 ){
		if( atoi(argv[1]) < 0 ){
			port = 0x9F + (byte)(-atoi(argv[1])) ;
		}else  if( ( argv[1][0]=='b' || argv[1][0]=='B' )&& argv[1][1]!='\0' ){
			port = 0xB0 + ( argv[1][1] - '0');
		}else  if( ( argv[1][0]=='a' || argv[1][0]=='A' )&& argv[1][1]!='\0' ){
			port = 0xA0 + ( argv[1][1] - '0');
		}else port = (byte)(atoi(argv[1]));
	}
	xbee_init( port );					// XBee用COMポートの初期化(引数はポート番号)
	xbee_atnj( 0xFF );					// デバイスを常に参加受け入れ(テスト用)
	printf("Press 'h'+Enter to help, 'q!'+Enter to quit.\n");
	
	// メイン処理
	while(ret){									// 永久に受信する
		printf("AT>");
		/* キーボードからの入力 */
		kb=0; c=0x00;
		for(i=0;i<AT_LEN;i++) at[i]=0x00;
		do{
			if ( kbhit() ){
				c = (char)getchar();
				//	printf("\n[%c]=%02X\n",c,(byte)c);	// キー確認用(OS依存)
				if( (byte)c >= 0x20 && (byte)c < 0x7E ){
					at[kb] = c;
					kb++;
				}else if( (byte)c == 0x7E){	// DELキー
					if( kb >= 3 ){
						kb -= 3;			// 5B 33 7E
					//	printf("\b \b");
					}else {
						kb=0;
					}
					for(i=kb;i<AT_LEN;i++) at[i]=0x00;	// 終端を見ずにAT解析しているので必要
					printf("\nAT>%s",at);
				}else if( (byte)c == 0x7F){	// TeraTerm
					if( kb > 0 ){
						kb -= 1;
					}else {
						kb=0;
					}
					for(i=kb;i<AT_LEN;i++) at[i]=0x00;	// 終端を見ずにAT解析しているので必要
					printf("\nAT>%s",at);
				}
			}
			
			/* データ受信(待ち受けて受信する) */
			xbee_result.MODE = MODE_AUTO;	// 自動受信モードに設定
			rx = xbee_rx_call( &xbee_result );	// データを受信します。(処理に50msかかります)
											// rx：受信結果の代表値
											// xbee_result：詳細な受信結果
											// xbee_result.MODE：受信モード
			switch( xbee_result.MODE ){		// 受信したデータの内容(MODE値)に応じて
				case 0x00:
					break;
				case MODE_RESP:	
					printf("\n--------------------\nrecieved RAT_RESP\n--------------------\n");
					printf("from   :");
					for(i=0;i<4;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( ' ' );
					for(i=4;i<8;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( '\n' );
					printf("id     :%02X\n", xbee_result.ID);
					printf("status :%02X", xbee_result.STATUS);
					print_status( xbee_result.STATUS );
					printf("\n");
					if( xbee_result.AT[0]=='I' && xbee_result.AT[1]=='S' ){	// ATISの応答の受信
						printf("gpi1-0 :");
						lcd_disp_bin( xbee_result.GPI.BYTE[1] ); printf(" ");
						lcd_disp_bin( xbee_result.GPI.BYTE[0] ); printf("\n");
						printf("adc1-3 :%d %d %d\n", xbee_result.ADCIN[1], xbee_result.ADCIN[2],xbee_result.ADCIN[3] );
					}
					if( xbee_result.ADCIN[0] != 0 )printf("battery:%d mV\n", xbee_result.ADCIN[0] );
					printf("\nAT>%s",at);
					break;
				case MODE_BATT:	
					printf("\n--------------------\nrecieved RAT_BATT\n--------------------\n");
					printf("from   :");
					for(i=0;i<4;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( ' ' );
					for(i=4;i<8;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( '\n' );
					printf("id     :%02X\n", xbee_result.ID);
					printf("status :%02X", xbee_result.STATUS);
					print_status(xbee_result.STATUS);
					printf("\n");
					printf("BATT   :");
					lcd_disp_bin( xbee_result.GPI.BYTE[0] ); printf(" ");
					lcd_disp_bin( xbee_result.GPI.BYTE[1] ); printf("\n");
					printf("battery:%d mV\n", xbee_result.ADCIN[0] );
					printf("\nAT>%s",at);
					break;
				case MODE_RES:
					printf("\n--------------------\nrecieved AT_RESP\n--------------------\n");
					printf("status :%02X", xbee_result.STATUS);
					print_status(xbee_result.STATUS);
					printf("\n");
					printf("\nAT>%s",at);
					break;
				case MODE_UART:
					/* シリアルデータを受信 */
					printf("\n--------------------\nrecieved UART\n--------------------\n");
					printf("from   :");
					for(i=0;i<4;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( ' ' );
					for(i=4;i<8;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( '\n' );
					printf("status :%02X ", xbee_result.STATUS);
					print_packet(xbee_result.STATUS);
					bytecpy(dev_tmp, xbee_result.FROM, 8);	// アドレスをdev_tmpに読み込み
					printf("length :%d (0x%02X)\n", rx , rx);
					printf("data   :");
					for(i=0;i<rx;i++){
						if( xbee_result.DATA[i] > 0x20 ) lcd_putch( xbee_result.DATA[i] );
						else lcd_putch( '/' );
					}
					lcd_putch( '\n' );
					printf("\nAT>%s",at);
					break;
				case MODE_UAR2:
					printf("\n--------------------\nrecieved UAR2 AO=1\n--------------------\n");
					printf("from   :");
					for(i=0;i<4;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( ' ' );
					for(i=4;i<8;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( ' ' );
					for(i=0;i<2;i++) lcd_disp_hex( xbee_result.SHORT[i] ); lcd_putch( '\n' );
					printf("endpt  :%02X to %02X\n", xbee_result.ZCL[0],xbee_result.ZCL[1]);
					printf("Cluster:%02X%02X\n", xbee_result.ZCL[2],xbee_result.ZCL[3]);
					printf("Profile:%02X%02X\n", xbee_result.ZCL[4],xbee_result.ZCL[5]);
					printf("status :%02X ", xbee_result.STATUS);
					print_packet(xbee_result.STATUS);
					printf("data   :");
					for(i=0;i<rx;i++){
						printf("%02X ",xbee_result.DATA[i] );
					}
					lcd_putch( '\n' );
					printf("\nAT>%s",at);
					break;
				case MODE_GPIN:
					printf("\n--------------------\nrecieved GPIO input\n--------------------\n");
					printf("from   :");
					for(i=0;i<4;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( ' ' );
					for(i=4;i<8;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( '\n' );
					printf("status :%02X\n", xbee_result.STATUS);
					// lcd_disp_bin( rx );	// rxに入った値をバイナリで表示
					// lcd_putch( '\n' );
					printf("gpi1-0 :");
					lcd_disp_bin( xbee_result.GPI.BYTE[1] ); printf(" ");
					lcd_disp_bin( xbee_result.GPI.BYTE[0] ); printf("\n");
					printf("adc1-3 :%d %d %d\n", xbee_result.ADCIN[1], xbee_result.ADCIN[2],xbee_result.ADCIN[3] );
					if( xbee_result.ADCIN[0] != 0 )printf("battery:%d mV\n", xbee_result.ADCIN[0] );
					printf("\nAT>%s",at);
					break;
				case MODE_SENS:
					printf("\n--------------------\nrecieved from 1-wire\n--------------------\n");
					printf("status :%02X ", xbee_result.STATUS);
					print_packet(xbee_result.STATUS);
					printf("\nAT>%s",at);
					break;
				case MODE_IDNT:				// 新しいデバイスを発見
					printf("\n--------------------\nrecieved IDNT\n--------------------\n");
					printf("from   :");
					for(i=0;i<4;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( ' ' );
					for(i=4;i<8;i++) lcd_disp_hex( xbee_result.FROM[i] ); lcd_putch( ' ' );
					for(i=0;i<2;i++) lcd_disp_hex( xbee_result.SHORT[i] ); lcd_putch( ' ' );
					for(i=0;i<2;i++) lcd_disp_hex( xbee_result.DATA[i+15-15] ); lcd_putch( '\n' );
					bytecpy(dev, xbee_result.FROM, 8);	// 発見したアドレスをdevに読み込み
					printf("network:");
					for(i=0;i<4;i++) lcd_disp_hex( xbee_result.DATA[i+17-15] ); lcd_putch( ' ' );
					for(i=4;i<8;i++) lcd_disp_hex( xbee_result.DATA[i+17-15+4] ); lcd_putch( '\n' );
					printf("type   :%02X ",rx);
					print_atvr(rx);
					printf("Node ID:%02X%02X\n",xbee_result.DATA[25-15],xbee_result.DATA[26-15]);
					printf("Parent :%02X%02X\n",xbee_result.DATA[27-15],xbee_result.DATA[28-15]);
					printf("Event  :%02X ",xbee_result.DATA[30-15]);
					switch(xbee_result.DATA[30-15]){
						case 0x01:	printf("Commissiong Pushbutton Event\n");	break;
						case 0x02:	printf("Joining Event Occurred\n");	break;
						case 0x03:	printf("Power Cycle Event Occurred\n");	break;
						default: printf("Unknown\n"); break;
					}
					printf("status :%02X ", xbee_result.STATUS);
					print_packet(xbee_result.STATUS);
										
				//	printf("xbee_ratd_myaddress (0x%02X)\n", xbee_ratd_myaddress(dev));
					// 見つけたデバイスに本機のアドレスを（宛先として）設定する
					xbee_atd(dev);	// 本モジュールの送信先ATDH/Lにdevのアドレスを設定する
					
					printf("\nAT>%s",at);
					break;
				case MODE_MODM:
					printf("\n--------------------\nrecieved MODEM Status\n--------------------\n");
					printf("status :%02X ", xbee_result.STATUS);
					switch( xbee_result.STATUS ){
						case 0x00:	printf("Hardware Reset\n");	break;
						case 0x01:	printf("Watchdog timer reset\n");	break;
						case 0x02:	printf("Joined network\n");	break;
						case 0x03:	printf("Disassociated\n");	break;
						case 0x06:	printf("Coordicator started\n");	break;
						default: printf("Unknown\n"); break;
					}
					printf("\nAT>%s",at);
					break;
				default:
					printf("\n--------------------\nrecieved UNKNOWN\n--------------------\n");
					printf("mode   :%02X\n", return_MODE );
					printf("status :%02X ", xbee_result.STATUS);
					print_packet(xbee_result.STATUS);
					printf("\nAT>%s",at);
					break;
			}
			if( strcmp(at,"---")==0 ){
				strcpy(at,"Q!");
				c='\n';
				kb=2;
			}
		}while( c != '\n' && kb < AT_LEN );
		
		// test
	//	printf("at(%d,%d)=[%s]\n",strlen(at),kb,at);
		printf("\n");
		
		if( at[0] == '!' ) for(i=0;i<AT_LEN;i++) at[i] = at_bk[i];
		else for(i=0;i<AT_LEN;i++) at_bk[i] = at[i];
		for(i=0;i<(API_SIZE-1);i++) data[i] = 0x00; data[API_SIZE-1] = 0x00;
		at_len=0;
		for(i=0;i<3;i++) if( at[i] >='a' && at[i] <= 'z' ) at[i] -= ('a'-'A');
		
		if( at[0] == 'Q' && at[1] == '!' ){
			xbee_log(3,"Quit ",strlen(at)-2);
			printf("\nBye!\n");
			ret=0;
		}else if( at[0] == 'A' && at[1] == 'D' ){
			if( at[2]=='C' ){
				xbee_log(3,"Set ADC ",strlen(at)-2);
				printf("adc 1 (0x%02X)\n",xbee_gpio_config( dev, 1 , AIN ));
				printf("adc 2 (0x%02X)\n",xbee_gpio_config( dev, 2 , AIN ));
				printf("adc 3 (0x%02X)\n",xbee_gpio_config( dev, 3 , AIN ));
			}else{
				if( at[2]=='=' ) at[2]=at[3];
				if( at[2]>='1' && at[2]<='3'){
					printf("adc port %c = (%d)\n",at[2],(int)xbee_adc(dev,(byte)(at[2]-'0')) );
				}
			}
		}else if( at[0] == 'A' && at[1] == 'I' ){
			xbee_log(3,"XBee Association Indication ",strlen(at)-2);
			i = xbee_atai();
			printf("result :");
			print_atai(i);
		}else if( at[0] == 'A' && at[1] == 'T' && at[4] != '=' && ( at[4] != '\0' || at[2] == '\0' )){
			xbee_log(3,"AT command ",strlen(at)-2);
			for(i=3;i<(AT_LEN-1);i++) if( at[i] >='a' && at[i] <= 'z' ) at[i] -= ('a'-'A');
			printf("xbee_at(%s)\n",at);
			printf("return(0x%02X)\n",xbee_at(at) );
		}else if( at[0] == 'B' && at[1] == 'A' && at[2] == 'T' ){
			xbee_log(3,"Force Battery Voltage command; xbee_batt_force ",strlen(at)-2);
			printf("return(0x%02X)\n", xbee_batt_force(dev) );
		}else if( at[0] == 'C' && at[1] == 'B' ){
			xbee_log(3,"Commission Button command; xbee_atcb ",strlen(at)-2);
			if( at[2] == '1' || at[2] == '2' || at[2] == '4' ){
				i = (byte)at[2] - (byte)'0';
				printf("return(0x%02X)\n", xbee_atcb( i ) );
			}
		}else if( 	(at[0] == 'D' && at[1] == 'T') ){
			xbee_log(3,"XBee Device Type ",strlen(at)-2);
			printf("result :");
			i=xbee_atvr();
			print_atvr(i);
		}else if( at[0] == 'E' && at[1] == 'E' ){
			xbee_log(3,"EncryptionEnable command; xbee_atee_on/off ",strlen(at)-2);
			if( at[2] == '=' ){
				printf("key = ");
				for(i=0;i<16;i++){
					if( at[3+i] == '\0' ) break;
					printf("%0X",(byte)at[3+i]);
				}
				printf(" (%s)\n", &at[3] );
				printf("return(0x%02X)\n", xbee_atee_on( &at[3] ) );
			}else if( at[2] == '0' ){
				printf("<Disable Encription>\n");
				printf("return(0x%02X)\n", xbee_atee_off() );
			}else if( at[2] == '1' ){
				printf("<Enable Encription>\n");
				printf("return(0x%02X)\n", xbee_atee_on( "" ) );
			}
		}else if( at[0] == 'E' && at[1] == 'D' ){
			xbee_log(3,"Test xbee_end_device ",strlen(at)-2);
			printf("return(0x%02X)\n", xbee_end_device(dev,10,10,0) );
		}else if(at[0] == 'H' && at[1] == 'Z' ){
			xbee_log(3,"Help about ZCL ",strlen(at)-2);
			if( (fp = fopen("xbee_test_help_zc.txt","r")) == NULL ){
				if( (fp = fopen("tools/xbee_test_help_zc.txt","r")) == NULL ){
					if( (fp = fopen("../tools/xbee_test_help_zc.txt","r")) == NULL ){
						printf("no help files\n");
					}
				}
			}
			if(fp){
				while( feof(fp) == 0 ){
					c=getc(fp);
					if( !feof(fp) ) printf("%c",c);
				}
				printf("\n");
				fclose( fp );                                // Output
			}
		}else if(at[0] == 'H' ){
			xbee_log(3,"Help ('hz' for more) ",strlen(at)-1);
			if( (fp = fopen("xbee_test_help.txt","r")) == NULL ){
				if( (fp = fopen("tools/xbee_test_help.txt","r")) == NULL ){
					if( (fp = fopen("../tools/xbee_test_help.txt","r")) == NULL ){
						printf("no help files\n");
					}
				}
			}
			if(fp){
				while( feof(fp) == 0 ){
					c=getc(fp);
					if( !feof(fp) ) printf("%c",c);
				}
				printf("\n");
				fclose( fp );                                // Output
			}
		}else if( at[0] == 'G' && at[1] == 'P' && at[2] == 'O' ){
			xbee_log(3,"Digital IO Output 'xbee_gpo' ",strlen(at)-3);
			if( at[3] == '=' ) j=4; else j=3;
			i = (byte)at[j] - (byte)'0';
			j = (byte)at[j+1] - (byte)'0';
			printf("return(0x%02X)\n", xbee_gpo(dev,i,j) );
		}else if( at[0] == 'G' && at[1] == 'P' && at[2] == 'I' ){
			xbee_log(3,"Digital IO Input 'xbee_gpi' ",strlen(at)-3);
			if( at[3] == '=' ) j=4; else j=3;
			i = (byte)at[j] - (byte)'0';
			printf("return(0x%02X)\n", xbee_gpi(dev,i) );
		}else if( at[0] == 'I' && at[1] == 'S' ){
			xbee_log(3,"Force Sample command (xbee_force) ",strlen(at)-2);
			printf("return(0x%02X)\n", xbee_force(dev) );
		}else if( at[0] == 'I' && at[1] == 'D' ){
			xbee_from(dev);
			xbee_log(3,"Stored recieved IEEE to destination's ",strlen(at)-2);
			for(i=0;i<4;i++) lcd_disp_hex( dev[i] ); lcd_putch( ' ' );
			for(i=4;i<8;i++) lcd_disp_hex( dev[i] ); lcd_putch( '\n' );
		}else if( at[0] == 'I' && at[1] == 'E' && at[2] != '\0'){
			xbee_log(3,"Stored destination IEEE ",strlen(at)-2);
			for(i=0;i<8;i++) dev[i]=hex2byte( &at[2+i*2] );
			for(i=0;i<8;i++) ADR_DEST[i]  = dev[i];
			for(i=0;i<4;i++) lcd_disp_hex( dev[i] ); lcd_putch( ' ' );
			for(i=4;i<8;i++) lcd_disp_hex( dev[i] ); lcd_putch( '\n' );
		}else if( at[0] == 'I'){
			unsigned short pan_id_s;
			
			xbee_log(3,"Display Information ",strlen(at));
			i=xbee_atvr();
			printf("device type :%02X ", i);
			print_atvr(i);
			pan_id_s=xbee_atop(dev_tmp);
			printf("PAN ID      :");
			for(i=0;i<4;i++) lcd_disp_hex( dev_tmp[i] ); lcd_putch( ' ' );
			for(i=0;i<4;i++) lcd_disp_hex( dev_tmp[4+i] ); lcd_putch( ' ' );
			printf("%04X\n",pan_id_s);
			i=xbee_atai();
			printf("xbee_atai   :%02X ", i );
			print_atai(i);
			xbee_myaddress( dev_tmp );
			printf("xbee_atnc   :%d\n", xbee_atnc() );
			printf("my address  :");
			for(i=0;i<4;i++) lcd_disp_hex( dev_tmp[i] ); lcd_putch( ' ' );
			for(i=0;i<4;i++) lcd_disp_hex( dev_tmp[4+i] ); lcd_putch( '\n' );
			printf("dest address:");
			for(i=0;i<4;i++) lcd_disp_hex( dev[i] ); lcd_putch( ' ' );
			for(i=0;i<4;i++) lcd_disp_hex( dev[4+i] ); lcd_putch( ' ' );
			for(i=0;i<2;i++) lcd_disp_hex( SADR_DEST[i] ); lcd_putch( '\n' );
		}else if( at[0] == 'N' && at[1] == 'C' ){
			xbee_log(3,"Number of Children; End Devices ",strlen(at)-2);
			printf("Coordinator  :");
			i=xbee_atnc();
			printf("%2d/10 (%2d)\n", i,10-i );
			printf("Remoto Router:");
			i=xbee_ratnc(dev);
			if(i==0xFF) printf("no\n");
			else printf("%2d/12 (%2d)\n",i,12-i);
		}else if( at[0] == 'P' && at[1] == 'I' ){
			xbee_log(3,"XBee Ping ",strlen(at)-2);
			printf("dest   :");
			for(i=0;i<4;i++) lcd_disp_hex( dev[i] ); lcd_putch( ' ' );
			for(i=4;i<8;i++) lcd_disp_hex( dev[i] ); lcd_putch( '\n' );
			i=xbee_ping(dev);
			printf("result :");
			switch(i){
				case DEV_TYPE_XBEE:		// 0x00		// XBeeモジュール
					printf("OK (DEV_TYPE_XBEE)\n");
					break;
				case DEV_TYPE_RS232:	// 0x05		// RS-232Cアダプタ
					printf("OK (DEV_TYPE_RS232)\n");
					break;
				case DEV_TYPE_SENS:		// 0x07		// Sensor (1wire専用)
					printf("OK (DEV_TYPE_SENS)\n");
					break;
				case DEV_TYPE_WALL:		// 0x08		// Wall Router
					printf("OK (DEV_TYPE_WALL)\n");
					break;
				case DEV_TYPE_PLUG:		// 0x0F		// SmartPlug
					printf("OK (DEV_TYPE_PLUG)\n");
					break;
				case 0x0E:		// XBee Sensor /L/T
					printf("OK (XBee Sensor /L/T)\n");
					break;
				case 0x0D:		// XBee Sensor /L/T/H
					printf("OK (XBee Sensor /L/T/H)\n");
					break;
				case 0xFF:
					printf("ERROR\n");
					break;
				default:
					printf("OK (unknown device)\n");
					break;
			}
			printf("return(0x%02X)\n", i );
		}else if( at[0] == 'R' && at[1] == 'A' && at[2] == 'T' && at[5] != '=' && (at[5] != '\0' || at[3] == '\0')){
			xbee_log(3,"RemoteAT command [lib] ",strlen(at)-3);
			for(i=1;i<(AT_LEN-1);i++) at[i-1] = at[i];
			for(i=2;i<(AT_LEN-1);i++) if( at[i] >='a' && at[i] <= 'z' ) at[i] -= ('a'-'A');
			printf("dest   :");
			for(i=0;i<4;i++) lcd_disp_hex( dev[i] ); lcd_putch( ' ' );
			for(i=4;i<8;i++) lcd_disp_hex( dev[i] ); lcd_putch( '\n' );
			printf("xbee_rat(%s)\n",at);
			printf("return(0x%02X)\n",xbee_rat(dev,at) );
		}else if( at[0] == 'J' && at[1] == 'H' && at[2] == 'J' && at[3] == 'h' ){
			// JHシステム（IEEE802.15.4）で使用されている無線方式を解析したところ
			// ZigBee PROに近い仕様であることが判明。
			// 使用しているクラスタIDはFD0C、プロファイルIDは0104、エンドポイントは14
			// 属性は4010と4013がある。（少なくともこれらに書き込みを行っている）
			// 情報提供：Tsunekichi Yoshida 様
			xbee_log(3,"ZigBee JH System ",strlen(at)-4);
			zcl_sep	=0x14;
			zcl_dep	=0x14;
			zcl_clid[0]=0xFD;
			zcl_clid[1]=0x0C;
			zcl_prid[0]=0x01;
			zcl_prid[1]=0x04;
			zcl_fc		=0x00;
			zcl_command=0x00;		// read
			zcl_att[0] =0x40;		// プロトコルアナライザで解析したところ4010と4013にWriteしている。
			zcl_att[1] =0x10;		// それぞれ何の属性かは今のところ不明。
			zcl_att_len=2;
		}else if( at[0] == 'T' && at[1] == 'T' && at[2] != '=' && at[2] != '\0' ){
			xbee_log(3,"uartTX command [lib] ",strlen(at)-2);
			at[0] = 'T';
			at[1] = 'X';
			printf("dest   :");
			for(i=0;i<4;i++) lcd_disp_hex( dev[i] ); lcd_putch( ' ' );
			for(i=4;i<8;i++) lcd_disp_hex( dev[i] ); lcd_putch( '\n' );
			for(i=3;i<(AT_LEN-1);i++) if( at[i] >='a' && at[i] <= 'z' ) at[i] -= ('a'-'A');
			printf("xbee_rat_tx(%s) = 0x%02X\n",at, xbee_rat(dev,at) );
		}else if( 	(at[0] == 'V' && at[1] == 'R') ){
			xbee_log(3,"XBee Device Type ",strlen(at)-2);
			printf("result :");
			i=xbee_atvr();
			print_atvr(i);
		}else if( at[0] == 'X' && at[1] == 'B' ){
			xbee_log(3,"XBee ZB Mode ",strlen(at)-2);
			printf("ATAO00(0x%02X)\n", xbee_at("ATAO00") );
			printf("ATZS00(0x%02X)\n", xbee_at("ATZS00") );
			printf("ATWR(0x%02X)\n", xbee_at("ATWR") );
		}else if( at[0] == 'Z' && at[1] == 'B' ){
			xbee_log(3,"ZigBee Mode ",strlen(at)-2);
			printf("ATAO01(0x%02X)\n", xbee_at("ATAO01") );
			printf("ATZS02(0x%02X)\n", xbee_at("ATZS02") );
			printf("ATWR(0x%02X)\n", xbee_at("ATWR") );
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'D' && at[3] == 't' ){
			xbee_log(3,"DesTination address ",strlen(at)-4);
			if(at[4] != '\0' && at[8] == '\0'){
				SADR_DEST[0] = hex2byte(&at[4]);
				SADR_DEST[1] = hex2byte(&at[6]);
				for(i=0;i<2;i++) zcl_dest[i]=SADR_DEST[i];
			}
			printf("Short Address :");
			for(i=0;i<2;i++) lcd_disp_hex( zcl_dest[i] ); lcd_putch( '\n' );
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'S' && at[3] == 'e' ){
			if(at[4] != '\0' && at[6] == '\0'){
				zcl_sep = hex2byte(&at[4]);
			}
			printf("ZCL Source End point:%02X\n",zcl_sep);
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'D' && at[3] == 'e' ){
			if(at[4] != '\0' && at[6] == '\0'){
				zcl_dep = hex2byte(&at[4]);
			}
			printf("ZCL Destination End point:%02X\n",zcl_dep);
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'C' && at[3] == 'i' ){
			if(at[4] != '\0' && at[8] == '\0'){
				zcl_clid[0] = hex2byte(&at[4]);
				zcl_clid[1] = hex2byte(&at[6]);
			}
			printf("ZCL Cluster ID:%02X%02X\n",zcl_clid[0],zcl_clid[1]);
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'P' && at[3] == 'i' ){
			if(at[4] != '\0' && at[8] == '\0'){
				zcl_prid[0] = hex2byte(&at[4]);
				zcl_prid[1] = hex2byte(&at[6]);
			}
			printf("ZCL Profile ID:%02X%02X\n",zcl_prid[0],zcl_prid[1]);
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'R' && at[3] == 'd' ){
			if(at[4] != '\0' && at[6] == '\0'){
				zcl_radi = hex2byte(&at[4]);
			}
			printf("ZCL RaDius :%02X\n",zcl_radi);
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'F' && at[3] == 'c' ){
			if(at[4] != '\0' && at[6] == '\0'){
				zcl_fc = hex2byte(&at[4]);
			}
			printf("ZCL Frame Control :%02X\n",zcl_fc);
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'S' && at[3] == 'q' ){
			if(at[4] != '\0' && at[8] == '\0'){
				zcl_seq = hex2byte(&at[4]);
			}
			printf("ZCL SeQuence number:%02X\n",zcl_seq);
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'C' && at[3] == 'm' ){
			if(at[4] != '\0' && at[8] == '\0'){
				zcl_command = hex2byte(&at[4]);
			}
			printf("ZCL CoMmand :%02X\n",zcl_command);
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == 'A' && at[3] == 't' ){
			if(at[4] != '\0' ){
				for(i=0;i<3;i++){
					if( at[i*2+4]=='\0' ) break;
					zcl_att[i] = hex2byte(&at[i*2+4]);
				}
				zcl_att_len=i;
			}
			printf("ATtribute :");
			for(i=0;i<zcl_att_len;i++)printf("%02X ",zcl_att[i]);
			printf("\n");
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] == '\0' ){
			xbee_log(3,"ZCL command [lib] ",strlen(at)-2);
			for(i=0;i<8;i++) ADR_DEST[i]  = dev[i];
			for(i=0;i<2;i++) SADR_DEST[i] = zcl_dest[i];
			printf("Destination:");
			for(i=0;i<4;i++) lcd_disp_hex( ADR_DEST[i] ); lcd_putch( ' ' );
			for(i=4;i<8;i++) lcd_disp_hex( ADR_DEST[i] ); lcd_putch( ' ' );
			for(i=0;i<2;i++) lcd_disp_hex( SADR_DEST[i] ); lcd_putch( '\n' );
			printf("Source End :%02X\n",zcl_sep);
			printf("Dest End   :%02X\n",zcl_dep);
			printf("Cluster ID :%02X%02X\n",zcl_clid[0],zcl_clid[1]);
			printf("Profile ID :%02X%02X\n",zcl_prid[0],zcl_prid[1]);
			printf("RaDius     :%02X\n",zcl_radi);
			printf("Frame Ctrl :%02X\n",zcl_fc);
			printf("SeQuence n :%02X\n",zcl_seq);
			printf("CoMmand    :%02X\n",zcl_command);
			printf("ATtribute  :");
			for(i=0;i<zcl_att_len;i++)printf("%02X ",zcl_att[i]);
			printf("\n");
			i=0;
			at[i++] = 'Z';
			at[i++] = 'C';
			at[i++] = byte2a(zcl_sep,1);
			at[i++] = byte2a(zcl_sep,0);
			at[i++] = byte2a(zcl_dep,1);
			at[i++] = byte2a(zcl_dep,0);
			at[i++] = byte2a(zcl_clid[0],1);
			at[i++] = byte2a(zcl_clid[0],0);
			at[i++] = byte2a(zcl_clid[1],1);
			at[i++] = byte2a(zcl_clid[1],0);
			at[i++] = byte2a(zcl_prid[0],1);
			at[i++] = byte2a(zcl_prid[0],0);
			at[i++] = byte2a(zcl_prid[1],1);
			at[i++] = byte2a(zcl_prid[1],0);
			at[i++] = byte2a(zcl_radi,1);
			at[i++] = byte2a(zcl_radi,0);
			at[i++] = '0';						// Offset 22 i=16
			at[i++] = '0';
			at[i++] = byte2a(zcl_fc,1);			// Offset 23 i=18
			at[i++] = byte2a(zcl_fc,0);
			at[i++] = byte2a(zcl_seq,1);		// Offset 24 i=20
			at[i++] = byte2a(zcl_seq,0);
			at[i++] = byte2a(zcl_command,1);	// Offset 25 i=22
			at[i++] = byte2a(zcl_command,0);
			/*
			at[i++] = byte2a(zcl_att[1],1);		// Offset 26 i=24
			at[i++] = byte2a(zcl_att[1],0);
			at[i++] = byte2a(zcl_att[0],1);		// Offset 27 i=26
			at[i++] = byte2a(zcl_att[0],0);
			*/
			for(j=0;j<zcl_att_len;j++){
				at[i++] = byte2a(zcl_att[zcl_att_len-j-1],1);
				at[i++] = byte2a(zcl_att[zcl_att_len-j-1],0);
				if(i >= XB_AT_SIZE ){
					// 念のためのメモリ保護
					printf("ERROR:Segmentation fault at at[XB_AT_SIZE]\n");
					exit(-1);
				}
			}
			at[i++] = '\0';
			printf("xbee_rat_tx(%s) = 0x%02X\n",at, xbee_rat(dev,at) );
			if(zcl_seq!=0 ){
				if(zcl_seq != 0xFF) zcl_seq++;
				else zcl_seq=0x01;
			}
/*
ZDO			ZDO Command					Cluster ID
			Network Address Request		0x0000
			IEEE Address Request		0x0001
			Node Descriptor Request		0x0002
			Simple Descriptor Request	0x0004
			Active Endpoints Request	0x0005
			Match Descriptor Request	0x0006
			Mgmt LQI Request			0x0031
			Mgmt Routing Request		0x0032
			Mgmt Leave Request			0x0034
			Mgmt Permit Joining Request	0x0036
			Mgmt Network Update Request	0x0038

ZCL
			Cluster(Cluster ID)	Attributes (Attribute ID) 		Command ID
			Basic (0x0000)		Application Version (0x0001)	-Reset to defaults(0x00)
								Hardware Version (0x0003)
								Model Identifier (0x0005)
			Identify (0x0003)	Identify Time (0x0000)			Identify (0x00)
																Identify Query (0x01)
			Time (0x000A)		Time (0x0000)
								Time Status (0x0001)
								Time Zone (0x0002)
			Thermostat (0x0201)	Local Temperature (0x0000)		-Setpoint raise / low(0x00)
								Occupancy (0x0002)

			Command(Command ID)					Description
			Read Attributes (0x00)				Used to read one or more attributes on a remote device.
			Read Attributes Response (0x01)		Generated in response to a read attributes command.
			Write Attributes (0x02)				Used to change one or more attributes on a remote device.
			Write Attributes Response (0x04)	Sent in response to a write attributes command.
			Configure Reporting (0x06)			Used to configure a device to automatically report on the values of one or more of its ttributes.
			Report Attributes (0x0A)			Used to report attributes when report conditions have been satisfied.
			Discover Attributes (0x0C)			Used to discover the attribute identifiers on a remote device.
			Discover Attributes Response (0x0D)	Sent in response to a discover attributes command.
*/
		}else if( at[0] == 'Z' && at[1] == 'C' && at[2] != '=' && at[2] != '\0' ){
			xbee_log(3,"ZCL command [lib] ",strlen(at)-2);
			at[0] = 'Z';
			at[1] = 'C';
			printf("dest   :");
			for(i=0;i<4;i++) lcd_disp_hex( ADR_DEST[i] ); lcd_putch( ' ' );
			for(i=4;i<8;i++) lcd_disp_hex( ADR_DEST[i] ); lcd_putch( ' ' );
			for(i=0;i<2;i++) lcd_disp_hex( SADR_DEST[i] ); lcd_putch( '\n' );
			for(i=3;i<(AT_LEN-1);i++) if( at[i] >='a' && at[i] <= 'z' ) at[i] -= ('a'-'A');
			printf("xbee_rat_tx(%s) = 0x%02X\n",at, xbee_rat(dev,at) );
			if(zcl_seq!=0 ){
				if(zcl_seq != 0xFF) zcl_seq++;
				else zcl_seq=0x01;
			}
		}else{
			/* 開発用：独自解析の開始 */
			xbee_log(3,"XBee API Test ",strlen(at));
			printf("Execute '%s'\n",at);
			wait_add = 0;
			switch( at[0] ){
				case 'A':
					if( at[1] == 'T' ){
						at_len=2;
						r_dat = 0; r_at=0;
						if( at[2] == 'W' && at[3] == 'R' ) wait_add = 100;			// ATWR 110～1100ms
					}
					break;
				case 'R':
				case 'S':
					if( at[1] == 'A' && at[2] == 'T' ){
						printf("RemoteAT command [dev]\n");
						printf("dest   :");
						for(i=0;i<4;i++) lcd_disp_hex( dev[i] ); lcd_putch( ' ' );
						for(i=4;i<8;i++) lcd_disp_hex( dev[i] ); lcd_putch( '\n' );
						at_len=3;
						r_dat = 10; r_at=1;
						if( at[3] == 'W' && at[4] == 'R' ) wait_add = 100;	// RATWR 220～2200ms
					}
					break;
				case 'T':
					if( at[1] == 'X' ){
						printf("uartTX command [dev]\n");
						printf("dest   :");
						for(i=0;i<4;i++) lcd_disp_hex( dev[i] ); lcd_putch( ' ' );
						for(i=4;i<8;i++) lcd_disp_hex( dev[i] ); lcd_putch( '\n' );
						printf("sending:\n");
						i=3;
						while( at[i] != '\0' && at[i] != '\n' && at[i] != '\r' && i < AT_LEN && i < FO_LEN ){
							s[i-3] = at[i];
							s[i-2] = '\0';
							if( (byte)at[i] >= 32 && (byte)at[i] <= 126 ) printf("%c",at[i]);
							else printf(".");
							i++;
						}
						printf("\n");
						xbee_address( dev );
						xbee_putstr( s );
						at_len=0;
						at[0] = 0x00;
						at[1] = 0x00;
					}
					break;
				case 'F':
					if( at[1] == 'O' ){
						for(i=3; i< AT_LEN ; i++){
							if( (byte)at[i] >= 32 && (byte)at[i] <= 126 ){
								filename[i-3] = at[i];
							}else{
								at[i] = 0x00;
								filename[i-3] = '\0';
								break;
							}
						}
						printf("open '%s'\n",filename);
						if( (i > 3) && (fp = fopen( filename,"r")) != NULL ){
							printf("dest   :");
							for(i=0;i<4;i++) lcd_disp_hex( dev[i] ); lcd_putch( ' ' );
							for(i=4;i<8;i++) lcd_disp_hex( dev[i] ); lcd_putch( '\n' );
							printf("sending:\n");
							while( feof(fp) == 0 ){
								for( i= 0; i < FO_LEN-1 ; i++ ) s[i] = '\0';
								i=0;
								while( feof(fp) == 0 && i < (FO_LEN - 1) ){
									if( fscanf( fp , "%c", &c ) >= 0 ){
										s[i] = c;
										i++;
										if( (byte)c >= 32 && (byte)c <= 126 ) printf("%c",c);
										else printf(".");
									}
								}
								printf("\n");
								xbee_address( dev );
								xbee_putstr( s );
							}							
							fclose( fp );
						}else{
							printf("Usage:FO <filename>\n");
						}
						at_len=0;
						at[0] = 0x00;
						at[1] = 0x00;
					}
					break;
				default:
					break;
			}
			for(i=3;i<(AT_LEN-1);i++) if( at[i] >='a' && at[i] <= 'z' ) at[i] -= ('a'-'A');
			for(i=0;i<2;i++){
				if( (byte)(at[at_len]) >= 32 && (byte)(at[at_len]) <= 126 ){
					at_len++;
				}else{
					at_len=0;
					i=2;
				}
			}
			val_len = 0;
			if( at_len > 0){
				if( at[at_len] == '=' ){
					at[at_len] = 0x00;
					at_len++;
				}
				for(i=0; i < (AT_LEN-at_len-1) ; i += 2){
					if( (byte)at[i+at_len] >= (byte)'0' && (byte)at[i+at_len] <= (byte)'9'){
						data[val_len] = 16 * ((byte)at[i+at_len] - (byte)'0');
					}else if( (byte)at[i+at_len] >= (byte)'A' && (byte)at[i+at_len] <= (byte)'F' ){
						data[val_len] = 16 * ((byte)at[i+at_len] + 10 - (byte)'A');
					}else{
						break;
					}
					at[i+at_len]=0x00;
					if( i < 0xF0 ){
						if( (byte)at[i+at_len+1] >= (byte)'0' && (byte)at[i+at_len+1] <= (byte)'9'){
							data[val_len] += ((byte)at[i+at_len+1] - '0');
							val_len++;
						}else if( (byte)at[i+at_len+1] >= (byte)'A' && (byte)at[i+at_len+1] <= (byte)'F' ){
							data[val_len] += ((byte)at[i+at_len+1] + 10 - (byte)'A' );
							val_len++;
						}else{
							i=0xF0;
						}
					}
					at[i+at_len+1]=0x00;
				}
				/* コマンド送信 */
				xbee_address(dev);
				printf("xbee_tx:%s[%d]",at,val_len);
				for( i=0 ; i< val_len ; i++) lcd_disp_hex( data[i] ); printf("\n");
				if( xbee_at_tx( at ,data ,val_len ) > 0){
					err = 12;							// 受信なしエラー
					for( retry = 10 ; ( retry > 0 && err == 12 ) ; retry-- ){
						wait_millisec( 10 + r_dat );	// 応答時間待ち AT 10～100ms / RAT 20～200ms
						if( wait_add != 0 ){
							wait_millisec( wait_add );	// 追加ウェイト
							printf("added wait time\n");
						}
						for(j=0;j<(API_SIZE-1);j++) data[j] = 0x00;  data[API_SIZE-1] = 0x00;
						res_len = xbee_at_rx( data );
						if( res_len > 0){
							if( data[4] == PACKET_ID
							 && data[5+r_dat] == (byte)at[2+r_at]
							 && data[6+r_dat] == (byte)at[3+r_at] ){
								if( data[7+r_dat] == 0x00 ){
									err=0;
								}else err = data[7+r_dat];	// ATのERRORコード (AT:data[7] RAT:data[17])
							}else	err = 13;				// 受信パケットの相違
						}
					}
				}else				err = 11;				// 送信失敗
				
				/* ERROR 表示 */
				switch(err){
					case 0:             // 01234567890123456789
						break;
					case 2:             // 01234567890123456789
						printf("ERR 02:AT Command:%s\n",at);
						break;
					case 3:
						printf("ERR 03:AT Param. \n");
						break;
					case 4:
						printf("ERR 04:AT Commu. \n");
						break;
					case 10:
						printf("ERR 10:not AT CMD:%s",at);
						break;
					case 11:
						printf("ERR 11:TX Failed \n");
						break;
					case 12:
						printf("ERR 12:no Rx Res.\n");
						break;
					case 13:
						printf("ERR 13:AT Pckt ID:%d\n",PACKET_ID);
						break;
					case 14:
						printf("ERR 14:Diff.Adrs.\n");
						break;
					default:
						printf("ERR :unknown[%02X]\n",err);
						break;
				}
				
				/* DATA 表示 */
				printf("xbee_rx:%s[%d]\n",at,res_len);
				if( res_len ){
					for( i=0; i < res_len ; i++ ){
						if( data[i] >= 32 && data[i] <= 126 ) c = (char)data[i]; else c = ' ';
						printf("data[%02d] = 0x%02X '%c' (%d)\n",i,data[i],c,(int)data[i]);
					}
					printf("checksum = 0x%02X     (%d)\n",data[res_len],(int)data[res_len]);
				}
				return_MODE = xbee_from_acum( data );
				switch( return_MODE ){		// 受信したデータの内容(MODE値)に応じて
					case MODE_RESP:	
						printf("\n--------------------\nrecieved RAT_RESP\n--------------------\n");
						printf("from   :");
						for(i=0;i<4;i++) lcd_disp_hex( data[5+i] ); lcd_putch( ' ' );
						for(i=0;i<4;i++) lcd_disp_hex( data[9+i] ); lcd_putch( '\n' );
						printf("id     :%02X\n", data[4]);
						printf("status :%02X", data[17]);
						print_status( data[17] );
						printf("\n");
						if( data[15]=='I' && data[16]=='S' ){	// ATISの応答の受信
							printf("gpi1-0 :");
							lcd_disp_bin( data[22] & data[19] ); printf(" ");
							lcd_disp_bin( data[23] & data[20] ); printf("\n");
							if( data[19] == 0x00 && data[20] == 0x00 ){
								j = 1;											// デジタルデータが無いとき
							}else{
								j = 0;											// デジタルデータがある時
							}
							for( i=0; i < 4 ; i++ ){							// この中でjを使用している
								if( (data[21]>>i) & 0x01 ){						// data[21] ADCマスク
									xbee_result.ADCIN[i] =  (unsigned int)(data[(2*i+24-2*j)]);
									xbee_result.ADCIN[i] *= (unsigned int)256;
									xbee_result.ADCIN[i] += (unsigned int)(data[2*i+25-2*j]);
									xbee_result.DATA[i*2+2] = data[24+2*i-2*j];
									xbee_result.DATA[i*2+3] = data[25+2*i-2*j];
								}else{									
									xbee_result.DATA[i*2+2] = 0xFF;
									xbee_result.DATA[i*2+3] = 0xFF;
									j++;
								}
							}
							if( (data[21]>>7) & 0x01 ){
								xbee_result.ADCIN[0] = xbee_batt_acum(data[32-2*j],data[33-2*j]);
							}
							printf("adc1-3 :%d %d %d\n", xbee_result.ADCIN[1], xbee_result.ADCIN[2],xbee_result.ADCIN[3] );
							if( xbee_result.ADCIN[0] != 0 )printf("battery:%d mV\n", xbee_result.ADCIN[0] );
						}else if( data[15] == '%' && data[16] == 'V'){	// BATTの受信
							printf("BATT   :");
							lcd_disp_bin( data[18] ); printf(" ");
							lcd_disp_bin( data[19] ); printf("\n");
							printf("battery:%d mV\n", (int)( ( (float)data[18] * 256. + (float)data[19] ) * (1200. / 1024.) ) );
						}
						break;
					case MODE_RES:
						printf("\n--------------------\nrecieved AT_RESP\n--------------------\n");
						printf("status :%02X", data[7]);
						print_status(data[7]);
						printf("\n");
						break;
					default:
						printf("\n--------------------\nrecieved UNKNOWN\n--------------------\n");
						printf("mode   :%02X\n", return_MODE );
						break;
				}
			}
		}
		printf("done\n");
	}
	close_serial_port();
	return(ret);
}
