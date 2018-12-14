#!/usr/bin/python3
# -*- coding: utf-8 -*-

from sys import stdin, stdout

mydict = dict()

for line in stdin:
	pair = [0,0]
	line = line.strip()
	words,count = line.split('\t')
	pair[0],pair[1] = words.split(' ')
	pair = (pair[0],pair[1]) #튜플로 만들어준다
	
	if  pair in mydict:
		mydict[pair] += 1
	else:
		count = int(count)
		mydict[pair] = count

for pair in sorted(mydict):
	result = pair[0] + ' ' + pair[1] + '\t' + str(mydict[pair]) + '\n'
	stdout.write(result)
