/***************************************************************************************
BLEタグ内の属性データを読み取る

                                                        Copyright (c) 2016 Wataru KUNINO
***************************************************************************************/
#include <stdio.h>                                              // 標準入出力ライブラリ
#include <stdlib.h>                                             // system命令に使用
#include <string.h>                                             // strlen命令に使用
#include <ctype.h>                                              // isxdigit用

/* 補助関数 */
int a2hex(char *s){                                             // 16進数2桁を数値に変換
    int i=0,ret=0;
    while(i<2){                                                 // 2桁分の繰り返し処理
        ret *=16;                                               // 前回の桁を繰り上げ
        if(isdigit(s[i])) ret += s[i]-'0';                      // 0～9の文字を数値に
        else ret += s[i] + 10 - 'a';                            // a～fの文字を数値に
        i++;                                                    // 次の桁へ
    }
    if(ret < 0 || ret > 255 ) ret = -1;                         // 適切な範囲かどうか
    return ret;                                                 // 変換後の数値を応答
}

/* メイン関数 */
int main(int argc,char **argv){                                 // ここからがメイン
    char  s[256];
    FILE  *fp;
    int i;
    char c;
    
    /* 入力値の確認 */
    if(argc != 2 || strlen(argv[1]) != 17){                     // MACアドレス文字長確認
        fprintf(stderr,"usage: %s MAC_Address\n",argv[0]);      // 入力誤り表示
        return -1;                                              // 終了
    }
    for(i=0;i<17;i++){                                          // MACアドレス形式の
        if( (i+1)%3 == 0 ){                                     // 3,6,9,12,15文字目が
            if( argv[1][i] != ':' ) break;                      // 「:」であることを確認
        }else{                                                  // その他の文字が
            if(isxdigit(argv[1][i])==0) break;                  // 16進数である事を確認
        }
    }
    if(i!=17){                                                  // for中にBreakした時
        fprintf(stderr,"Invalid MAC Format (%s,%d)\n",argv[1],i);    // 形式誤り表示
        return -1;                                              // 終了
    }
    
    /* 主要部 */
    sprintf(s,"sudo gatttool -b %s --char-read -a 0x0003",argv[1]); // コマンドの作成
    fp = popen(s,"r");                                          // コマンドの実行
    if( fp==NULL ){                                             // 開始できなかった時
        fprintf(stderr,"System Command Error!\n");              // エラー表示
        return -1;                                              // 終了
    }
    fgets(s,256,fp);                                            // 受信データの取得
    printf("%s",s);                                             // 受信データの表示
    pclose(fp);                                                 // popenを閉じる
    for(i=33;i<65;i+=3){                                        // 34文字目から
        c=a2hex(&s[i]);                                         // 受信した数値をcに代入
        if( isprint(c) ) putchar(c);                            // 数値を文字にして表示
        else break;                                             // 文字以外なら抜ける
    }
    putchar('\n');                                              // 改行を出力
    return 0;
}
