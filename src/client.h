#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;
class Client{

	public:
		int Init();
		int Start();
		Client();
		~Client();

	static	void * workThread( void * arg );
int GetResInfo(char *Buffer,int &nlen);

int SetResInfo(char *Buffer,int nlen);
	private:
		int cClient;
		struct sockaddr_in cli;
		int getIntervalTime(void);
		int connectServer(void);
		int disconnect();
		int OnRecv(void);
		int OnSend(void);

		int doWork(void);
};
