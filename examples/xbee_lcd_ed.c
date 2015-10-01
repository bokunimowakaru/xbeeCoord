/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。
液晶表示プログラム　16文字×１行／８文字×２行用
					(その他にもdefineの変更で対応可能です。)

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
Coordinatorから送られてきたテキストをキャラクタＬＣＤに表示するための
表示デバイス（XBee End device AT＋Ｈ８マイコン＋キャラクタＬＣＤ）側の
プログラムです。

赤外線送信機能はH8 3694 専用です。

Coordinator側は xbee.cを使用し、xbee_putstr("hello\n");のように文字を
送信します。（本プログラムには含まれていません。）

・テキストを受信すると改行までの続き文字をＬＣＤへ出力します。
・表示しきれない場合はスクロール表示します。
・エスケープコマンド(0x1B)を受信するとコマンドに応じてLEDの点灯などを
　行います。
　（esc_commandとesc_valueに応じてプログラムを追記して下さい。）
・バッテリ駆動も考慮してスリープモードで動作します。
　より省電力なスタンバイモードでも動作しますが、動作に合わせてLEDが
　点滅してしまいます。
・注意：
　AKI-H8/3664Fの裏面のRS-232CドライバICが多くの電流を消費しています。
　AKI-H8/3664BPであればRS-232CドライバICが簡単に外せます。
　
*/
/*********************************************************************
ハードウェア

	μP		H8 3664 / 3694 以下のいずれか
			・AKI-H8/3664BP(SDIP)タイニーマイコンキット
			・AKI-H8/3664F(QFP) タイニーマイコンキット
			・AKI-H8/3694F(QFP) タイニーマイコンキット

	LCD		16文字×１行：Ｓ１０５５１Ｄ
			16文字×１行：ＤＭＣ１６１１７Ａ
			８文字×２行：ＡＣＭ０８０２Ｃ－ＮＬＷ－ＢＢＨ

			表示アドレス
			１～８文字：0x00～0x07
			９～16文字：0x40～0x47

	XBee	XBee ZigBee (Series 2)
			Firmware = END DEVICE AT

					AKI-H8　　　　　　液晶モジュール
					(CN1)　　　　　　 (HD44780搭載)
					━━┓　　　　　　┏━━━━━┓　　　　┯ 5V
					　　┃P50　　　　 ┃　　　　　┃　　　　｜
					　14┠→─────┨DB4　　Vdd┠───●┘
					　　┃P51　　　　 ┃　　　　　┃　　　｜
					　15┠→─────┨DB5 　　　┃　　　＜
					　　┃P52　　　　 ┃　　　　　┃　┌→＞
					　16┠→─────┨DB6　　VLC┠─┘　＜ 10k
					　　┃P53　　　　 ┃　　　　　┃　　　｜
					　17┠→─────┨DB7　DB0-3┠─┐　｜
					　　┃P54　　　　 ┃　　　　　┃　｜　｜
					　18┠→─────┨E　　　R/W┠─●　｜
					　　┃P55　　　　 ┃　　　　　┃　｜　｜
					　19┠→─────┨RS　　 Vss┠─●─●┐
					　　┃　　　　　　┗━━━━━┛　　　┯┿┯
					　　
					(CN2)　　　　　　　XBee
					━━┓　　　　　　┏━━━━━┓　　┯ 3.3V
					　　┃RXD　　 DOUT┃　　　　　┃Vdd ｜
					　18┠←─────┨2　　　　1┠──┘
					　　┃TXD 抵抗 DIN┃　　　　　┃
					　19┠→─□□──┨3 　　　　┃
					　　┃IRQ0 分圧 ON┃　　　　　┃GND
					　20┠←─────┨13　　　10┠──┐
					　　┃　　　　　　┃　　　　　┃　┯┿┯
					　　┃
					　　┃　　　　　　　　Diode
					　　┃　　　　　┌──┨＜───┐
					　　┃　　　　　｜　　　　　　　｜
					　　┃　　　　　｜ 圧電スピーカ ｜
					　　┃TMOV　　　｜┏━━━━━┓｜
					　 7┠→─┨┠─●┨　｜○｜　┠●─┐
					　　┃　　22uF　　┗━━━━━┛　┯┿┯

					抵抗分圧 Series 10K ＋ Shunt 15k
					
			GPIO	P10(CN1 20)	LED0(Green)
					P11(CN1 21)	LED1(Green)
					P12(CN1 22)	LED2(Green)
					P15(CN2 21)	LED3(Green)
					P16(CN2 22)	LED4(Green)
					P80(CN2  9)	LED5(yellow)
					P81(CN2 10)	LED6(yellow)
					P82(CN2 11)	LED7(yellow)
					P83(CN2 12)	LED8(red)
					P84(CN2 13)	LED9(red)
					
					P85(CN2 14)	IR-LED
					P86(CN2 15)	IR-VCC
					P87(CN2 16)	LED(警告用)
					P17(CN2 23)	IR-IN


*********************************************************************/

/*
通信仕様(XBee UART)

	レベルメータ表示

		[ESC] [L] [0～9,A]			レベルメータの表示レベル(0～10段)
		[ESC] [L] [0x00～0x0A]		レベルメータの表示レベル(0～10段)
		[ESC] [L] [0X0E,'E']		警告灯の表示消去
		[ESC] [L] [0X0F,'F']		警告灯の表示

	ブザー出力

		[ESC] [A] [0～9] 			アラート音を鳴らす(0～9回),0＝OFF
		[ESC] [B] [0～9] 			ベル音を鳴らす(0～10回),0＝OFF
		[ESC] [C] [0～9]			チャイム音(1回)
		
	赤外線リモコン
	
		[ESC][I][R][長さ][データ]	長さ、データはバイナリ
		[ESC][I][R][0xFF]			赤外線の受光モードへ

*/

// #include <3694.h>
#ifdef H3694
	#include <3694.h>
#else
	#include <3664.h>
#endif
#define	TARGET_H8_H
#include "../lib/lcd_h8.c"
#define	LCD_H8_H

			//   01234567
#define	NAME	"XBee LCD"
#define	VERSION	" Vr.1.40"

#define LCD_CHRS	8				// 液晶の文字数(16文字×1行でも8文字)
#define LCD_ROWS	2				// 液晶の行数(16文字×1行でも2行)

#define	LED0_OUT	IO.PDR1.BIT.B0	// P10(CN1 20)	LED0(Green)
#define	LED1_OUT	IO.PDR1.BIT.B1	// P11(CN1 21)	LED1(Green)
#define	LED2_OUT	IO.PDR1.BIT.B2	// P12(CN1 22)	LED2(Green)
#define	LED3_OUT	IO.PDR1.BIT.B5	// P15(CN2 21)	LED3(Green)
#define	LED4_OUT	IO.PDR1.BIT.B6	// P16(CN2 22)	LED4(Green)
#define	LED5_OUT	IO.PDR8.BIT.B0	// P80(CN2  9)	LED5(yellow)
#define	LED6_OUT	IO.PDR8.BIT.B1	// P81(CN2 10)	LED6(yellow)
#define	LED7_OUT	IO.PDR8.BIT.B2	// P82(CN2 11)	LED7(yellow)
#define	LED8_OUT	IO.PDR8.BIT.B3	// P83(CN2 12)	LED8(red)
#define	LED9_OUT	IO.PDR8.BIT.B4	// P84(CN2 13)	LED9(red)
#define	LED_OUT_PRT	IO.PDR8.BIT.B7	// P87(CN2 16)	LED(警告用)
#define	IR_OUT		IO.PDR8.BIT.B5	// P85(CN2 14)	IR-LED
#define	IR_VCC		IO.PDR8.BIT.B6	// P86(CN2 15)	IR-VCC
#define	IR_IN		IO.PDR1.BIT.B7	// P17(CN2 23)	IR-IN
#define	IR_IN_OFF	1				// 赤外線センサ非受光時の入力値
#define	IR_IN_ON	0				// 赤外線センサ受光時の入力値
#define	IR_OUT_OFF	0				// 赤外線センサ非発光時の入力値
#define	IR_OUT_ON	1				// 赤外線センサ発光時の入力値
#define	IR_SYNC_WAIT	4095			// 同期長の２倍以上 最大32235
#define	IR_DATA_SIZE	16				// データ長(byte),4の倍数

/* H3664 16MHz用
#define FLASH_AEHA_WAIT		5		// 13us(38kHz)分の空ループ初期値
#define FLASH_AEHA_TIMES	18		// シンボル区間の点滅回数
#define FLASH_NEC_WAIT		5		// 13us(38kHz)分の空ループ初期値
#define FLASH_NEC_TIMES		22		// シンボル区間の点滅回数
#define FLASH_SIRC_WAIT		5		// 12.5us(40kHz)分の空ループ初期値
#define FLASH_SIRC_TIMES	24		// シンボル区間の点滅回数
*/
#define FLASH_AEHA_WAIT_H	7		// 13us(38kHz)分の空ループ初期値
#define FLASH_AEHA_WAIT_L	7		// 13us(38kHz)分の空ループ初期値
									// 6=(41kHz) 6.5=(39kHz) 7=(37kHz)
#define FLASH_AEHA_TIMES	16		// シンボル区間の点滅回数
									// 17=(5263us) 16=(5023us)
/*
照明用リモコン(Panasonic)
	Wave Length  = 27.073[us]
	Carrier freq = 36,936[Hz]
	IR Mode = AEHA
	Data Length  = 40 bits
	SYNC Duration= 5304 us (ON: 3508 us)/(OFF: 1796 us)
	DATA Duration= 47.705 msec.
	TOTAL Time   = 53.009 msec.
	SYMBOL Rate  = 754 Baud (bps)
	DATA Rate    = 838 Baud (bps)
	byte light_down[5]={0x2C,0x52,0x9,0x2B,0x22};
テレビ用リモコン(Sharp)
	Wave Length  = 26.253[us]
	Carrier freq = 38,090[Hz]
	IR Mode = AEHA
	Data Length  = 48 bits
	SYNC Duration= 5081 us (ON: 3399 us)/(OFF: 1682 us)
	DATA Duration= 59.523 msec.
	TOTAL Time   = 64.603 msec.
	SYMBOL Rate  = 787 Baud (bps)
	DATA Rate    = 806 Baud (bps)
	byte tv_vol_down[6]={0xAA,0x5A,0x8F,0x12,0x15,0xE1};
*/
#define	SCI_SIZE	256				// シリアルデータ長(大きすぎるとRAMが不足する)

typedef unsigned char byte;
byte	LED_OUT = 0 ;				// 警告用LED
byte	TIMER_COUNT	=	0	;		//ＲＴＣカウント用250ms単位
char	strings[LCD_ROWS+1][LCD_CHRS+1]	;
byte	lcd_str_length	=	0	;

byte	ir_len = 0x00;
byte	ir_data[IR_DATA_SIZE];
enum Ir_Mode {NA,AEHA,NEC,SIRC,ERROR};	// 赤外線モードの型

byte	alert_value = 0;

#define IR_READ_S_H
#include "../lib/ir_read_simple.c"
/*
int ir_sens(int det);
int ir_read_simple(unsigned char *data, enum Ir_Mode mode);
*/

#define IR_SEND_S_H
#include "../lib/ir_send_simple.c"
/*
void ir_flash(int times);
void ir_wait(int times);
void ir_send_simple(byte *data, int data_len, enum Ir_Mode mode);
*/

/* 割り込み許可 */
void interrupt_enable(void){
	EI;
}

/* 割り込み許可 */
void interrupt_disable(void){
	DI;
}

void timer_interrupt_reset(void){
	IRR1.BIT.IRRTA=0;			// タイマーA割込フラグのリセット
}

byte port_xb_interrupt_status(void){
	return( (byte)IO.PDR1.BIT.B4 );
}

byte port_ir_interrupt_status(void){
	return( (byte)IO.PDR1.BIT.B7 );
}

void port_xb_interrupt_reset(void){
	IEGR1.BIT.IEG0 = 1;				// IRQ0端子(P14)のエッジを検出（立ち上がり=1)
	IRR1.BIT.IRRI0 = 0;				// IRQ0端子(P14)の割込みフラグのリセット 
	IENR1.BIT.IEN0 = 1;				// IRQ0端子(P14)の割込みを利用可能にする 
}

void port_ir_interrupt_reset(void){
	while( port_ir_interrupt_status() != 1 );
	IEGR1.BIT.IEG3 = 0;				// IRQ3端子(P17)のエッジを検出（立ち下がり=0)
	IRR1.BIT.IRRI3 = 0;				// IRQ3端子(P17)の割込みフラグのリセット 
	IENR1.BIT.IEN3 = 1;				// IRQ3端子(P17)の割込みを利用可能にする 
}

void port_init(void){
	/*ポート1         76543210*/
	IO.PMR1.BYTE  = 0b10010000;     // モード(P14)     入出力=0  IRQ=1
	IO.PCR1       = 0b01100111;     // 入出力設定       入力  =0  出力  =1  B3=リザーブ
	IO.PUCR1.BYTE = 0b10010000;     // プルアップ(P14) しない=0  する  =1
	IO.PDR1.BYTE  = 0b00000000;     // アクセス         Ｌ出力=0  Ｈ出力=1

	DI;
	port_xb_interrupt_reset();
	port_ir_interrupt_reset();
	IENR1.BIT.IEN3 = 0;				// IRの割り込み禁止

	/*ポート8         76543210*/	// 秋月 H8 Tiny I/O BOARD TERA2 [K-00207]用
	IO.PCR8       = 0b01111111;     // 入出力設定       入力  =0  出力  =1
	IO.PDR8.BYTE  = 0b00000000;     // アクセス         Ｌ出力=0  Ｈ出力=1
	
	/*ポート8         76543210*/	// 秋月 H8 Tiny I/O BOARD TERA2 [K-00207]用
	IO.PCR8       = 0b11111111;     // 入出力設定       入力  =0  出力  =1
	IO.PDR8.BYTE  = 0b00000000;     // アクセス         Ｌ出力=0  Ｈ出力=1
}

/* タイマー初期化 */
void timer_count_init(void){
	DI;

	/* タイマーA 8bit 文字表示の送りタイミング用 */
	TA.TMA.BYTE     =0b00001010;  /* 時計用クロック  */
	/*                 |  |||-|__ 分周比 000～111(高速時) 000～011(低即時)
	         固定値 ___|--||             000:0.5120 sec.  000 1000 msec.
	                       |             001:0.2560 sec.  001  500 msec.
	 高速=0, 低速=1 _______|             100:0.0160 sec.  010  250 msec.
	                                     111:0.0005 sec.  011   31.25 msec.
	*/

	IRR1.BIT.IRRTA  =0;           /* タイマーＡ割込みフラグのリセット */
	IENR1.BIT.IENTA =1;           /* タイマーＡ割込みを利用可能にする */
	
	/* スタンバイモード(全てオフ)
	SYSCR1.BIT.SSBY =1;			// スタンバイ=1／スリープ=0
	SYSCR2.BIT.DTON =0;			// ダイレクトトランスファONフラグ
	*/
	
	/* サブスリープモード */
	SYSCR1.BIT.SSBY =0;			// スタンバイ=1／スリープ=0
	SYSCR2.BIT.DTON =0;			// ダイレクトトランスファONフラグ
	SYSCR2.BIT.SMSEL =1;		// スリープモード
	SYSCR2.BIT.LSON =0;			// ロースピードONフラグ
}

byte timera(void){
	return( (byte)TA.TCA );
}

/* タイマーV 8bit ブザー用 */
void timerv(void){
	TV.TCRV0.BYTE   =0b00010011;
	/*                 ||||||||__ 分周比 最下位はTCSV1。とともに設定する
	                   ||||||____ タイマー用のクロック = 0 内部クロック
	                   |||||_____ CCLR カウンタクリア = 10 (コンペアマッチB)
	                   |||_______ ORIE オーバフローIE = 0
	                   ||________ CMIE コンペアマッチIE(B,A) = 00
	*/
	TV.TCSRV.BYTE   =0b00001001;
	/*                 ||| |__|__ OS 1001 マッチAでLow  マッチBでHign __/~~
	                   |||           0110 マッチAでHign マッチBでLow  ~~\__
	                   |||_______ OVF オーバフローフラグ
	                   ||________ CMF コンペアマッチフラグ(B,A) 
	*/
	TV.TCRV1.BYTE   =0b11100010; // TRGV端子入力設定 (データシート P11-6)
	/*                    ||| |__ ※分周比の最下位
	                      |||       010:1/4   5 MHz   (4MHz)
	                      |||       011:1/8   2.5 M   (2MHz)
	                      |||       100:1/16  1.25M   (1MHz)
	                      |||       101:1/32   625k   (500k)
	                      |||       110:1/64  312.5k  (250k) 1/100で3125Hz
	                      |||       111:1/128 156.25k (125k) 1/100で1563Hz
	                      |||____ TRGE
	                      ||_____ TVEG
	*/
	TV.TCORA=100;
	TV.TCORB=200;
}
void timerv_off(void){
	TV.TCRV0.BYTE   =0b00000000;
	TV.TCSRV.BYTE   =0b00000000;
	TV.TCRV1.BYTE   =0b11100010;
}

/* ミリ秒待ち250ms 入力範囲＝4～250 ms */
void wait_millisec_250( byte ms ){
	byte counter;
	if( ms < 0x04 ) ms = 0x04;
	counter = timera() + (byte)( ms>>2 );
	if( counter == timera() ) counter++;
	while( counter != timera() );
}

/* ミリ秒待ち 入力範囲＝4～30,000 ms */
void wait_millisec( const int ms ){
	byte loops;
	if( ms < 250 ){
		wait_millisec_250( (byte)ms );
	}else{
		loops = (byte)( ms / 250);
		wait_millisec_250( (byte)(ms-loops*250) );
		while( loops > 0x00 ){
			wait_millisec_250( 0xFA );
			loops--;
		}
	}
}

/* LED 点灯 */
void led(byte value){
	switch( (value & 0x0F) ){
		case 0x00:	case '0':
			LED0_OUT=0;	LED1_OUT=0;	LED2_OUT=0;	LED3_OUT=0;	LED4_OUT=0;
			LED5_OUT=0;	LED6_OUT=0;	LED7_OUT=0;	LED8_OUT=0;	LED9_OUT=0;
			break;
		case 0x01:	case '1':
			LED0_OUT=1;	LED1_OUT=0;	LED2_OUT=0;	LED3_OUT=0;	LED4_OUT=0;
			LED5_OUT=0;	LED6_OUT=0;	LED7_OUT=0;	LED8_OUT=0;	LED9_OUT=0;
			break;
		case 0x02:	case '2':
			LED0_OUT=1;	LED1_OUT=1;	LED2_OUT=0;	LED3_OUT=0;	LED4_OUT=0;
			LED5_OUT=0;	LED6_OUT=0;	LED7_OUT=0;	LED8_OUT=0;	LED9_OUT=0;
			break;
		case 0x03:	case '3':
			LED0_OUT=1;	LED1_OUT=1;	LED2_OUT=1;	LED3_OUT=0;	LED4_OUT=0;
			LED5_OUT=0;	LED6_OUT=0;	LED7_OUT=0;	LED8_OUT=0;	LED9_OUT=0;
			break;
		case 0x04:	case '4':
			LED0_OUT=1;	LED1_OUT=1;	LED2_OUT=1;	LED3_OUT=1;	LED4_OUT=0;
			LED5_OUT=0;	LED6_OUT=0;	LED7_OUT=0;	LED8_OUT=0;	LED9_OUT=0;
			break;
		case 0x05:	case '5':
			LED0_OUT=1;	LED1_OUT=1;	LED2_OUT=1;	LED3_OUT=1;	LED4_OUT=1;
			LED5_OUT=0;	LED6_OUT=0;	LED7_OUT=0;	LED8_OUT=0;	LED9_OUT=0;
			break;
		case 0x06:	case '6':
			LED0_OUT=1;	LED1_OUT=1;	LED2_OUT=1;	LED3_OUT=1;	LED4_OUT=1;
			LED5_OUT=1;	LED6_OUT=0;	LED7_OUT=0;	LED8_OUT=0;	LED9_OUT=0;
			break;
		case 0x07:	case '7':
			LED0_OUT=1;	LED1_OUT=1;	LED2_OUT=1;	LED3_OUT=1;	LED4_OUT=1;
			LED5_OUT=1;	LED6_OUT=1;	LED7_OUT=0;	LED8_OUT=0;	LED9_OUT=0;
			break;
		case 0x08:	case '8':
			LED0_OUT=1;	LED1_OUT=1;	LED2_OUT=1;	LED3_OUT=1;	LED4_OUT=1;
			LED5_OUT=1;	LED6_OUT=1;	LED7_OUT=1;	LED8_OUT=0;	LED9_OUT=0;
			break;
		case 0x09:	case '9':
			LED0_OUT=1;	LED1_OUT=1;	LED2_OUT=1;	LED3_OUT=1;	LED4_OUT=1;
			LED5_OUT=1;	LED6_OUT=1;	LED7_OUT=1;	LED8_OUT=1;	LED9_OUT=0;
			break;
		case 0x0A:	case 'A':
			LED0_OUT=1;	LED1_OUT=1;	LED2_OUT=1;	LED3_OUT=1;	LED4_OUT=1;
			LED5_OUT=1;	LED6_OUT=1;	LED7_OUT=1;	LED8_OUT=1;	LED9_OUT=1;
			break;
		case 0x0E:	case 'E':
			LED_OUT=0;	LED_OUT_PRT=0;
			break;
		case 0x0F:	case 'F':
			LED_OUT=1;
			break;
		default:
			break;
	}
}

/* beep 音 */
void beep(byte value){
	byte i;
	byte time=0;
	
	if( value >= (byte)'0' && value <= (byte)'9' ) value -= (byte)'0';
	if( value > 10 ) value = 10 ;
	if( value != 0x00 && value != 255 ){
		for( i = 0 ; i < value ; i++){
			time = timera();
			timerv();
			TV.TCORA =  28;		// 
			TV.TCORB =  56;		// 5594Hz (7F)
			time += 8;
			while( timera() != time );
			TV.TCORA =  25;		// 
			TV.TCORB = 112;		// 2797Hz (6F)
			time += 8;
			while( timera() != time );
			TV.TCORA = 112;		// 
			TV.TCORB = 224;		// 1399Hz (5F)
			time += 16;
			while( timera() != time );
		}
	}
	timerv_off();
}

void beep_ir( void ){
	byte time;
	TV.TCORA=100;
	TV.TCORB=100;
	timerv();
	TV.TCORB = 89;				// 0x59 3510Hz(6A)
	TV.TCORA = 2;				// 0x02
	time = timera() + 8;
	if( time < 8 ) while( timera() >= 8);
	while( timera() <= time );
	timerv_off();
}

/*
byte BEEP_FREQ;
byte BEEP_DUTY;
void beep_freq( byte freq ){
	byte time;
	BEEP_FREQ = freq;
	timerv();
	TV.TCORB = BEEP_FREQ;
	TV.TCORA = BEEP_DUTY;
	time = timera() -1;
	while( timera() != time );
	//timerv_off();
}
void beep_duty( byte duty ){
	byte time;
	BEEP_DUTY = duty;
	timerv();
	TV.TCORB = BEEP_FREQ;
	TV.TCORA = BEEP_DUTY;
	time = timera() -1;
	while( timera() != time );
	//timerv_off();
}
*/

/* alert 音 0.5秒 */
void alert(byte value){
	byte i,j;
	byte time=0;
	
	if( value >= (byte)'0' && value <= (byte)'9' ) value -= (byte)'0';
	if( value > 10 ) value = 10 ;
	if( value != 0x00 ){
		for( j=0 ; j< value ; j++){
			time = timera();
			timerv();
			for( i = 28 ; i < 224 ; i += 8 ){
				TV.TCORB = i;
				TV.TCORA = i/2;
				time += 8;
				while( timera() != time );
			}
			timerv_off();
		}
	}
	timerv_off();
}

/* chime 音 2秒間 */
/*
	   分周比 1/64
	    1108.731×2Hz 1/141
	     880.000×2Hz 1/178
	   
	    156.25k (125k) 1/100で1563Hz
*/
#define CHIME_T 6
void chime(byte value){
	byte i,j;
	byte time=0;
	
	if( value >= (byte)'0' && value <= (byte)'9' ) value -= (byte)'0';
	if( value > 10 ) value = 10 ;
	if( value != 0x00 ){
		/* 単音版(ノイズが乗る)
		for( j=0 ; j< value ; j++){
			if( value != 255 ){
				time = timera();
				timerv();
				TV.TCORB = 141;		// 2216Hz (6C#)
				TV.TCORA = 70;
				time += 192;
				while( timera() != time );
			//	TV.TCORB = 141;
			//	TV.TCORA = 70;
				time += 64;
				while( timera() != time );
				//for( i=1 ; i<=6 ;i++ ){
				//	TV.TCSRV.BIT.CMFA=0;
				//	while(TV.TCSRV.BIT.CMFA==0);
				//	TV.TCORA = (byte)(70/(byte)(2^i));
				//	time += 21;
				//	while( timera() != time );
				//}
				while( timera() != time );
				TV.TCORB = 178;		// 1755Hz (5A)
				TV.TCORA = 89;	
				time += 192;
				while( timera() != time );
			//	TV.TCORB = 178;
			//	TV.TCORA = 89;
				time += 64;
				while( timera() != time );
				timerv_off();
			}
		}
		*/
		/* 和音版（ノイズがごまかせる） */
		for( j=0 ; j< value ; j++){
			if( value != 255 ){
				time = timera();
				timerv();
				for( i=0 ; i<5 ; i++){
					TV.TCORB = 141;		// 2216Hz (6C#)
					TV.TCORA = 63;
					time += CHIME_T;
					while( timera() != time );
					TV.TCORB = 70;		// 4463Hz (7C#)
					TV.TCORA = 32;
					time += CHIME_T;
					while( timera() != time );
				}
				for( i=0 ; i<15 ; i++){
					TV.TCORB = 141;		// 2216Hz (6C#)
					TV.TCORA = 63 - (byte)(63.*(float)i/14. );
				//	TV.TCORA = 70/(i*(CHIME_T/4)+2)+1;
					time += CHIME_T;
					while( timera() != time );
					TV.TCORB = 70;		// 4463Hz (7C#)
					TV.TCORA = 32 - (byte)(32.*(float)i/14.);
				//	TV.TCORA = 35/(i*(CHIME_T/4)+2)+1;
					time += CHIME_T;
					while( timera() != time );
				}
				for( i=0 ; i<5 ; i++){
					TV.TCORB = 178;		// 1755Hz (5A)
					TV.TCORA = 80;	
					time += CHIME_T;
					while( timera() != time );
					TV.TCORB = 89;		// 35103Hz (6A)
					TV.TCORA = 40;
					time += CHIME_T;
					while( timera() != time );
				}
				for( i=0 ; i<15 ; i++){
					TV.TCORB = 178;		// 1755Hz (5A)
					TV.TCORA = 80 - (byte)(80.*(float)i/14. );
				//	TV.TCORA = 89/(i*(CHIME_T/4)+2)+1;
					time += CHIME_T;
					while( timera() != time );
					TV.TCORB = 89;		// 35103Hz (6A)
					TV.TCORA = 40 - (byte)(40.*(float)i/14. );
				//	TV.TCORA = 44/(i*(CHIME_T/4)+2)+1;
					time += CHIME_T;
					while( timera() != time );
				}
			}
		}
	}
	timerv_off();
}


/* シリアルの初期化 */
char sci_tx[SCI_SIZE], sci_rx[SCI_SIZE];	// PCは受信バッファを使用しない
byte sci_init( void ){
	SCI3_INIT(br9600, sci_tx, SCI_SIZE, sci_tx, SCI_SIZE);
	// EI;									// 割り込み許可
	return(1);
}

/* シリアル送受信バッファクリア */
void sci_clear(void){
	SCI3_IN_DATA_CLEAR();
	SCI3_OUT_DATA_CLEAR();
}

byte sci_read_value;

/* シリアル受信 */
byte sci_read(byte timeout){
	byte timer;
	sci_read_value = 0;
	timer = timera() + (timeout)+1;		// timeout[ms] = timer/256*1000
	while( timer != timera() && SCI3_IN_DATA_CHECK() < 1 );
	if( SCI3_IN_DATA_CHECK() ) sci_read_value=(byte)SCI3_IN_DATA();
	return( sci_read_value );
}

/* シリアル受信バッファの確認 */
byte sci_read_check(void){
	return( sci_read_value );
}

/* シリアル送信バッファが空になるのを待つ */
byte sci_write_check(void){
	byte timer;
	timer = timera() + 0x7F;	// timeout = 500ms
	while( (timer != timera()) && ( SCI3_OUT_DATA_CHECK() < 1 ) );
	return( (byte)SCI3_OUT_DATA_CHECK() );
}

/* シリアル送信 */
void sci_write( char *data, byte len ){
	SCI3_OUT_STRINGB( data , (short)len );
}

/* string/byte操作 */
void strcopy(char *s1, const char *s2){	// string.hが含まれる場合はstrcpyを使用
    while( *s2 != 0x00 ) *s1++ = *s2++;
    *s1 = 0x00;
}
void bytecpy(byte *s1, const byte *s2, byte size){
	byte i;
	for(i=0; i< size ;i++ ){
		s1[i] = s2[i];
	}
}
byte bytecmp(byte *s1, const byte *s2, byte size){	// strcmpのような大小比較は無い
	byte i=0;										// 同一なら0を応答
	for(i=0; i< size ;i++ ){
		if( s1[i] != s2[i] ) return(1);
	}
	return(0);
}

/*********************************************************************
表示

*********************************************************************/

/*	割り込み処理	0.25秒ごとに呼び出される	*/
char str_putch_nc( byte c ){
	if( (c >= (byte)' ' && c <= 0x7E)||(c >= 0xA1 && c <= 0xFC )){
		return( (char)c );
	}else{
		return( ' ' );
	}
}
void lcd_puts_nc(void){
	byte i,j;
	for( i=0 ; i < LCD_ROWS ; i++){
		switch(i){
			case 0:
				lcd_goto(LCD_ROW_1);
				break;
			case 1:
				lcd_goto(LCD_ROW_2);
				break;
			case 2:
				lcd_goto(LCD_ROW_3);
				break;
			case 3:
				lcd_goto(LCD_ROW_4);
				break;
		}
		for( j=0 ; j < LCD_CHRS ; j++){
			lcd_putch( strings[i][j] );
		}
	}
}
void str_shift(){
	int i,j;
	for( i=0 ; i < LCD_ROWS ; i++){
		for( j=0 ; j < (LCD_CHRS-1) ; j++){
			strings[i][j] = strings[i][j+1];
		}
		if( i < (LCD_ROWS-1) ){
			strings[i][LCD_CHRS-1] = strings[i+1][0];
		}
	}
}

byte esc_command	= 0x00;
byte esc_value		= 0x00;
byte esc_done		= 0xFF;

/* タイマー割り込み(H8システムからのコール) */
void int_timera(void){
	byte i,j;
	byte c;
	
	if( esc_done ){				// エスケープコマンド実行中(0x00)の場合は以下を実行せずに処理を抜ける
		TIMER_COUNT++;				// タイマーのカウントアップ
		if( TIMER_COUNT >= 240 ){	// 1分に1回の処理
			TIMER_COUNT = 0;
		}
		if( LED_OUT || alert_value ) LED_OUT_PRT = (TIMER_COUNT & 0x01 );
		
		if( alert_value && ( TIMER_COUNT % 6) == 0) {
			lcd_cls();
			lcd_goto(LCD_ROW_1);
			lcd_putstr("#### ｹｲ");
			lcd_goto(LCD_ROW_2);
			lcd_putstr("ﾎｳ ####");
			alert( 0x01 );
		}else{	
			c = sci_read( 2 );					// sciからreadする。値はunsigned char
			if( c == 0x1B){			// エスケープ(27)
				esc_done=0x00;
			}else if( c ){
				if( c == 0x0A || c == 0x0D ){	// 改行の時
					lcd_str_length = 0x00;
				}else{
					if( lcd_str_length >= (LCD_ROWS * LCD_CHRS) ){
						str_shift();
						strings[LCD_ROWS-1][LCD_CHRS-1] = str_putch_nc( c );	// 文字に変換してバッファに代入
					}else{
						if( lcd_str_length == 0){
							for( i=0 ; i < LCD_ROWS ; i++){
								for( j=0 ; j < LCD_CHRS ; j++){
									strings[i][j] = ' ';
								}
							}
						}
						i = ( lcd_str_length / LCD_CHRS );
						strings[i][ lcd_str_length - i * LCD_CHRS ] = str_putch_nc( c );	// 文字に変換してバッファに代入
					}
					if( lcd_str_length != 0xFF ) lcd_str_length++ ;
				}
				lcd_puts_nc();
			}
		}
	}
	timer_interrupt_reset();	// タイマーA割込フラグのリセット
//	port_ir_interrupt_reset();	// IRQ3(P17)の割り込みフラグのリセットして赤外線割り込みを許可する
}


/* IRQ0(P14)割り込み(H8システムからのコール) */
void int_irq0 (void){
	port_xb_interrupt_reset();			// IRQ0(P14)の割り込みフラグのリセット
}

/* IRQ0(P17)割り込み(H8システムからのコール) */
void int_irq3 (void){
	IENR1.BIT.IEN3 = 0;				// 割り込み禁止
	timerv_off();
	/* すぐにIR受信を始める(取りこぼし防止のため) */
	if( ir_len == 0 ){				// IR受信中のir_readの２重起動を防止
		ir_len = 1;					// 同上対策
		ir_len = (byte)(ir_read_simple( ir_data, AEHA ) / 8);
	}
}

/* メイン */
void main( void ){
	byte i;
	byte j;
//	byte lcd_refresh=0;
	
	/* 初期化、設定 */
	port_init();					// 汎用入出力ポートの初期化
	wait_millisec_250(15);			// 起動待ち
	lcd_init();						// 液晶制御ＩＣのプレ初期化
	wait_millisec_250(15);			// 初期化待ち
	lcd_init();						// ２回目の初期化（確実な初期化）
	lcd_control(1,0,0);				// 液晶制御 表示ON,カーソルOFF,点滅OFF
	lcd_cls();						// 液晶消去
	lcd_goto(LCD_ROW_1);
	lcd_putstr( NAME );
	lcd_goto(LCD_ROW_2);
	lcd_putstr( VERSION );
	
	strcopy( &strings[0][0], NAME );
	strcopy( &strings[1][0], VERSION );
	
	timer_count_init();				// 秒タイマー初期化
	beep(3);
	sci_init();						// シリアル初期化

	IR_VCC = 1;
	interrupt_enable();				// 割り込み許可

	/* 永久ループ(処理) */
	while(1){
		if( port_xb_interrupt_status() == 0 && sci_read_check() == 0 && LED_OUT == 0 && alert_value == 0 && esc_done == 0xFF){
			sleep();
			// timer_count_init();
			sci_init();
			interrupt_enable();	
		}
		
		/* エスケープコマンド実行 */
		if( esc_done == 0x00 ){
			esc_command = sci_read( 100 );			// コマンド入力
			esc_value = sci_read( 100 );			// 値入力
			if( esc_command != (byte)'I' && esc_value != (byte)'R' ){	// IR以外の時
				switch( esc_command ){
					case (byte)'L':
						led( esc_value );
						break;
					/*
					case (byte)'F':
						beep_freq( esc_value );
						break;
					case (byte)'D':
						beep_duty( esc_value );
						break;
					*/
					case (byte)'B':
						/*
						lcd_cls();
						lcd_goto(LCD_ROW_1);
						lcd_putstr("BEEP=");
						lcd_disp_hex( esc_value );
						lcd_putch('h');
						*/
						beep( esc_value );
						break;
					case (byte)'C':
						chime( esc_value );
						break;
					case (byte)'A':
						alert_value = esc_value;
						if( alert_value >= (byte)'0' && alert_value <= (byte)'9' ) alert_value -= (byte)'0';
						if( alert_value > 10 ) alert_value = 10 ;
						break;
					case (byte)'P':
						if( esc_value == 0x00 || esc_value == '0' )
							IR_VCC = 0;
						else
							IR_VCC = 1;
						lcd_cls();
						lcd_goto(LCD_ROW_1);
						lcd_putstr("IR_VCC =");
						lcd_goto(LCD_ROW_2);
						lcd_disp_1(IR_VCC);
						break;
					case (byte)'E':
						lcd_cls();
						lcd_goto(LCD_ROW_1);
						lcd_putstr("ｼｽﾃﾑｴﾗｰ ");
						lcd_goto(LCD_ROW_2);
						lcd_putstr("code=");
						lcd_disp_hex( esc_value );
						lcd_putch('h');
						alert( 0x01 );
						break;
					default:
						lcd_cls();
						lcd_goto(LCD_ROW_1);
						lcd_putstr("ESCｺｰﾄﾞ=");
						lcd_goto(LCD_ROW_2);
						lcd_putch( esc_command );
						lcd_putch('(');
						lcd_disp_hex( esc_command );
						lcd_putstr(")=");
						lcd_disp_hex( esc_value );
						break;
				}
			}else if( esc_command == (byte)'I' && esc_value == (byte)'R' ){
				//beep_ir();
				/* IR 信号コマンドの時 */
				ir_len = (byte)sci_read( 100 );			// IR長
				if( ir_len == 0xFF ){
					/* IR入力待ちへ */
					interrupt_disable();
					port_ir_interrupt_reset();
					interrupt_enable();
				}else if( ir_len < IR_DATA_SIZE ){
					/* IR信号出力 */
					for( i=0; i < ir_len ; i++ ){
						ir_data[i] = (byte)sci_read( 100 );
					}
					interrupt_disable();
					ir_send_simple( ir_data, (ir_len * 8), AEHA );
					interrupt_enable();
					ir_len = 0;				// これを省略するとxbeeからの受信データをxbeeへ送信できる					//beep_ir();
				}
			}
			esc_done = 0xFF;			// 次のエスケープコマンドの待ち受け開始
		}
		
		/* 赤外線信号受信結果のXBee送信と表示 */
		if( ir_len >= 2 ){
			sci_write( ir_data, ir_len );	// XBee送信
			lcd_cls();						// 液晶消去
			lcd_goto(LCD_ROW_1);
			for( i=0 ; i < (LCD_CHRS/2) ; i++ ) lcd_disp_hex( ir_data[i] );
			lcd_goto(LCD_ROW_2);
			for( i=0 ; i < (LCD_CHRS/2)-2 ; i++ ) lcd_disp_hex( ir_data[(LCD_CHRS/2)+i] );
			lcd_putstr(" L");
			lcd_disp_2( ir_len );
			beep_ir();
			ir_len = 0;				// 次の赤外線信号の受信を許可
		}
	}
}
