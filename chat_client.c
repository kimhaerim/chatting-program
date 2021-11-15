#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[]) {
	int serv_sock;
	char message[BUF_SIZE];
	char send_message[BUF_SIZE];
	char recv_message[BUF_SIZE];
	int str_len;
	fd_set fd_reads, readtemp;
	int max_fd = 0, result = 0;
	struct timeval timeout;

	struct sockaddr_in serv_adr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected....");
	
	FD_ZERO(&fd_reads);
	FD_SET(0, &fd_reads);
	FD_SET(serv_sock, &fd_reads);
	//FD_SET(clnt_sock, &fd_reads);
	max_fd = serv_sock;
	
	while(1) {
		readtemp = fd_reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;		
		result = select(max_fd+1, &readtemp, 0, 0, &timeout);
		if(result == -1) {
			puts("select() error!");
			break;
		}
		else if(result == 0) {
			puts("Time out!");
		}
		else {
			//키보드에서 입력
			if(FD_ISSET(0, &readtemp)) {
				str_len = read(0, send_message, BUF_SIZE);
				send_message[str_len] = 0;
				write(serv_sock, send_message, str_len);
				printf("messge from console : %s", send_message);
			}
			//serv_sock에서 메시지를 받음
			else if(FD_ISSET(serv_sock, &readtemp)) {
				str_len = read(serv_sock, recv_message, BUF_SIZE);
				recv_message[str_len] = 0;
				if(!strcmp(recv_message, "q\n"))
					break;
				else
					printf("server : %s\n", recv_message);
			}
		}
		/*
		//client에서 메세지가 오면
		if(1 == FD_ISSET(clnt_sock, &fd_reads)) {
			str_len = read(clnt_sock, recv_message, BUF_SIZE);
			recv_message[str_len] = '\0';
			printf("client : %s\n", recv_message);
		}
		//키보드에서 입력을 받음
		else if(1 == FD_ISSET(0, &fd_reads)) {
			//str_len = read(0, send_message, 500);
			fgets(send_message, BUF_SIZE, stdin);
			printf("%s", send_message);
			write(clnt_sock, send_message, str_len);
		}
		*/
	}
	//close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
