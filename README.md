
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
## 추가할 항목 
 파일 access  -> 로그 파일 .. 로그인 .. 대화 기록 .. 링크...  

---
## 검토할 항목
확인할 항목(참고 프로그램) : 쓰레드 , 동기화 . 
아직 코드가 어떤 구조로 실행되는지 확인은 못한 상태. 