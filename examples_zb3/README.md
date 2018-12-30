# �{�N�ɂ��킩�� XBee3 ZigBee�p Digi MicroPython

XBee3 ZigBee�ŃT�|�[�g���ꂽ Digi MicroPython �𗘗p�����T���v���W�ł��B

## �Ή�XBee���W���[��

- XBee3 (ZigBee)

## Digi MicroPython �ݒ���@

���L�́AMicroPython���V���A���|�[�g����REPL���[�h (�_�C���N�g���[�h)�ň������߂̐ݒ���@�ł��B  

1. DIGI XCTU (�ŐV��)�́uRadio Modules�v�̃A�C�R��[�{]�iAdd a radio Module�j�Ŏg�p����V���A��COM�|�[�g��I������B  
2. �V���A���ڑ�����XBee���W���[������ʍ����̃t���[���ɕ\�������̂ŁA�����I������B  
3. �E�t���[���uRadio Configuration�v��XBee �ݒ��ʂ��\�������̂ŁA�X�N���[�����āA�uAP APIEnabe�v��[MicroPython REPL [4]]��I������B  
		[Radio Configuration]
		�� UART Interface
			AP APIEnabe
				MicroPython REPL [4]
4. �E�t���[���̏㕔�̉��M�A�C�R���iWrite�j���N���b�N����B  
5. ���j���[�uTool�v����[MicroPython Terminal]��I�����A�ڑ��A�C�R���iOpen�j���N���b�N����B�iTera Terminal���g�p����ꍇ�́A�ʐM�ݒ聁9600-8-N-1�E�t���[����Ȃ��A���s�R�[�h��CR�A���[�J���G�R�[�������Ȃǂ�ݒ肷��B�j  

## �T���v���E�X�N���v�g�̏������ݕ��@

�\�߁A�O�L�̕��@�� MicroPython REPL���[�h�ɐݒ肵��XBee3 ZigBee���W���[���ցA�T���v���E�X�N���v�g��]�����Ď��s���܂��B  

1. �T���v���X�N���v�g����������G�ۃG�f�B�^�ŊJ���A[Ctrl]+[A]�̑���őS�I�����A[Ctrl]+[C]�ŃN���b�v�{�[�h�ɃR�s�[���܂��B  
2. MicroPython Terminal �܂��� TeraTerm�ȂǂŁA[Ctrl]+[F]�𑀍삵�A�v���O������XBee3 ZigBee���W���[���֏������ރ��[�h�ɐݒ肵�܂��B  
3. [Ctrl]+[V]�𑀍삵�A�N���b�v�{�[�h�ɕێ������T���v���X�N���v�g��]�����܂��B
4. [Ctrl]+[D]�𑀍삵�A�R���p�C�������s���܂��B  
5. �uAutomatically run�v�ɑ΂���[Y]����͂���ƁA�N�����⃊�Z�b�g���쎞�ɁA�����N�������邱�Ƃ��o���܂��B  
6. [Ctrl]+[R]�Ńt���b�V�����������̃v���O���������s���܂��BAutomatically run��ݒ肵�Ă����ꍇ�́A���Z�b�g����[Ctrl]+[D]�Ŏ��s���邱�Ƃ��o���܂��B  

## Digi MicroPython REPL���[�h Keyboard shortcuts

MicroPython REPL���[�h�̑�����@���ȉ��Ɏ����܂��B

- �N����(�ʏ펞): REPL mode.  
	Python���߂���͂��邽�тɎ��s���s���A�Θb�^�̎��s���[�h�i�_�C���N�g���[�h�j�ł��B���߂��e�X�g�������Ƃ��ɗ��p���܂��B   

- [Ctrl]+[E]: Enter paste mode.  
	�N���b�v�{�[�h����X�N���v�g���y�[�X�g���A�ꎞ�I�Ƀv���O�����̓��͂Ǝ��s���s���Ƃ��Ɏg�p����u�X�N���v�g���̓��[�h�v�ł��B�v�����v�g�́u1===�v�̂悤�Ɂu�s�ԍ��v�{�u===�v�ƂȂ�܂��B���͌�A�R���p�C�����ăv���O���������s����ɂ́A[Ctrl]+[D]�𑀍삵�Ă��������B���͂���s�̎�������[Ctrl]+[C]�ł��i���͂���̓R���p�C������Ȃ��A�v���O�������s���͎��s���~����j�B�Ȃ��A�{���[�h�ł̓��쒆�́APython���߂̃X�R�[�v�������u�^�u�v��u�X�y�[�X�����v�̎����}���⎩���R���p�C���͍s���܂���B  

- [Ctrl]+[F]: Upload code to flash.   
	�N���b�v�{�[�h����X�N���v�g���y�[�X�g���A�t���b�V���������Ƀv���O�������������ނƂ��Ɏg�p���܂��B�d����؂��Ă��A�v���O������ێ����邱�Ƃ��o����_���A[Ctrl]+[E]�Ƃ̈Ⴂ�ł��B�v�����v�g�́u1^^^�v�̂悤�Ɂu�s�ԍ��v�{�u^^^�v�ƂȂ�܂��B[Ctrl]+[D]�ŏ������݂܂��B���s��[Crtl]+[R]�A���͂̎�������[Ctrl]+[C]�ł��B  

- [Ctrl]+[R]: Run code in flash.
	�t���b�V�����������̃v���O���������s���܂��B���s�̒�~��[Ctrl]+[C]�ł��B  

- [Ctrl]+[C]: Regain control of the terminal.  
	���s���̃X�N���v�g�̒��~��A�X�N���v�g���͂̒��~���s���܂��B  

- [Ctrl]+[D]: Reboot the MicroPython REPL.  
	�\�t�g�E�F�A�E���Z�b�g���s���APython�R�}���h�̏�Ԃ�ϐ��l�Ȃǂ����������܂��B

- [Ctrl]+[A]: Enter raw REPL mode.  
	�G�R�[�o�b�N��OFF�ɂ��܂��B[Ctrl]+[B]�̑���ŃG�R�[�o�b�N���[�h�ɖ߂�܂��B  

- [Ctrl]+[B]: Print the MicroPython banner.  
	�ʏ�́i�G�R�[�o�b�N���s���jREPL���[�h�ɖ߂��܂��B  

- [Ctrl]+[I]: �I�u�W�F�N�g�\��

- [Ctrl]+[M]: �s��

## XBee���W���[�� GPIO

XBee��GPIO�ėp�|�[�g�ꗗ�\  

| Port |  IO��   | Pin | DIN | AIN | �o�� | ��ȗp�r  
|:----:|:--------|:---:|:---:|:---:|:----:|:------------------------  
|    1 |DIO1(AD1)|  19 |  �� | ��  |  ��  |�ėp���͗p(DIN or AIN)  
|    2 |DIO2(AD2)|  18 |  �� | ��  |  ��  |�ėp���͗p(DIN or AIN)  
|    3 |DIO3(AD3)|  17 |  �� | ��  |  ��  |�ėp���͗p(DIN or AIN)  
|    4 |DIO4     |  11 |  �� | �~  |  ��  |�ėp�o�͗p(LED3)  
|   11 |DIO11    |   7 |  �� | �~  |  ��  |�ėp�o�͗p(LED2)�^I2C SCL  
|   12 |DIO12    |   4 |  �� | �~  |  ��  |�ėp�o�͗p(LED1)�^I2C SDA  


## �֘A�����N

- Digi XBee3 ZB�Ή� XBee���C�u���� Raspberry Pi �ł̌��J�ɂ��� 
	https://blogs.yahoo.co.jp/bokunimowakaru/56286434.html

- �{�N�ɂ��킩��XBee�p�Z���T�l�b�g���[�N�F  
	http://bokunimo.net/xbee/  
  
- �{�N�ɂ��킩��Raspberry Pi�F  
	http://bokunimo.net/raspi/  

## �Q�l����

�T���v���W�Ȃ�тɖ{�h�L�������g�̍쐬�ɂ�����A���L�̕������Q�l�ɂ��܂����B

- Digi MicroPython Programming Guide Revision L (Dec. 2018)



## ���C�Z���X�E���쌠�\��

���C�Z���X�ɂ��Ă͊e�\�[�X���X�g�Ȃ�тɊe�t�H���_���̃t�@�C���ɋL�ڂ̒ʂ�ł��D
�g�p�E�ύX�E�z�z�͉\�ł����C�����\�����c���Ă��������B
�܂��C�񋟏���z�z�\�t�g�ɂ���Đ����������Ȃ��Q�ɂ��Ă��C��؁C�⏞�������܂���B

							   Copyright (c) 2010-2018 Wataru KUNINO
							   http://bokunimo.net/

