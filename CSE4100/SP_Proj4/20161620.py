import requests
from bs4 import BeautifulSoup
#import sys
#sys.setrecursionlimit(10000)

url_list = list()
same_url_flag = 0
count = 0

def write_url(results,contents,fp):
	if(results == "[]"):
		return

	global url_list
	global same_url_flag
	global count
	name = "Output_00"

	for i in results:
		j = i.get('href')
		same_url_flag = 0

		if((j[0:5] == "http:") or (len(j) == 0) or (j[:1] == '#') or (j[:1] == '?')):
			continue #태그 자체가 http로 시작할때는 더하지 않고 바로 주소,그 외 예외처리
		else:
			url = "http://cspro.sogang.ac.kr/~gr120170213/" + j

		if(len(url_list) == 0):
			url_list.append(url) #처음에 리스트에 넣어주고 무조건 출력

		for k in range(len(url_list)):
			if ((len(url_list) != 1) and (url_list[k] == url)):
				same_url_flag = 1
				break

		if(same_url_flag == 1):
			continue
		else:
			url_list.append(url) #들어왔던 url리스트를 만들어서 나중에 중복되는지 여부 확인

		r1 = requests.get(url)
		soup1 = BeautifulSoup(r1.content,"html.parser")
		results1 = soup1.find_all('a')
		contents1 = soup1.find_all('p')

		if(r1.status_code != 404):
			fp.writelines(url) #404아닐때만 아웃풋 만든다
			fp.write('\n')
			count = count + 1
			if(count<10):
				out_name = name + '0' + str(count)
			else:
				out_name = name + str(count)
			fp_out = open(out_name+'.txt','w')
			fp_out.write(soup1.get_text()) #output파일 생성하고 파일에 쓰는 부분
#			for c in contents1:
#				fp_out.write(c.get_text())
#				fp_out.write('\n')

		if((results1 == "[]") or (contents == contents1) or (r1.status_code == 404)):
			continue
		else:
			write_url(results1,contents1,fp) #재귀로 계속 들어가면서 크롤링


#여기서부터 main
r = requests.get("http://cspro.sogang.ac.kr/~gr120170213/")
soup = BeautifulSoup(r.content,"html.parser")
results = soup.find_all('a')
contents = soup.find_all('p')

fp = open('URL.txt','w')

write_url(results,contents,fp) #처음 화면 root page 읽어주고 함수 들어간다
