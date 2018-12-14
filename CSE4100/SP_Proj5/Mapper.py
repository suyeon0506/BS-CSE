#!/usr/bin/python3
# -*- coding: utf-8 -*-

from sys import stdin,stdout

for line in stdin:
	line = line.strip() #공백을 지운다
	words = line.split() # 쪼개기
	zip_words = list(zip(words,words[1:]))
	for pair in zip_words:
		mydict = dict()
		mydict[pair] = mydict.get(pair,0)+1
		result = pair[0] + ' ' + pair[1] + '\t' + str(mydict[pair]) + '\n'
		stdout.write(result)	
