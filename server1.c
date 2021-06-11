#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MAXLINE  511
#define MAX_SOCK 1024 // 솔라리스의 경우 64

char *EXIT_STRING = "exit";	// 클라이언트의 종료요청 문자열
char *START_STRING = "Connected to chat_server \n";
// 클라이언트 환영 메시지
int maxfdp1;				// 최대 소켓번호 +1
int num_user = 0;			// 채팅 참가자 수
int num_chat = 0;			// 지금까지 오간 대화의 수
int clisock_list[MAX_SOCK];		// 채팅에 참가자 소켓번호 목록
char ip_list[MAX_SOCK][20];		//접속한 ip목록
int listen_sock;			// 서버의 리슨 소켓


							// 새로운 채팅 참가자 처리
void addClient(int s, struct sockaddr_in *newcliaddr);
int getmax();				// 최대 소켓 번호 찾기
void removeClient(int s);	// 채팅 탈퇴 처리 함수
int tcp_listen(int host, int port, int backlog); // 소켓 생성 및 listen
void errquit(char *mesg) { perror(mesg); exit(1); }
void log_msg(char* mslg); // 로그파일 메시지 보내기 

time_t ct;
struct tm tm;

void *server_thread_function(void *arg) { //명령어를 처리할 스레드
	int i;
	printf("명령어 목록 : help, num_user, num_chat, ip_list\n");
	while (1) {
		char bufmsg[MAXLINE + 1];
		fprintf(stderr, "\033[1;32m"); //글자색을 녹색으로 변경
		printf("server>"); //커서 출력
		fgets(bufmsg, MAXLINE, stdin); //명령어 입력
		if (!strcmp(bufmsg, "\n")) continue;   //엔터 무시
		else if (!strcmp(bufmsg, "help\n"))    //명령어 처리
			printf("help, num_user, num_chat, ip_list\n");
		else if (!strcmp(bufmsg, "num_user\n"))//명령어 처리
			printf("현재 참가자 수 = %d\n", num_user);
		else if (!strcmp(bufmsg, "num_chat\n"))//명령어 처리
			printf("지금까지 오간 대화의 수 = %d\n", num_chat);
		else if (!strcmp(bufmsg, "ip_list\n")) //명령어 처리
			for (i = 0; i < num_user; i++)
				printf("%s\n", ip_list[i]);
		else //예외 처리
			printf("해당 명령어가 없습니다.help를 참조하세요.\n");
	}
}
void *msg_thread_function(char* buf,int nbyte){ // 메시지를 클라이언트 들에게 전송해주는 쓰레드
	for(int i=0;i<num_user;i++){
		send(clisock_list[i],buf,nbyte,0);
	}

}
void *child_thread_function(int accp_sock){
	char buf[MAXLINE + 1]; //클라이언트에서 받은 메시지
	fd_set read_fds;
	int nbyte=0;
	send(accp_sock,START_STRING,strlen(START_STRING),0);
	

	while(1){
	      FD_ZERO(&read_fds);
	      FD_SET(accp_sock,&read_fds);
	      	if (select(accp_sock+1, &read_fds, NULL, NULL, NULL) < 0)
			errquit("select fail");
	      if(FD_ISSET(accp_sock,&read_fds)){
		     num_chat++;
		     nbyte=recv(accp_sock,buf,MAXLINE,0);
		     msg_thread_function(buf,nbyte);
	      }
	}
}


int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr;
	
	int i, j, nbyte, accp_sock, addrlen = sizeof(struct
		sockaddr_in);
	pthread_t a_thread;
	pthread_t tid[60];
	fd_set listen_fds;

	if (argc != 2) {
		printf("사용법 :%s port\n", argv[0]);
		exit(0);
	}

	

	//server 명령어 처리 스레드 생성
	pthread_create(&a_thread, NULL, server_thread_function, (void *)NULL);
	// tcp_listen(host, port, backlog) 함수 호출
	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);
	
	while (1) {
		
		accp_sock = accept(listen_sock,
			(struct sockaddr*)&cliaddr, &addrlen);	
		if(accp_sock<0){
			printf("accept failed\n");
		}
		addClient(accp_sock, &cliaddr);
		pthread_create(&tid[num_user++],NULL,child_thread_function(accp_sock),(void *)NULL);//
		
		// 클라이언트가 보낸 메시지를 모든 클라이언트에게 방송
	}  // end of while

	return 0;
}

// 새로운 채팅 참가자 처리
void addClient(int s, struct sockaddr_in *newcliaddr) {
	char buf[20];
	inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf));
	write(1, "\033[0G", 4);		//커서의 X좌표를 0으로 이동
	fprintf(stderr, "\033[33m");	//글자색을 노란색으로 변경
	printf("new client: %s\n", buf);//ip출력
	// 채팅 클라이언트 목록에 추가
	clisock_list[num_user] = s;
	strcpy(ip_list[num_user], buf);
	num_user++; //유저 수 증가
	ct = time(NULL);			//현재 시간을 받아옴
	tm = *localtime(&ct);
	write(1, "\033[0G", 4);		//커서의 X좌표를 0으로 이동
	printf("[%02d:%02d:%02d]", tm.tm_hour, tm.tm_min, tm.tm_sec);
	fprintf(stderr, "\033[33m");//글자색을 노란색으로 변경
	printf("사용자 1명 추가. 현재 참가자 수 = %d\n", num_user);
	fprintf(stderr, "\033[32m");//글자색을 녹색으로 변경
	fprintf(stderr, "server>"); //커서 출력
}

// 채팅 탈퇴 처리
void removeClient(int s) {
	close(clisock_list[s]);
	if (s != num_user - 1) { //저장된 리스트 재배열
		clisock_list[s] = clisock_list[num_user - 1];
		strcpy(ip_list[s], ip_list[num_user - 1]);
	}
	num_user--; //유저 수 감소
	ct = time(NULL);			//현재 시간을 받아옴
	tm = *localtime(&ct);
	write(1, "\033[0G", 4);		//커서의 X좌표를 0으로 이동
	fprintf(stderr, "\033[33m");//글자색을 노란색으로 변경
	printf("[%02d:%02d:%02d]", tm.tm_hour, tm.tm_min, tm.tm_sec);
	printf("채팅 참가자 1명 탈퇴. 현재 참가자 수 = %d\n", num_user);
	fprintf(stderr, "\033[32m");//글자색을 녹색으로 변경
	fprintf(stderr, "server>"); //커서 출력
}

// 최대 소켓번호 찾기
int getmax() {
	// Minimum 소켓번호는 가정 먼저 생성된 listen_sock
	int max = listen_sock;
	int i;
	for (i = 0; i < num_user; i++)
		if (clisock_list[i] > max)
			max = clisock_list[i];
	return max;
}

// listen 소켓 생성 및 listen
int  tcp_listen(int host, int port, int backlog) {
	int sd;
	struct sockaddr_in servaddr;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("socket fail");
		exit(1);
	}
	// servaddr 구조체의 내용 세팅
	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(host);
	servaddr.sin_port = htons(port);
	if (bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");  exit(1);
	}
	// 클라이언트로부터 연결요청을 기다림
	listen(listen_sock, 5);
	return sd;
}

void log_msg(char* msg)
{
   FILE* fp;

   fp = fopen("chatlog.log", "a+");
   fprintf(fp, "%s\n", msg);
   fclose(fp);
}