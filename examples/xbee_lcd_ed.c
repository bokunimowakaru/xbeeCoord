/*********************************************************************
�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B
�t���\���v���O�����@16�����~�P�s�^�W�����~�Q�s�p
					(���̑��ɂ�define�̕ύX�őΉ��\�ł��B)

                               Copyright (c) 2010-2012 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
Coordinator���瑗���Ă����e�L�X�g���L�����N�^�k�b�c�ɕ\�����邽�߂�
�\���f�o�C�X�iXBee End device AT�{�g�W�}�C�R���{�L�����N�^�k�b�c�j����
�v���O�����ł��B

�ԊO�����M�@�\��H8 3694 ��p�ł��B

Coordinator���� xbee.c���g�p���Axbee_putstr("hello\n");�̂悤�ɕ�����
���M���܂��B�i�{�v���O�����ɂ͊܂܂�Ă��܂���B�j

�E�e�L�X�g����M����Ɖ��s�܂ł̑����������k�b�c�֏o�͂��܂��B
�E�\��������Ȃ��ꍇ�̓X�N���[���\�����܂��B
�E�G�X�P�[�v�R�}���h(0x1B)����M����ƃR�}���h�ɉ�����LED�̓_���Ȃǂ�
�@�s���܂��B
�@�iesc_command��esc_value�ɉ����ăv���O������ǋL���ĉ������B�j
�E�o�b�e���쓮���l�����ăX���[�v���[�h�œ��삵�܂��B
�@���ȓd�͂ȃX�^���o�C���[�h�ł����삵�܂����A����ɍ��킹��LED��
�@�_�ł��Ă��܂��܂��B
�E���ӁF
�@AKI-H8/3664F�̗��ʂ�RS-232C�h���C�oIC�������̓d��������Ă��܂��B
�@AKI-H8/3664BP�ł����RS-232C�h���C�oIC���ȒP�ɊO���܂��B
�@
*/
/*********************************************************************
�n�[�h�E�F�A

	��P		H8 3664 / 3694 �ȉ��̂����ꂩ
			�EAKI-H8/3664BP(SDIP)�^�C�j�[�}�C�R���L�b�g
			�EAKI-H8/3664F(QFP) �^�C�j�[�}�C�R���L�b�g
			�EAKI-H8/3694F(QFP) �^�C�j�[�}�C�R���L�b�g

	LCD		16�����~�P�s�F�r�P�O�T�T�P�c
			16�����~�P�s�F�c�l�b�P�U�P�P�V�`
			�W�����~�Q�s�F�`�b�l�O�W�O�Q�b�|�m�k�v�|�a�a�g

			�\���A�h���X
			�P�`�W�����F0x00�`0x07
			�X�`16�����F0x40�`0x47

	XBee	XBee ZigBee (Series 2)
			Firmware = END DEVICE AT

					AKI-H8�@�@�@�@�@�@�t�����W���[��
					(CN1)�@�@�@�@�@�@ (HD44780����)
					�������@�@�@�@�@�@���������������@�@�@�@�� 5V
					�@�@��P50�@�@�@�@ ���@�@�@�@�@���@�@�@�@�b
					�@14����������������DB4�@�@Vdd������������
					�@�@��P51�@�@�@�@ ���@�@�@�@�@���@�@�@�b
					�@15����������������DB5 �@�@�@���@�@�@��
					�@�@��P52�@�@�@�@ ���@�@�@�@�@���@������
					�@16����������������DB6�@�@VLC�������@�� 10k
					�@�@��P53�@�@�@�@ ���@�@�@�@�@���@�@�@�b
					�@17����������������DB7�@DB0-3�������@�b
					�@�@��P54�@�@�@�@ ���@�@�@�@�@���@�b�@�b
					�@18����������������E�@�@�@R/W�������@�b
					�@�@��P55�@�@�@�@ ���@�@�@�@�@���@�b�@�b
					�@19����������������RS�@�@ Vss������������
					�@�@���@�@�@�@�@�@���������������@�@�@������
					�@�@
					(CN2)�@�@�@�@�@�@�@XBee
					�������@�@�@�@�@�@���������������@�@�� 3.3V
					�@�@��RXD�@�@ DOUT���@�@�@�@�@��Vdd �b
					�@18����������������2�@�@�@�@1��������
					�@�@��TXD ��R DIN���@�@�@�@�@��
					�@19����������������3 �@�@�@�@��
					�@�@��IRQ0 ���� ON���@�@�@�@�@��GND
					�@20����������������13�@�@�@10��������
					�@�@���@�@�@�@�@�@���@�@�@�@�@���@������
					�@�@��
					�@�@���@�@�@�@�@�@�@�@Diode
					�@�@���@�@�@�@�@������������������
					�@�@���@�@�@�@�@�b�@�@�@�@�@�@�@�b
					�@�@���@�@�@�@�@�b ���d�X�s�[�J �b
					�@�@��TMOV�@�@�@�b���������������b
					�@ 7�����������������@�b���b�@��������
					�@�@���@�@22uF�@�@���������������@������

					��R���� Series 10K �{ Shunt 15k
					
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
					P87(CN2 16)	LED(�x���p)
					P17(CN2 23)	IR-IN


*********************************************************************/

/*
�ʐM�d�l(XBee UART)

	���x�����[�^�\��

		[ESC] [L] [0�`9,A]			���x�����[�^�̕\�����x��(0�`10�i)
		[ESC] [L] [0x00�`0x0A]		���x�����[�^�̕\�����x��(0�`10�i)
		[ESC] [L] [0X0E,'E']		�x�����̕\������
		[ESC] [L] [0X0F,'F']		�x�����̕\��

	�u�U�[�o��

		[ESC] [A] [0�`9] 			�A���[�g����炷(0�`9��),0��OFF
		[ESC] [B] [0�`9] 			�x������炷(0�`10��),0��OFF
		[ESC] [C] [0�`9]			�`���C����(1��)
		
	�ԊO�������R��
	
		[ESC][I][R][����][�f�[�^]	�����A�f�[�^�̓o�C�i��
		[ESC][I][R][0xFF]			�ԊO���̎�����[�h��

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

#define LCD_CHRS	8				// �t���̕�����(16�����~1�s�ł�8����)
#define LCD_ROWS	2				// �t���̍s��(16�����~1�s�ł�2�s)

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
#define	LED_OUT_PRT	IO.PDR8.BIT.B7	// P87(CN2 16)	LED(�x���p)
#define	IR_OUT		IO.PDR8.BIT.B5	// P85(CN2 14)	IR-LED
#define	IR_VCC		IO.PDR8.BIT.B6	// P86(CN2 15)	IR-VCC
#define	IR_IN		IO.PDR1.BIT.B7	// P17(CN2 23)	IR-IN
#define	IR_IN_OFF	1				// �ԊO���Z���T�������̓��͒l
#define	IR_IN_ON	0				// �ԊO���Z���T������̓��͒l
#define	IR_OUT_OFF	0				// �ԊO���Z���T�񔭌����̓��͒l
#define	IR_OUT_ON	1				// �ԊO���Z���T�������̓��͒l
#define	IR_SYNC_WAIT	4095			// �������̂Q�{�ȏ� �ő�32235
#define	IR_DATA_SIZE	16				// �f�[�^��(byte),4�̔{��

/* H3664 16MHz�p
#define FLASH_AEHA_WAIT		5		// 13us(38kHz)���̋󃋁[�v�����l
#define FLASH_AEHA_TIMES	18		// �V���{����Ԃ̓_�ŉ�
#define FLASH_NEC_WAIT		5		// 13us(38kHz)���̋󃋁[�v�����l
#define FLASH_NEC_TIMES		22		// �V���{����Ԃ̓_�ŉ�
#define FLASH_SIRC_WAIT		5		// 12.5us(40kHz)���̋󃋁[�v�����l
#define FLASH_SIRC_TIMES	24		// �V���{����Ԃ̓_�ŉ�
*/
#define FLASH_AEHA_WAIT_H	7		// 13us(38kHz)���̋󃋁[�v�����l
#define FLASH_AEHA_WAIT_L	7		// 13us(38kHz)���̋󃋁[�v�����l
									// 6=(41kHz) 6.5=(39kHz) 7=(37kHz)
#define FLASH_AEHA_TIMES	16		// �V���{����Ԃ̓_�ŉ�
									// 17=(5263us) 16=(5023us)
/*
�Ɩ��p�����R��(Panasonic)
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
�e���r�p�����R��(Sharp)
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
#define	SCI_SIZE	256				// �V���A���f�[�^��(�傫�������RAM���s������)

typedef unsigned char byte;
byte	LED_OUT = 0 ;				// �x���pLED
byte	TIMER_COUNT	=	0	;		//�q�s�b�J�E���g�p250ms�P��
char	strings[LCD_ROWS+1][LCD_CHRS+1]	;
byte	lcd_str_length	=	0	;

byte	ir_len = 0x00;
byte	ir_data[IR_DATA_SIZE];
enum Ir_Mode {NA,AEHA,NEC,SIRC,ERROR};	// �ԊO�����[�h�̌^

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

/* ���荞�݋��� */
void interrupt_enable(void){
	EI;
}

/* ���荞�݋��� */
void interrupt_disable(void){
	DI;
}

void timer_interrupt_reset(void){
	IRR1.BIT.IRRTA=0;			// �^�C�}�[A�����t���O�̃��Z�b�g
}

byte port_xb_interrupt_status(void){
	return( (byte)IO.PDR1.BIT.B4 );
}

byte port_ir_interrupt_status(void){
	return( (byte)IO.PDR1.BIT.B7 );
}

void port_xb_interrupt_reset(void){
	IEGR1.BIT.IEG0 = 1;				// IRQ0�[�q(P14)�̃G�b�W�����o�i�����オ��=1)
	IRR1.BIT.IRRI0 = 0;				// IRQ0�[�q(P14)�̊����݃t���O�̃��Z�b�g 
	IENR1.BIT.IEN0 = 1;				// IRQ0�[�q(P14)�̊����݂𗘗p�\�ɂ��� 
}

void port_ir_interrupt_reset(void){
	while( port_ir_interrupt_status() != 1 );
	IEGR1.BIT.IEG3 = 0;				// IRQ3�[�q(P17)�̃G�b�W�����o�i����������=0)
	IRR1.BIT.IRRI3 = 0;				// IRQ3�[�q(P17)�̊����݃t���O�̃��Z�b�g 
	IENR1.BIT.IEN3 = 1;				// IRQ3�[�q(P17)�̊����݂𗘗p�\�ɂ��� 
}

void port_init(void){
	/*�|�[�g1         76543210*/
	IO.PMR1.BYTE  = 0b10010000;     // ���[�h(P14)     ���o��=0  IRQ=1
	IO.PCR1       = 0b01100111;     // ���o�͐ݒ�       ����  =0  �o��  =1  B3=���U�[�u
	IO.PUCR1.BYTE = 0b10010000;     // �v���A�b�v(P14) ���Ȃ�=0  ����  =1
	IO.PDR1.BYTE  = 0b00000000;     // �A�N�Z�X         �k�o��=0  �g�o��=1

	DI;
	port_xb_interrupt_reset();
	port_ir_interrupt_reset();
	IENR1.BIT.IEN3 = 0;				// IR�̊��荞�݋֎~

	/*�|�[�g8         76543210*/	// �H�� H8 Tiny I/O BOARD TERA2 [K-00207]�p
	IO.PCR8       = 0b01111111;     // ���o�͐ݒ�       ����  =0  �o��  =1
	IO.PDR8.BYTE  = 0b00000000;     // �A�N�Z�X         �k�o��=0  �g�o��=1
	
	/*�|�[�g8         76543210*/	// �H�� H8 Tiny I/O BOARD TERA2 [K-00207]�p
	IO.PCR8       = 0b11111111;     // ���o�͐ݒ�       ����  =0  �o��  =1
	IO.PDR8.BYTE  = 0b00000000;     // �A�N�Z�X         �k�o��=0  �g�o��=1
}

/* �^�C�}�[������ */
void timer_count_init(void){
	DI;

	/* �^�C�}�[A 8bit �����\���̑���^�C�~���O�p */
	TA.TMA.BYTE     =0b00001010;  /* ���v�p�N���b�N  */
	/*                 |  |||-|__ ������ 000�`111(������) 000�`011(�ᑦ��)
	         �Œ�l ___|--||             000:0.5120 sec.  000 1000 msec.
	                       |             001:0.2560 sec.  001  500 msec.
	 ����=0, �ᑬ=1 _______|             100:0.0160 sec.  010  250 msec.
	                                     111:0.0005 sec.  011   31.25 msec.
	*/

	IRR1.BIT.IRRTA  =0;           /* �^�C�}�[�`�����݃t���O�̃��Z�b�g */
	IENR1.BIT.IENTA =1;           /* �^�C�}�[�`�����݂𗘗p�\�ɂ��� */
	
	/* �X�^���o�C���[�h(�S�ăI�t)
	SYSCR1.BIT.SSBY =1;			// �X�^���o�C=1�^�X���[�v=0
	SYSCR2.BIT.DTON =0;			// �_�C���N�g�g�����X�t�@ON�t���O
	*/
	
	/* �T�u�X���[�v���[�h */
	SYSCR1.BIT.SSBY =0;			// �X�^���o�C=1�^�X���[�v=0
	SYSCR2.BIT.DTON =0;			// �_�C���N�g�g�����X�t�@ON�t���O
	SYSCR2.BIT.SMSEL =1;		// �X���[�v���[�h
	SYSCR2.BIT.LSON =0;			// ���[�X�s�[�hON�t���O
}

byte timera(void){
	return( (byte)TA.TCA );
}

/* �^�C�}�[V 8bit �u�U�[�p */
void timerv(void){
	TV.TCRV0.BYTE   =0b00010011;
	/*                 ||||||||__ ������ �ŉ��ʂ�TCSV1�B�ƂƂ��ɐݒ肷��
	                   ||||||____ �^�C�}�[�p�̃N���b�N = 0 �����N���b�N
	                   |||||_____ CCLR �J�E���^�N���A = 10 (�R���y�A�}�b�`B)
	                   |||_______ ORIE �I�[�o�t���[IE = 0
	                   ||________ CMIE �R���y�A�}�b�`IE(B,A) = 00
	*/
	TV.TCSRV.BYTE   =0b00001001;
	/*                 ||| |__|__ OS 1001 �}�b�`A��Low  �}�b�`B��Hign __/~~
	                   |||           0110 �}�b�`A��Hign �}�b�`B��Low  ~~\__
	                   |||_______ OVF �I�[�o�t���[�t���O
	                   ||________ CMF �R���y�A�}�b�`�t���O(B,A) 
	*/
	TV.TCRV1.BYTE   =0b11100010; // TRGV�[�q���͐ݒ� (�f�[�^�V�[�g P11-6)
	/*                    ||| |__ ��������̍ŉ���
	                      |||       010:1/4   5 MHz   (4MHz)
	                      |||       011:1/8   2.5 M   (2MHz)
	                      |||       100:1/16  1.25M   (1MHz)
	                      |||       101:1/32   625k   (500k)
	                      |||       110:1/64  312.5k  (250k) 1/100��3125Hz
	                      |||       111:1/128 156.25k (125k) 1/100��1563Hz
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

/* �~���b�҂�250ms ���͔͈́�4�`250 ms */
void wait_millisec_250( byte ms ){
	byte counter;
	if( ms < 0x04 ) ms = 0x04;
	counter = timera() + (byte)( ms>>2 );
	if( counter == timera() ) counter++;
	while( counter != timera() );
}

/* �~���b�҂� ���͔͈́�4�`30,000 ms */
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

/* LED �_�� */
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

/* beep �� */
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

/* alert �� 0.5�b */
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

/* chime �� 2�b�� */
/*
	   ������ 1/64
	    1108.731�~2Hz 1/141
	     880.000�~2Hz 1/178
	   
	    156.25k (125k) 1/100��1563Hz
*/
#define CHIME_T 6
void chime(byte value){
	byte i,j;
	byte time=0;
	
	if( value >= (byte)'0' && value <= (byte)'9' ) value -= (byte)'0';
	if( value > 10 ) value = 10 ;
	if( value != 0x00 ){
		/* �P����(�m�C�Y�����)
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
		/* �a���Łi�m�C�Y�����܂�����j */
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


/* �V���A���̏����� */
char sci_tx[SCI_SIZE], sci_rx[SCI_SIZE];	// PC�͎�M�o�b�t�@���g�p���Ȃ�
byte sci_init( void ){
	SCI3_INIT(br9600, sci_tx, SCI_SIZE, sci_tx, SCI_SIZE);
	// EI;									// ���荞�݋���
	return(1);
}

/* �V���A������M�o�b�t�@�N���A */
void sci_clear(void){
	SCI3_IN_DATA_CLEAR();
	SCI3_OUT_DATA_CLEAR();
}

byte sci_read_value;

/* �V���A����M */
byte sci_read(byte timeout){
	byte timer;
	sci_read_value = 0;
	timer = timera() + (timeout)+1;		// timeout[ms] = timer/256*1000
	while( timer != timera() && SCI3_IN_DATA_CHECK() < 1 );
	if( SCI3_IN_DATA_CHECK() ) sci_read_value=(byte)SCI3_IN_DATA();
	return( sci_read_value );
}

/* �V���A����M�o�b�t�@�̊m�F */
byte sci_read_check(void){
	return( sci_read_value );
}

/* �V���A�����M�o�b�t�@����ɂȂ�̂�҂� */
byte sci_write_check(void){
	byte timer;
	timer = timera() + 0x7F;	// timeout = 500ms
	while( (timer != timera()) && ( SCI3_OUT_DATA_CHECK() < 1 ) );
	return( (byte)SCI3_OUT_DATA_CHECK() );
}

/* �V���A�����M */
void sci_write( char *data, byte len ){
	SCI3_OUT_STRINGB( data , (short)len );
}

/* string/byte���� */
void strcopy(char *s1, const char *s2){	// string.h���܂܂��ꍇ��strcpy���g�p
    while( *s2 != 0x00 ) *s1++ = *s2++;
    *s1 = 0x00;
}
void bytecpy(byte *s1, const byte *s2, byte size){
	byte i;
	for(i=0; i< size ;i++ ){
		s1[i] = s2[i];
	}
}
byte bytecmp(byte *s1, const byte *s2, byte size){	// strcmp�̂悤�ȑ召��r�͖���
	byte i=0;										// ����Ȃ�0������
	for(i=0; i< size ;i++ ){
		if( s1[i] != s2[i] ) return(1);
	}
	return(0);
}

/*********************************************************************
�\��

*********************************************************************/

/*	���荞�ݏ���	0.25�b���ƂɌĂяo�����	*/
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

/* �^�C�}�[���荞��(H8�V�X�e������̃R�[��) */
void int_timera(void){
	byte i,j;
	byte c;
	
	if( esc_done ){				// �G�X�P�[�v�R�}���h���s��(0x00)�̏ꍇ�͈ȉ������s�����ɏ����𔲂���
		TIMER_COUNT++;				// �^�C�}�[�̃J�E���g�A�b�v
		if( TIMER_COUNT >= 240 ){	// 1����1��̏���
			TIMER_COUNT = 0;
		}
		if( LED_OUT || alert_value ) LED_OUT_PRT = (TIMER_COUNT & 0x01 );
		
		if( alert_value && ( TIMER_COUNT % 6) == 0) {
			lcd_cls();
			lcd_goto(LCD_ROW_1);
			lcd_putstr("#### ��");
			lcd_goto(LCD_ROW_2);
			lcd_putstr("γ ####");
			alert( 0x01 );
		}else{	
			c = sci_read( 2 );					// sci����read����B�l��unsigned char
			if( c == 0x1B){			// �G�X�P�[�v(27)
				esc_done=0x00;
			}else if( c ){
				if( c == 0x0A || c == 0x0D ){	// ���s�̎�
					lcd_str_length = 0x00;
				}else{
					if( lcd_str_length >= (LCD_ROWS * LCD_CHRS) ){
						str_shift();
						strings[LCD_ROWS-1][LCD_CHRS-1] = str_putch_nc( c );	// �����ɕϊ����ăo�b�t�@�ɑ��
					}else{
						if( lcd_str_length == 0){
							for( i=0 ; i < LCD_ROWS ; i++){
								for( j=0 ; j < LCD_CHRS ; j++){
									strings[i][j] = ' ';
								}
							}
						}
						i = ( lcd_str_length / LCD_CHRS );
						strings[i][ lcd_str_length - i * LCD_CHRS ] = str_putch_nc( c );	// �����ɕϊ����ăo�b�t�@�ɑ��
					}
					if( lcd_str_length != 0xFF ) lcd_str_length++ ;
				}
				lcd_puts_nc();
			}
		}
	}
	timer_interrupt_reset();	// �^�C�}�[A�����t���O�̃��Z�b�g
//	port_ir_interrupt_reset();	// IRQ3(P17)�̊��荞�݃t���O�̃��Z�b�g���ĐԊO�����荞�݂�������
}


/* IRQ0(P14)���荞��(H8�V�X�e������̃R�[��) */
void int_irq0 (void){
	port_xb_interrupt_reset();			// IRQ0(P14)�̊��荞�݃t���O�̃��Z�b�g
}

/* IRQ0(P17)���荞��(H8�V�X�e������̃R�[��) */
void int_irq3 (void){
	IENR1.BIT.IEN3 = 0;				// ���荞�݋֎~
	timerv_off();
	/* ������IR��M���n�߂�(��肱�ڂ��h�~�̂���) */
	if( ir_len == 0 ){				// IR��M����ir_read�̂Q�d�N����h�~
		ir_len = 1;					// ����΍�
		ir_len = (byte)(ir_read_simple( ir_data, AEHA ) / 8);
	}
}

/* ���C�� */
void main( void ){
	byte i;
	byte j;
//	byte lcd_refresh=0;
	
	/* �������A�ݒ� */
	port_init();					// �ėp���o�̓|�[�g�̏�����
	wait_millisec_250(15);			// �N���҂�
	lcd_init();						// �t������h�b�̃v��������
	wait_millisec_250(15);			// �������҂�
	lcd_init();						// �Q��ڂ̏������i�m���ȏ������j
	lcd_control(1,0,0);				// �t������ �\��ON,�J�[�\��OFF,�_��OFF
	lcd_cls();						// �t������
	lcd_goto(LCD_ROW_1);
	lcd_putstr( NAME );
	lcd_goto(LCD_ROW_2);
	lcd_putstr( VERSION );
	
	strcopy( &strings[0][0], NAME );
	strcopy( &strings[1][0], VERSION );
	
	timer_count_init();				// �b�^�C�}�[������
	beep(3);
	sci_init();						// �V���A��������

	IR_VCC = 1;
	interrupt_enable();				// ���荞�݋���

	/* �i�v���[�v(����) */
	while(1){
		if( port_xb_interrupt_status() == 0 && sci_read_check() == 0 && LED_OUT == 0 && alert_value == 0 && esc_done == 0xFF){
			sleep();
			// timer_count_init();
			sci_init();
			interrupt_enable();	
		}
		
		/* �G�X�P�[�v�R�}���h���s */
		if( esc_done == 0x00 ){
			esc_command = sci_read( 100 );			// �R�}���h����
			esc_value = sci_read( 100 );			// �l����
			if( esc_command != (byte)'I' && esc_value != (byte)'R' ){	// IR�ȊO�̎�
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
						lcd_putstr("���Ѵװ ");
						lcd_goto(LCD_ROW_2);
						lcd_putstr("code=");
						lcd_disp_hex( esc_value );
						lcd_putch('h');
						alert( 0x01 );
						break;
					default:
						lcd_cls();
						lcd_goto(LCD_ROW_1);
						lcd_putstr("ESC����=");
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
				/* IR �M���R�}���h�̎� */
				ir_len = (byte)sci_read( 100 );			// IR��
				if( ir_len == 0xFF ){
					/* IR���͑҂��� */
					interrupt_disable();
					port_ir_interrupt_reset();
					interrupt_enable();
				}else if( ir_len < IR_DATA_SIZE ){
					/* IR�M���o�� */
					for( i=0; i < ir_len ; i++ ){
						ir_data[i] = (byte)sci_read( 100 );
					}
					interrupt_disable();
					ir_send_simple( ir_data, (ir_len * 8), AEHA );
					interrupt_enable();
					ir_len = 0;				// ������ȗ������xbee����̎�M�f�[�^��xbee�֑��M�ł���					//beep_ir();
				}
			}
			esc_done = 0xFF;			// ���̃G�X�P�[�v�R�}���h�̑҂��󂯊J�n
		}
		
		/* �ԊO���M����M���ʂ�XBee���M�ƕ\�� */
		if( ir_len >= 2 ){
			sci_write( ir_data, ir_len );	// XBee���M
			lcd_cls();						// �t������
			lcd_goto(LCD_ROW_1);
			for( i=0 ; i < (LCD_CHRS/2) ; i++ ) lcd_disp_hex( ir_data[i] );
			lcd_goto(LCD_ROW_2);
			for( i=0 ; i < (LCD_CHRS/2)-2 ; i++ ) lcd_disp_hex( ir_data[(LCD_CHRS/2)+i] );
			lcd_putstr(" L");
			lcd_disp_2( ir_len );
			beep_ir();
			ir_len = 0;				// ���̐ԊO���M���̎�M������
		}
	}
}
