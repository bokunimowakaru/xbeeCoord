/***************************************************************************************
Bluetoothモジュール RN-42XVP用の通信ドライバ
Ver 1.1 for Raspberry Pi 2016/01/22

                                                Copyright (c) 2013-2016 Wataru KUNINO
                                                http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#include "../libs/15term.c"
#define RX_MAX	64

extern char rx_data[RX_MAX];					// 受信データの格納用の文字列変数

/* シリアルの受信バッファを消去する関数 */
void bt_rx_clear(){
	while( read_serial_port() ){				// 受信データが残っている場合
		usleep(2000);
	}	
}

/* コマンドの受信を行う関数 */
int bt_rx(void){
	int i,loop=50;
	
	for(i=0;i<(RX_MAX-1);i++) rx_data[i]='\0';	// 受信データの初期化
	i=0;										// 受信データの数
	while(loop>0){
		loop--;
		rx_data[0] = read_serial_port();
		if( rx_data[0] ){						// 何らかの応答があった場合
			for(i=1;i<(RX_MAX-1);i++){
				rx_data[i]=read_serial_port();	// 受信データを保存する
				if( rx_data[i] ){
					usleep(2000);
				}else{
					break;
				}
			}
			bt_rx_clear();						// シリアルの受信バッファを消去する
			loop=0;
		}else usleep(10000);					// 応答待ち
	}
	return(i);
}

/* コマンド(cmd)の送受信を行う関数 */
int bt_cmd(char *cmd){
	bt_rx_clear();								// シリアルの受信バッファを消去する
	write(ComFd, cmd, strlen(cmd) );
	if(strcmp(cmd,"$$$")) write(ComFd, "\n", 1 );
	return(bt_rx());							// 送信結果の受信
}

/* コマンドモード(mode)に入るための送受信を行う関数 */
int bt_cmd_mode(char mode){
	int i;
	char cmd[4];
	if( bt_cmd("GK") > 0 ){ 					// コマンドに応答があった場合で、かつ、
		if( rx_data[0]=='1') bt_cmd("K,");		// ネットワーク接続されている場合は切断
		else bt_cmd("---"); 					// 接続していない時はコマンドモード解除
		sleep(1);
	}
	bt_rx_clear();								// シリアルの受信バッファを消去する
	for(i=0;i<3;i++) cmd[i]=mode;
	cmd[3]='\0';
	write(ComFd, cmd, 3 );						// コマンドモードに入る命令を実行
	i = bt_rx();
	if( i>=5 ){ 								// 何らかの応答があった場合
		if( rx_data[i-5]=='C' && rx_data[i-4]=='M' && rx_data[i-3]=='D'){
			return(1);							// 成功
		}
	}
	return(0);									// 失敗
}

/* コマンド(cmd)を期待の応答(res)が得られるまで永続的に発行し続ける関数 */
void bt_repeat_cmd(char *cmd, char *res, int SizeOfRes){
	int i=0,j;
	bt_rx_clear();								// シリアルの受信バッファを消去する
	if(SizeOfRes >= RX_MAX)SizeOfRes=RX_MAX-1;	// 文字制限の調整
	do{
		if( i==0 ) bt_cmd(cmd);					// コマンドの発行（10秒に1回）
		if( ++i > 10 ) i=0;						// iのインクリメント
		sleep(1);								// 1秒間の応答待ち
		if( bt_rx() ){							// 受信データを取得
			if( SizeOfRes == 0 ) i=-1;			// SizeOfResが0の時は受信内容の確認なし
			for(j=0; j <= SizeOfRes ; j++ ){	// 内容を確認
				if( res[j] == '\0' ){			// 期待文字がすべて一致した
					i=-1;						// do-whileループを抜ける
					break;						// 本forループを抜ける
				}
				if( res[j] != rx_data[j] || rx_data[j] == '\0') break;
			}									// 不一致があればforループを抜ける
			if( j == SizeOfRes ) i=-1;			// 
		}
	}while( i >= 0 );							// 期待の応答があるまで繰り返し
}

/* エラー表示用の関数 */
void bt_error(char *err){
	char s[17];
	int i;
	for(i=0;i<16;i++){
		if(rx_data[i]=='\0') break;
		if(isprint(rx_data[i])) s[i]=rx_data[i];
		else s[i]=' ';
	}
	s[i]='\0';
	if(i==0) sprintf(s,"No RX data");
	bt_cmd("K,");								// 通信の切断
	bt_cmd("---");								// コマンドモードの解除
	fprintf(stderr,"%s\nPlease RESTART\n",err);	// 再起動依頼の表示
	exit(-1);
}

void bt_init_local(void){
	/* ローカルMaster機の設定 */
    printf("Config BT \n");	
    if( !bt_cmd_mode('$') ){					// ローカルコマンドモードへの移行を実行
	    bt_error("Config FAILED");
	}
	bt_cmd("SF,1");								// 工場出荷時の設定に戻す
	bt_cmd("SM,1");								// BluetoothのMasterデバイスに設定する
	bt_cmd("SA,4");								// PINペアリング方式に変更する。
	bt_cmd("SO,%");								// 接続・切断時にメッセージを表示する
	bt_cmd("ST,255");							// コマンドモードの時間制限を解除する
	bt_cmd("R,1");								// 再起動
    printf("DONE\n");
    sleep(1);									// 再起動待ち

	/* デバイス探索の実行 */
    if( !bt_cmd_mode('$') ){					// ローカルコマンドモードへの移行を実行
    	bt_error("FAILED to open");
    }
	printf("Inquiry \n");
	bt_repeat_cmd("I","Found",5);				// デバイスが見つかるまで探索を繰り返す
	printf("Found\n");
	while( bt_rx()==0 );						// アドレスの取得待ち
	
	/* ペアリングの実行 */
	printf("Pairing \n");
	bt_cmd("SR,I");								// 発見したデバイスのアドレスを保存
	bt_repeat_cmd("C","%CONNECT",8);			// 接続するまで接続コマンドを繰り返す
	printf("DONE\n");
	sleep(1);									// 接続後の待ち時間
	
	/* リモート機の設定 */
	printf("RemoteCnf \n");
    if( !bt_cmd_mode('$') ){					// ローカルコマンドモードへの移行
    	bt_error("RemoteCnf Failed");
	}
	bt_cmd("ST,255");							// コマンドモードの時間制限を解除する
	bt_cmd("R,1");								// 再起動
    printf("DONE\n");
	sleep(1);									// 再起動待ち
}

void bt_init(char *mac){
    printf("Bluetooth Remote\n");				// タイトル文字を表示
    if(open_rfcomm(mac) <= 0){                  // Bluetooth接続の開始
        bt_error("Bluetooth Open ERROR");
        /* bt_errorは戻ってこない */
        exit(1);
    }
}

void bt_close(void){
	close_rfcomm();
}
