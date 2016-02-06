/***************************************************************************************
Bluetoothモジュール RN-42XVP搭載の照度センサを製作してリモートで照度値を取得します。

                                                Copyright (c) 2013-2016 Wataru KUNINO
                                                http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#include "../libs/bt_rn42.c"
#include "../libs/kbhit.c"
char rx_data[RX_MAX];                           // 受信データの格納用の文字列変数

int main(int argc,char **argv){
    int in,i,j;
    int adc[3];                                 // アナログ入力値の保持用
    
    if(argc != 2 || strlen(argv[1]) != 17){
        fprintf(stderr,"usage: %s xx:xx:xx:xx:xx:xx\n",argv[0]);
        return -1;
    }
    printf("example 48 Bluetooth RN-42 Sensor\n");
    if( bt_init(argv[1]) ) return -1;           // Bluetooth RN-42接続の開始
    printf("CONNECTED\nHit any key to EXIT\n");
    while( bt_cmd_mode('$') ){                  // リモートコマンドモードへの移行
        for(j=0;j<3;j++){                       // 3回の読み取りを実行
            adc[j]=0;
            bt_cmd("A");                        // ADC1ポートの読み取り
            for(i=5;i<8;i++){
                if( rx_data[i]==',' ) break;    // 16進数値に続くカンマを検出したら終了
                in = (unsigned char)rx_data[i]; // 大きい桁の文字コードを変数inに代入
                if( in >= '0' && in <= '9' ) in -= '0';        // 0～9なら数値へ変換
                else if( in >= 'A' && in <= 'F' ) in -= 'A'-10;// A～Fなら16進数値へ変換
                else break;
                adc[j] *= 16;                   // これまでの値を16倍する
                adc[j] += in;                   // 読み取った数値を加算
            }
        }
        i=0;
        if(adc[1] <= adc[2]){
            if( adc[0] <= adc[1] ) i = 1;
            if( adc[0] >= adc[2] ) i = 2;
        }else{
            if( adc[0] >= adc[1] ) i = 1;
            if( adc[0] <= adc[2] ) i = 2;
        }
        printf("AD1[%d] = %.1f Lux (0x%04X,%dmV)\n",i+1,(float)adc[i]*.13,adc[i],adc[i]);
        bt_cmd("---");                          // コマンドモードの解除
        sleep(5);
        if( kbhit() ) break;
    }
    bt_close();                                 // 切断処理
    return 0;
}
