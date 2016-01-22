/***************************************************************************************
Bluetoothモジュール RN-42XVP搭載の照度センサを製作してリモートで照度値を取得します。

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
    unsigned char in;                           // アナログ入力値
    int adc[3];                                 // アナログ入力値の保持用
    
    /* 接続処理 */
    lcd_cls(1);
    lcd.print("Calling ");
    if( !bt_cmd_mode('#') ){                    // ローカルコマンドモードへの移行を実行
        bt_error("FAILED to open");
    }
    bt_repeat_cmd("C","%CONNECT",8);            // 接続するまで接続コマンドを繰り返す
    lcd.print("DONE");
    if( bt_cmd_mode('$') ){                     // ローカルコマンドモードへの移行

        /* ADCの読み取り */
        for(int j=0;j<3;j++){                   // 3回の読み取りを実行
            adc[j]=0;
            bt_cmd("A");                        // ADC1ポートの読み取り
            for(int i=5;i<8;i++){
                if( rx_data[i]==',' ) break;    // 16進数値に続くカンマを検出したら終了
                in = (unsigned char)rx_data[i]; // 大きい桁の文字コードを変数inに代入
                if( in >= '0' && in <= '9' ) in -= (unsigned char)'0';          // 値へ
                else if( in >= 'A' && in <= 'F' ) in -= (unsigned char)'A'-10;  // 値へ
                else break;
                adc[j] *= 16;                   // これまでの値を16倍する
                adc[j] += in;                   // 読み取った数値を加算
            }
        }
        /* 中央値の検索 */
        in=0;
        if( adc[1] <= adc[0] && adc[0] <= adc[2] ) in = 0;
        if( adc[1] >= adc[0] && adc[0] >= adc[2] ) in = 0;
        if( adc[0] <= adc[1] && adc[1] <= adc[2] ) in = 1;
        if( adc[0] >= adc[1] && adc[1] >= adc[2] ) in = 1;
        if( adc[0] <= adc[2] && adc[2] <= adc[1] ) in = 2;
        if( adc[0] >= adc[2] && adc[2] >= adc[1] ) in = 2;
        lcd_cls(0);
        lcd.print("AD1=");
        lcd.print(adc[in],HEX);
        lcd.print(' ');
        lcd.print((float)adc[in]*3.03,0);
        lcd.print(" Lux");
    }
    /* 切断処理 */
    bt_cmd("K,1");                              // 通信の切断処理
    lcd_cls(1);
    lcd.print("Disconnected");
    delay(5000);                                // 待ち時間
}
