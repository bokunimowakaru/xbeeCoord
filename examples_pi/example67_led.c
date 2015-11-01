/***************************************************************************************
XBee Wi-FiのLEDをリモート制御する②ライブラリ関数xbee_gpoで簡単制御

                                                       Copyright (c) 2013 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee_wifi.c"          // XBeeライブラリのインポート

// お手持ちのXBeeモジュールのIPアドレスに変更する(区切りはカンマ)
byte dev[] = {192,168,0,135};

int main(void){
    char s[3];                          // 入力用(2文字まで)
    byte port=4;                        // リモート機のポート番号(初期値＝4)
    byte value=1;                       // リモート機への設定値(初期値＝1)

    xbee_init( 0 );                     // XBeeの初期化
    printf("Example 67 LED ('q' to Exit)\n");
    while( xbee_ping(dev)==00 ){        // 繰り返し処理
        xbee_gpo(dev,port,value);       // リモート機ポート(port)に制御値(value)を設定
        printf("Port  =");              // ポート番号入力のための表示
        gets( s );                      // キーボードからの入力
        if( s[0] == 'q' ) break;        // [q]が入力された時にwhileを抜ける
        port = atoi( s );               // 入力文字を数字に変換してportに代入
        printf("Value =");              // 値の入力のための表示
        gets( s );                      // キーボードからの入力
        value = atoi( s );              // 入力文字を数字に変換してvalueに代入
    }
    printf("done\n");
    return(0);
}
