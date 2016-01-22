/***************************************************************************************
Bluetoothモジュール RN-42XVPのGPIOポート3と7に接続したスイッチ状態を取得します。

                                                Copyright (c) 2013-2016 Wataru KUNINO
                                                http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

/* DF ROBOT 製液晶を使用する場合は上記を下記に入れ替えてください。
    #include <LiquidCrystalDFR.h>
    LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
*/

#define RX_MAX  17                              // 受信データ最大値
char rx_data[RX_MAX];                           // 受信データの格納用の文字列変数

void setup(){
    bt_init();                                  // bt_rn42.ino内に記述した初期化処理
}

void loop(){
    unsigned char in;                           // GPIO入力値の計算用
    
    /* 接続処理 */
    lcd_cls(1);
    lcd.print("Calling ");
    if( !bt_cmd_mode('#') ){                    // ローカルコマンドモードへの移行を実行
        bt_error("FAILED to open");
    }
    bt_repeat_cmd("C","%CONNECT",8);            // 接続するまで接続コマンドを繰り返す
    lcd.print("DONE");
    if( bt_cmd_mode('$') ){                     // リモートコマンドモードへの移行
        /* GPIOの読み取り */
    //  bt_cmd("S@,8800");                      // GPIOポート3と7を入力に設定(初期値)
        bt_cmd("G&");                           // GPIOポートの読み取り
        lcd_cls(0);
        lcd.print("IN3=");
        for(int i=0;i<2;i++){
            in = (unsigned char)rx_data[1-i];   // 文字コードを変数inに代入
            if( in >= '0' && in <= '9' ) in -= (unsigned char)'0';        // 数値へ変換
            else if( in >= 'A' && in <= 'F' ) in -= (unsigned char)'A'-10;// 数値へ変換
            else in = 0;
            in = (in>>3) & 0x01;                // 変数inに入力値を代入。bitRead(in,3)
            lcd.print(in,BIN);
            if(i==0) lcd.print(", IN7=");
        }
    }
    
    /* 切断処理 */
    bt_cmd("K,1");                              // 通信の切断処理
    lcd_cls(1);
    lcd.print("Disconnected");
    delay(5000);                                // 待ち時間
}
