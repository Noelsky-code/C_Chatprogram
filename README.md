# 서버 - 클라이언트 


## 실행방법
server : gcc - o server server.c -lpthread  
       : ./server 9090

client : gcc -o client client.c  
       : ./client 10.0.2.15 9090 hansol


---
## 참고
참고한 소켓 프로그램 : https://crazythink.github.io/2018/05/25/socketchat/  
참고한 프로그램은 소켓프로그래밍 이용한 채팅 프로그램 , 단체 대화 프로그램 같은 형식. 


---
## 필수 구현 항목
서버의 concurrent , 동기화 , 파일 access(방명록 읽기/ 작성)

---
## 추가할 항목 
 접속시 로그인 시 아이디 , 비밀번호 접속 .. 
 비밀번호 해시 암호화 .. 사용자 목록 파일 .. 
 클라이언트의 select를 이용한 concurrent 

---