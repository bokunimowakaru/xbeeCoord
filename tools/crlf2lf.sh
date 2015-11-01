#!/bin/bash
#
# 改行コードCR+LFをLFに変換する
#
echo "改行コード CR+LF -> LF"
while [ $# -gt 0 ]
do
	echo $1
	/usr/bin/tr -d '\r' < $1 > tmp.txt
	/usr/bin/mv -f tmp.txt $1
	shift
done
