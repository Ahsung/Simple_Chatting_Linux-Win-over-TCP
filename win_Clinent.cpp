#include<iostream>
#include<cstdlib>
#include<winSock2.h>
#include<stdio.h>
#include<cstring>
#include<stdlib.h>
#include<string>
#include<process.h>
#define lenMsg 1000
using namespace std;

void erromessage(const char * message) {
	cerr << message << endl;
	exit(1);
}
void setSOCKIN(SOCKADDR_IN &sock, char * ip, uint16_t port) {
	memset(&sock, 0, sizeof(sock));
	sock.sin_family = AF_INET;
	sock.sin_addr.s_addr = htonl(inet_addr(ip));
	sock.sin_port = htons(port);
}

void readSock(void *arg) {
	SOCKET hSocket = *(SOCKET *)arg;
	char msg[lenMsg];
	while (1) {;
	if (recv(hSocket, msg, sizeof(msg), 0) <= 0) {
		cout << "connection is break" << endl;
		exit(0);
	}
		printf("\nrecv : %s\n", msg);
		cout << "send msg : ";
		cout.flush();
	}
}

int main(int argc, char * argv[]) {
	WSADATA wsadata;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char message[lenMsg];
	char ip[20], port[10];
	int strLen = 0;
	if (argc != 3) {
		//cout << "Usage: 프로세스name ip port 순서로 입력" << endl;
		//exit(1);
		cout << "Input Ip: ";
		cin >> ip;
		cout << "Input Port:";
		cin >> port;
		cin.ignore();
	}
	else
		strcpy(ip, argv[1]), strcpy(port, argv[2]);


	cout << "ip: " << ip << " port: " << port << " 연결시도!" << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		erromessage("WSAStartup이 잘못됨");
	}

	//소켓 생성
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET) erromessage("클라이언트 소켓 안열림");
	
	//소켓에 주소할당.
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(atoi(port));
	if (connect(hSocket, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) erromessage("connect 실패");
	
	if (_beginthread(readSock, sizeof(char) * lenMsg + sizeof(SOCKET), &hSocket) < 0)
		erromessage("fail create thread");

	while (1) {
		cout << "send msg : ";
		fgets(message,sizeof(message),stdin);
		if (message[0] == '0') break;

		strLen = send(hSocket, message, sizeof(message), 1);
		if (strLen < 0)
			erromessage("서버에 내용 전송 실패");
		if (strcmp(message, "exit") == 0)
			break;
	}

	closesocket(hSocket);
	WSACleanup();
	return 0;
}
