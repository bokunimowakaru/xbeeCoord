/***************************************************************************************
LEDをリモート制御する④通信の暗号化

                                                  Copyright (c) 2013-2015 Wataru KUNINO
***************************************************************************************/

#include "../libs/xbee.c"
#include "../libs/kbhit.c"

int main(int argc,char **argv){
    
    byte com=0xB0;                          // シリアル(USB)、拡張IOコネクタの場合は0xA0
    byte dev[8];                            // XBee子機デバイスのアドレス
    
    /* 初期化 */
    if(argc==2) com += atoi(argv[1]);       // 引数があれば変数comに値を加算する
    xbee_init( com );                       // XBee用COMポートの初期化
    
    /* Step 1. 暗号化有効 */
    if( xbee_atee_on("password") <= 1 ){    // 暗号化ON設定。passwordは16文字まで
        printf("Encryption On\n");          // "password" -> 70617373776F7264
        
        /* Step 2. ペアリング */
        printf("Waiting for XBee\n");       // 待ち受け中の表示
        if( xbee_atnj(180) ){               // デバイスの参加受け入れを開始(180秒間)
            printf("Found a Device\n");     // XBee子機デバイスの発見表示
            xbee_from( dev );               // 見つけたデバイスのアドレスをdevに取込む
            
            /* Step 3. LEDの点滅(暗号化) */
            while( !kbhit() ){              // キーボードからの入力が無い時に繰り返す
                xbee_gpo(dev, 4, 1);        // リモートXBeeのポート4を出力'H'に設定する
                delay( 1000 );              // 1000ms(1秒間)の待ち
                xbee_gpo(dev, 4, 0);        // リモートXBeeのポート4を出力'L'に設定する
                delay( 1000 );              // 1000ms(1秒間)の待ち
            }
        }
    //  xbee_atee_off();                    // 暗号化OFF(実際のプログラムには用いない)
    }
    return 0;                               // 関数mainの終了
}
