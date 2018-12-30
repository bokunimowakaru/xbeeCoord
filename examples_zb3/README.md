# ボクにもわかる XBee3 ZigBee用 Digi MicroPython

XBee3 ZigBeeでサポートされた Digi MicroPython を利用したサンプル集です。

## 対応XBeeモジュール

- XBee3 (ZigBee)

## Digi MicroPython 設定方法

下記は、MicroPythonをシリアルポートからREPLモード (ダイレクトモード)で扱うための設定方法です。  

1. DIGI XCTU (最新版)の「Radio Modules」のアイコン[＋]（Add a radio Module）で使用するシリアルCOMポートを選択する。  
2. シリアル接続したXBeeモジュールが画面左側のフレームに表示されるので、それを選択する。  
3. 右フレーム「Radio Configuration」にXBee 設定画面が表示されるので、スクロールして、「AP APIEnabe」で[MicroPython REPL [4]]を選択する。  
		[Radio Configuration]
		▼ UART Interface
			AP APIEnabe
				MicroPython REPL [4]
4. 右フレームの上部の鉛筆アイコン（Write）をクリックする。  
5. メニュー「Tool」から[MicroPython Terminal]を選択し、接続アイコン（Open）をクリックする。（Tera Terminalを使用する場合は、通信設定＝9600-8-N-1・フロー制御なし、改行コード＝CR、ローカルエコー＝無しなどを設定する。）  

## サンプル・スクリプトの書き込み方法

予め、前記の方法で MicroPython REPLモードに設定したXBee3 ZigBeeモジュールへ、サンプル・スクリプトを転送して実行します。  

1. サンプルスクリプトをメモ帳や秀丸エディタで開き、[Ctrl]+[A]の操作で全選択し、[Ctrl]+[C]でクリップボードにコピーします。  
2. MicroPython Terminal または TeraTermなどで、[Ctrl]+[F]を操作し、プログラムをXBee3 ZigBeeモジュールへ書き込むモードに設定します。  
3. [Ctrl]+[V]を操作し、クリップボードに保持したサンプルスクリプトを転送します。
4. [Ctrl]+[D]を操作し、コンパイルを実行します。  
5. 「Automatically run」に対して[Y]を入力すると、起動時やリセット操作時に、自動起動させることが出来ます。  
6. [Ctrl]+[R]でフラッシュメモリ内のプログラムを実行します。Automatically runを設定していた場合は、リセット操作[Ctrl]+[D]で実行することも出来ます。  

## Digi MicroPython REPLモード Keyboard shortcuts

MicroPython REPLモードの操作方法を以下に示します。

- 起動時(通常時): REPL mode.  
	Python命令を入力するたびに実行を行う、対話型の実行モード（ダイレクトモード）です。命令をテストしたいときに利用します。   

- [Ctrl]+[E]: Enter paste mode.  
	クリップボードからスクリプトをペーストし、一時的にプログラムの入力と実行を行うときに使用する「スクリプト入力モード」です。プロンプトは「1===」のように「行番号」＋「===」となります。入力後、コンパイルしてプログラムを実行するには、[Ctrl]+[D]を操作してください。入力や実行の取り消しは[Ctrl]+[C]です（入力ちゅはコンパイルされない、プログラム実行中は実行を停止する）。なお、本モードでの動作中は、Python命令のスコープを示す「タブ」や「スペース文字」の自動挿入や自動コンパイルは行われません。  

- [Ctrl]+[F]: Upload code to flash.   
	クリップボードからスクリプトをペーストし、フラッシュメモリにプログラムを書き込むときに使用します。電源を切っても、プログラムを保持することが出来る点が、[Ctrl]+[E]との違いです。プロンプトは「1^^^」のように「行番号」＋「^^^」となります。[Ctrl]+[D]で書き込みます。実行は[Crtl]+[R]、入力の取り消しは[Ctrl]+[C]です。  

- [Ctrl]+[R]: Run code in flash.
	フラッシュメモリ内のプログラムを実行します。実行の停止は[Ctrl]+[C]です。  

- [Ctrl]+[C]: Regain control of the terminal.  
	実行中のスクリプトの中止や、スクリプト入力の中止を行います。  

- [Ctrl]+[D]: Reboot the MicroPython REPL.  
	ソフトウェア・リセットを行い、Pythonコマンドの状態や変数値などを初期化します。

- [Ctrl]+[A]: Enter raw REPL mode.  
	エコーバックをOFFにします。[Ctrl]+[B]の操作でエコーバックモードに戻ります。  

- [Ctrl]+[B]: Print the MicroPython banner.  
	通常の（エコーバックを行う）REPLモードに戻します。  

- [Ctrl]+[I]: オブジェクト表示

- [Ctrl]+[M]: 不明

## XBeeモジュール GPIO

XBeeのGPIO汎用ポート一覧表  

| Port |  IO名   | Pin | DIN | AIN | 出力 | 主な用途  
|:----:|:--------|:---:|:---:|:---:|:----:|:------------------------  
|    1 |DIO1(AD1)|  19 |  ○ | ○  |  ○  |汎用入力用(DIN or AIN)  
|    2 |DIO2(AD2)|  18 |  ○ | ○  |  ○  |汎用入力用(DIN or AIN)  
|    3 |DIO3(AD3)|  17 |  ○ | ○  |  ○  |汎用入力用(DIN or AIN)  
|    4 |DIO4     |  11 |  ○ | ×  |  ◎  |汎用出力用(LED3)  
|   11 |DIO11    |   7 |  ○ | ×  |  ◎  |汎用出力用(LED2)／I2C SCL  
|   12 |DIO12    |   4 |  ○ | ×  |  ○  |汎用出力用(LED1)／I2C SDA  


## 関連リンク

- Digi XBee3 ZB対応 XBeeライブラリ Raspberry Pi 版の公開について 
	https://blogs.yahoo.co.jp/bokunimowakaru/56286434.html

- ボクにもわかるXBee用センサネットワーク：  
	http://bokunimo.net/xbee/  
  
- ボクにもわかるRaspberry Pi：  
	http://bokunimo.net/raspi/  

## 参考文献

サンプル集ならびに本ドキュメントの作成にあたり、下記の文献を参考にしました。

- Digi MicroPython Programming Guide Revision L (Dec. 2018)



## ライセンス・著作権表示

ライセンスについては各ソースリストならびに各フォルダ内のファイルに記載の通りです．
使用・変更・配布は可能ですが，権利表示を残してください。
また，提供情報や配布ソフトによって生じたいかなる被害についても，一切，補償いたしません。

							   Copyright (c) 2010-2018 Wataru KUNINO
							   http://bokunimo.net/

