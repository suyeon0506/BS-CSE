*** RELEASE모드 X86에서 실행하였습니다 ***


(a) 기하 물체 배치

	1. 정적인 기하 물체 5개 추가
		- teapot2 (책상 위)
		- ironman (호랑이의 길목을 막음)
		- godzilla (집 밖에서 공격)
		- bike (중앙에 누워있음)
		- bus (오른쪽에 주차됨)

	2. 호랑이를 건물 내에서 이동
		- 호랑이의 경로는 왼쪽 위쪽에 생성, 가는 길목에 아이언맨 객체를 세워서 호랑이가 회전하여 돌아감.
		- 막다른 길목에서 호랑이는 멈춘다

	3.기본 소 배치

(b) 카메라 설계 -> 주로 키보드(keybord)로 구현
	0. 기본 추가 구현
		- '0'키 : 주 카메라(MATRIX[0],중앙의 가장 큰 화면)의 화면이 첫 기본 화면으로 되돌아간다
		- '1'키 : 주 카메라의 화면이 상면도로 넘어간다
		- '2'키 : 주 카메라의 화면이 측면도로 넘어간다

	1. 주 카메라가 카메라의 세 기본축 방향으로 이동
		- '3'키 : 주 카메라를 U축 양의 방향으로 이동
		- '4'키 : 주 카메라를 U축 음의 방향으로 이동
		- '5'키 : 주 카메라를 V축 양의 방향으로 이동
		- '6'키 : 주 카메라를 V축 음의 방향으로 이동
		- '7'키 : 주 카메라를 N축 양의 방향으로 이동
		- '8'키 : 주 카메라를 N축 음의 방향으로 이동

	2. 주 카메라가 카메라의 세 기본축 둘레로 회전 (키보드 왼쪽 맨위 6개 키)
		- 'q'키 : 주 카메라를 X축 시계 방향으로 회전
		- 'w'키 : 주 카메라를 X축 반시계 방향으로 회전
		- 'e'키 : 주 카메라를 Y축 시계 방향으로 회전
		- 'r'키 : 주 카메라를 Y축 반시계 방향으로 회전
		- 't'키 : 주 카메라를 Z축 시계 방향으로 회전
		- 'y'키 : 주 카메라를 Z축 반시계 방향으로 회전
	
	3. 주 카메라가 화면의 내용을 확대 또는 축소
		- '+'키(shift눌러야함) : 주 카메라를 확대(줌 인)
		- '-'키(shift누르지X)  : 주 카메라를 축소(줌 아웃)

	4. 정적인 CCTV
		- 주 카메라 외의 CCTV용 카메라 별도 설정(MATRIX[4])
		- 화면에서 중앙의 가장 큰 화면(주 카메라) 위쪽의 맨 오른쪽 화면임
		- 정적이므로 어떤 키를 눌러도 움직이지X
		- 소, 책상, 의자, 주전자 2개를 비추고 있다
	
	5. 동적인 CCTV
		- 주 카메라 외의 CCTV용 카메라 별도 설정(MATRIX[5])
		- 화면에서 중앙의 가장 큰 화면(주 카메라) 위쪽의 중앙 화면임
		- 'j'키 : CCTV는 고정된 채로, 카메라의 시선이 v의 음의 방향으로 전환
		- 'i'키 : CCTV는 고정된 채로, 카메라의 시선이 v의 양의 방향으로 전환

	6. 정면도, 측면도, 상면도 카메라
		- 주 카메라 외의 카메라 별도 설정 : 상면도(MATRIX[1]), 측면도(MATRIX[2]), 정면도(MATRIX[3])
		- 화면의 중앙의 가장 큰 화면 오른쪽, 즉 맨 오른쪽의 3개의 화면이 이에 해당한다.
		- 맨 오른쪽 위에서부터 정면도, 측면도, 상면도

	7. 뷰잉 변환 - 구현하지 못함