' Sensor for IchigoJam
' CC BY Copyright(C)2015-2016 Wataru KUNINO
' http://www.geocities.jp/bokunimowakaru/q/15/
cls:new
1 'SENSOR and LED
2 UART 1
3 PRINT "Hello!"
10 'LOOP
20 I=INKEY()
30 IF I=ASC("0") LED 0
40 IF I=ASC("1") LED 1
50 PRINT BIN$(IN()),ANA(2)
60 WAIT 60
70 GOTO 10
