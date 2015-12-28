#include "client.h"
#include "server.h"

int main(int argc,char *argv[])
{
Client client;
client.Start();
Server server;
if(!server.Init())
{
	printf("server init error\n");
	return -1;
}
server.Start();
while(1)
{
 sleep(1000);
}
return 0;
}
