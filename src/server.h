#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
using namespace std;


#define MAXBUF 1024*2
#define MAXEPOLLSIZE 10000
class Server
{
	public:
		int Init();	
		int Start();
		Server();
		~Server();
	private:
int doWork(void);
static void * workThread( void * arg );
int setnonblocking(int sockfd);
int handle_message(int new_fd);
    int listener, new_fd, nfds, n, ret;
    struct epoll_event ev;
    int kdpfd, curfds;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    unsigned int myport, lisnum;
    struct epoll_event events[MAXEPOLLSIZE];
    struct rlimit rt;
};
