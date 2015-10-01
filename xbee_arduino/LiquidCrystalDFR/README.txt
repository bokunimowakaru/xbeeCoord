/*********************************************************************
DF ROBOT LCD Keypad Shield �p �t�����L�[�p�b�h�h���C�o

Arduino �W�����C�u������LiquidCrystal��DF ROBOT��Keypad�p�֐���
�ǉ����܂����B

�֘A	http://www.geocities.jp/bokunimowakaru/diy/arduino/lcds.html

�{�\�[�X���X�g����у\�t�g�E�F�A��GPL V2�Ɋ�Â����C�Z���X��L���܂��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2012-2013 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

/*
�g�p���@�F

���̃t�H���_���̉��L�̃t�@�C����Arduino IDE���C���X�g�[�����Ă���
libraries/LiquidCrystal�t�H���_���̂��̂ƒu�������܂��B

	�t�@�C���F
		LiquidCrystal.h
		LiquidCrystal.cpp

	�ꏊ(��)�F
		C:\arduino\libraries\LiquidCrystal

�T���v���X�P�b�`��libraries/examples/HelloWorld���̉��L�̃t�@�C����
�u�������܂��B

	�t�@�C���F
		HelloWorld.pde

	�ꏊ(��)�F
		C:\arduino\libraries\LiquidCrystal\examples\HelloWorld

*/

/***************************************
LiquidCrystal.h		60�s��
****************************************/

// for button by Wataru
#define BUTTON_UP 0x08
#define BUTTON_DOWN 0x04
#define BUTTON_LEFT 0x10
#define BUTTON_RIGHT 0x02
#define BUTTON_SELECT 0x01

/***************************************
LiquidCrystal.h		106�s��
****************************************/

  uint8_t readButtons();	// Wataru

/***************************************
LiquidCrystal.cpp	327�s�ڂ���
****************************************/

	// Wataru
	/******************************************************

	This program will test the LCD panel and the buttons
	Mark Bramwell, July 2010 

	********************************************************/

	uint8_t LiquidCrystal::readButtons(void) {
	  int adc_key_in = analogRead(0);      // read the value from the sensor
	  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741 
	  // we add approx 50 to those values and check to see if we are close 
	  if (adc_key_in > 1000) return 0x00; // We make this the 1st option for speed reasons since it will be the most likely result 
	  if (adc_key_in < 50)   return BUTTON_RIGHT;
	  if (adc_key_in < 195)  return BUTTON_UP;
	  if (adc_key_in < 380)  return BUTTON_DOWN;
	  if (adc_key_in < 555)  return BUTTON_LEFT;
	  if (adc_key_in < 790)  return BUTTON_SELECT;
	  return 0x1F;  // when all others fail, return this...
	}

