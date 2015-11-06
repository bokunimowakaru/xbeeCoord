/***************************************************************************************
取得した情報をファイルに保存するロガーの製作

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#define S_MAX   255                             // 文字列変数sの最大容量(文字数-1)を定義

int main(int argc,char **argv){

    byte com=0xB0;                              // 拡張IOコネクタの場合は0xA0
    byte dev[8];                                // XBee子機デバイスのアドレス
    float value;                                // 受信データの代入用
    XBEE_RESULT xbee_result;                    // 受信データ(詳細)

    FILE *fp;                                   // 出力ファイル用のポインタ変数fpを定義
    char filename[] = "data.csv";               // ファイル名
    time_t timer;                               // タイマー変数の定義
    struct tm *time_st;                         // タイマーによる時刻格納用の構造体定義
    char s[S_MAX];                              // 文字列用の変数

    if(argc==2) com += atoi(argv[1]);           // 引数があれば変数comに代入する
    xbee_init( com );                           // XBee用COMポートの初期化
    xbee_atnj( 0xFF );                          // 子機XBeeデバイスを常に参加受け入れ
    printf("Waiting for XBee Commissoning\n");  // 待ち受け中の表示

    while(1){
        
        time(&timer);                               // 現在の時刻を変数timerに取得する
        time_st = localtime(&timer);                // timer値を時刻に変換してtime_stへ
        
        xbee_rx_call( &xbee_result );               // データを受信
        switch( xbee_result.MODE ){                 // 受信したデータの内容に応じて
            case MODE_GPIN:                         // 子機XBeeの自動送信の受信
                value = (float)xbee_result.ADCIN[1] * 3.55;
                strftime(s,S_MAX,"%Y/%m/%d, %H:%M:%S", time_st);    // 時刻→文字列変換
                sprintf(s,"%s, %.1f", s , value );                  // 測定結果をsに追加
                printf("%s Lux\n" , s );                            // 文字列sを表示
                if( (fp = fopen(filename, "a")) ) {                 // ファイルオープン
                    fprintf(fp,"%s\n" , s );                        // 文字列sを書き込み
                    fclose(fp);                                     // ファイルクローズ
                }else printf("fopen Failed\n");
                break;
            case MODE_IDNT:                         // 新しいデバイスを発見
                printf("Found a New Device\n");
                xbee_atnj(0);                       // 子機XBeeデバイスの参加を制限する
                bytecpy(dev, xbee_result.FROM, 8);  // 発見したアドレスをdevにコピーする
                xbee_ratnj(dev,0);                  // 子機に対して孫機の受け入れを制限
                xbee_gpio_config( dev, 1 , AIN );   // XBee子機のポート1をアナログ入力へ
                xbee_end_device( dev, 3, 3, 0);     // 起動間隔3秒,自動測定3秒,S端子無効
                break;
        }
    }
}
