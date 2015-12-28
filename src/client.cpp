#include "client.h"
#include <netdb.h>
#include <errno.h>

#define DEFAULT_BUFLEN 1024*2
char g_recvbuf[DEFAULT_BUFLEN];
int g_recvbuflen = DEFAULT_BUFLEN;

pthread_mutex_t workmutex;
Client *g_pClient = NULL;
Client::Client()
{
pthread_mutex_init(&workmutex,NULL);
g_pClient = this;
}
Client::~Client()
{
}
int Client::Init()
{

	string webAdd = "agps.u-blox.com";
	int nPort=46434; 

	struct hostent* host;
	host = gethostbyname(webAdd.c_str());

	cli.sin_family = AF_INET;
	cli.sin_port = htons(nPort);
	//cli.sin_addr.s_addr = inet_addr(*(host->h_addr));
	//cli.sin_addr.s_addr =*((struct in_addr *)host->h_addr); 
	//cli.sin_addr.s_addr = *(in_addr_t *)host->h_addr; 
cli.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
	cClient = socket(AF_INET, SOCK_STREAM, 0);
	if(cClient < 0)
	{
		printf("socket() failure!\n");
		return -1; 
	}




	return 1;
}
void * Client::workThread( void * arg )
{
	Client *pMe = (Client *)arg;
	while(1)
{
	pMe->doWork();
	sleep(60*10);
}
	return NULL;
}
int Client::Start()
{
	pthread_t thread_id;
	if(pthread_create(&thread_id, NULL, workThread, this) != 0){
		return -1;
	}
	return -1;
}

int Client::doWork(void)
{
	if (connectServer())
	{
		OnSend();
		OnRecv();
		disconnect();
	}
	return 0;
}
int Client::connectServer(void)
{
	if(!Init())
		return -1;
	for (int i=0 ; i<5; i++)
	{
		if(connect(cClient, (struct sockaddr*)&cli, sizeof(cli)) < 0)
		{
					fprintf(stderr, "connect  %s\n",strerror(errno));
		}
		else
		{
			return 1;
		}
	}
	return 1;
}

int Client::disconnect()
{
	close(cClient);
	printf("disconnect\n");

	return 0;
}
int Client::OnRecv(void)
{
	char cbuf[4096] = {0};
	memset(cbuf, 0, sizeof(cbuf));
	int cLen = 0;
	cLen = recv(cClient, cbuf, sizeof(cbuf),0);    
	if((cLen < 0)||(cLen == 0))
	{
		printf("recv() failure!\n");
		return -1;
	}
	printf("recv() Data From Server: len:%d  [%s]\n", cLen,cbuf);
	SetResInfo(cbuf,cLen);
	return 0;
}

int Client::OnSend(void)
{
	char gps_info_buffer[100]={0};
	sprintf(gps_info_buffer,"cmd=eph;user=lang02-7@163.com;pwd=Xdydc;lat=30.51667;lon=114.31667;pacc=1000\n");
	if (send(cClient,gps_info_buffer,strlen(gps_info_buffer)+1,0)>-1)
	{
		printf("Send apgs ok\n");
	}
	else
	{
		printf("Send apgs error\n");

	}

	return 0;
}
int Client::GetResInfo(char *Buffer,int &nlen)
{
	if (g_recvbuflen != 0)
	{
pthread_mutex_lock(&workmutex);
printf("getRes\n");
		memcpy(Buffer,g_recvbuf,g_recvbuflen);
		nlen = g_recvbuflen;
pthread_mutex_unlock(&workmutex);
		return 0;
	}
	return -1;
}

int Client::SetResInfo(char *Buffer,int nlen)
{
	if(!(strstr(Buffer,"application")))
		return -1;

pthread_mutex_lock(&workmutex);
	memcpy(g_recvbuf,Buffer,nlen);
	g_recvbuflen = nlen;

pthread_mutex_unlock(&workmutex);
	return 0;
}
