#include<iostream>
#include<cstdlib>
#include<cstring>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
using namespace std;

void erromessage(const char * message) {
        cerr << message << endl;
        exit(1);
}

void setSOCKIN(sockaddr_in &sock,const char* port) {
        memset(&sock, 0, sizeof(sock));
        sock.sin_family = AF_INET;
        sock.sin_addr.s_addr = htonl(INADDR_ANY);
        sock.sin_port = htons(atoi(port));
}


void *readSock(void *arg){
	int sock  = *(int *)arg;
	
	char msg[1000];
	while(1){
		if(read(sock,msg,sizeof(msg)) < 0){
			cout << "connection break"<<endl;
			exit(0);
		}
		printf("\nrecv : %s",msg);
		cout << "send msg : ";
		cout.flush(); //not buffering
	}

}



int main(int argc, char * argv[]) {

        int hserveSock, hClntSock;
        sockaddr_in servAddr, clntaddr;
        socklen_t szClntAddr;

        char message[1000];
		char sendmsg[1000];
        if (argc != 2) {
                cout << "Usage: ./filename portnum" << endl;
                exit(1);
        }
		
		//create Socket
        hserveSock = socket(PF_INET, SOCK_STREAM, 0);
        if (hserveSock == -1) {
                erromessage("server can't open");
        }
		
		//setting socket
        setSOCKIN(servAddr,argv[1]);

        if (bind(hserveSock, (sockaddr *)&servAddr, sizeof(servAddr)) == -1) {
                erromessage("bind fail");
        }
		
		//listen
        if (listen(hserveSock, 5) == -1) erromessage("listen fail");

        szClntAddr = sizeof(clntaddr);
        cout <<"port: " << argv[1] <<" accept excute!" << endl;

        //accept!!
        hClntSock = accept(hserveSock, (sockaddr *)&clntaddr, &szClntAddr);
		if (hClntSock == -1) erromessage("accep fail");

		// read thread
		pthread_t r1;
		if (pthread_create(&r1,NULL,readSock,(void *)&hClntSock) < 0)
			erromessage("thread create fail");


		// write 
        while(1){

                cout << "send msg : ";
                fgets(sendmsg,sizeof(sendmsg),stdin);
				sendmsg[strlen(sendmsg)-1] = '\0'; // rm 'n\'

                write(hClntSock, sendmsg, sizeof(sendmsg));
				if(strcmp(sendmsg,"exit")==0)
					break;
        }

        close(hserveSock);
        close(hClntSock);
        exit(1);
        return 0;
}
