/***************************************************************************************
親機XBeeと子機XBeeとのペアリングと状態取得

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"

int main(int argc,char **argv){
    
    byte i;                             // 繰り返し（ループ）回数保持用
    byte com=0xB0;                      // シリアル(USB)、拡張IOコネクタの場合は0xA0
    byte value;                         // リモート子機からの入力値
    byte dev[8];                        // XBee子機デバイスのアドレス

    if(argc==2) com += atoi(argv[1]);   // 引数があれば変数comに代入する
    xbee_init( com );                   // XBee用COMポートの初期化(引数はポート番号)
    
    printf("XBee in Commissioning\n");  // 待ち受け中の表示
    if(xbee_atnj(30) == 0){             // デバイスの参加受け入れを開始（最大30秒間）
        printf("No Devices\n");         // エラー時の表示
        exit(-1);                       // 異常終了
    }else{
        printf("Found a Device\n");     // XBee子機デバイスの発見表示
        xbee_from( dev );               // 見つけたデバイスのアドレスを変数devに取込む
        xbee_ratnj(dev,0);              // 子機に対して孫機の受け入れ制限を設定
        for(i=0;i<8;i++){
            printf("%02X ",dev[i]);    // アドレスの表示
        }
        printf("\n");
    }
    // 処理の一例（XBee子機のポート１～３の状態を取得して表示する）
    while(1){
        for(i=1;i<=3;i++){
            value=xbee_gpi(dev,i);       // XBee子機のポートiのデジタル値を取得
            printf("D%d:%d ",i,value);  // 表示
        }
        printf("\n");
        delay(1000);
    }
}
