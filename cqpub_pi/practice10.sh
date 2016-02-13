#!/bin/bash

#**************************************************************************************
# 第１６章	IoT機器のインターネット連携
#
#                                                      Copyright (c) 2016 Wataru KUNINO
#**************************************************************************************

hr="--------------------------------------------------------------------------------"
echo $hr
echo -n "現在の時刻 (NICT): "
curl -s ntp-a1.nict.go.jp/cgi-bin/time
echo $hr
echo -n "天気予報 (Yahoo!): "
curl -s rss.weather.yahoo.co.jp/rss/days/6200.xml| cut -d'<' -f17| cut -d'>' -f2|tail -1
echo $hr
echo "著者からのメッセージ: "
curl -s www.geocities.jp/bokunimowakaru/cq/pi.txt -o ~tmp_boku.txt
grep '<title>' ~tmp_boku.txt |cut -f2|cut -d'<' -f1
grep '<descr>' ~tmp_boku.txt |tr '<' ' '|awk '{print $2}'
grep '<info>'  ~tmp_boku.txt |cut -f2|cut -d'<' -f1
grep '<state>' ~tmp_boku.txt |tr '<' ' '|awk '{print $2}'
grep '<url>'   ~tmp_boku.txt |cut -f2|cut -d'<' -f1
grep '<date>'  ~tmp_boku.txt |tr '<' ' '|awk '{print $2}'
\rm ~tmp_boku.txt
echo $hr
