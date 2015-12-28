#include "server.h"
#include "client.h"
extern Client *g_pClient;
Server::Server()
{
}
Server::~Server()
{

	close(listener);
}
int Server::setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;
}
int Server::handle_message(int new_fd)
{
	char buf[MAXBUF + 1];
	memset(buf,'\0',sizeof(buf));
	char sendbuf[MAXBUF+1];
	memset(sendbuf,'\0',sizeof(sendbuf));
	int sendLen;
	int len;
	bzero(buf, MAXBUF + 1);
	len = recv(new_fd, buf, MAXBUF, 0);
	if (len > 0){
		printf("Recv %s\n",buf);
		if(g_pClient == NULL)
			return -1;
		g_pClient->GetResInfo(sendbuf,sendLen);
		printf("send:%s  sendlen:%d\n",sendbuf,sendLen);
		len=send(new_fd,sendbuf,sendLen,0);
		if(len<0)
		{
			printf("send error\n");
		}

	}
	else {
	  return -1;
	}
	return len;
}
int Server::Init() 
{ 


	myport =1725;

	lisnum = 10;

	//rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
	//if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
	//	perror("setrlimit");
	//	exit(1);
	//}

	if ((listener = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	} else
		printf("socket ok\n");

	int opt=SO_REUSEADDR;
	setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	setnonblocking(listener);

	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = PF_INET;
	my_addr.sin_port = htons(myport);
	//if (argc>3)
	//    my_addr.sin_addr.s_addr = inet_addr(argv[3]);
	//else
	my_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(listener, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))== -1) {
		perror("bind");
		exit(1);
	} else
		printf("bind ok\n");

	if (listen(listener, lisnum) == -1) {
		perror("listen");
		exit(1);
	} else
		printf("linsten ok\n");

	kdpfd = epoll_create(MAXEPOLLSIZE);
	len = sizeof(struct sockaddr_in);
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = listener;
	if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
		fprintf(stderr, "epoll set insertion error: fd=%d\n", listener);
		return -1;
	} else
		printf(" add epol ok\n");

	return -1;
}
int Server::doWork(void)
{
	curfds = 1;
	while (1) {
		nfds = epoll_wait(kdpfd, events, curfds, -1);
		if (nfds == -1) {
			perror("epoll_wait");
			break;
		}
		for (n = 0; n < nfds; ++n) {
			if (events[n].data.fd == listener) {
				new_fd = accept(listener, (struct sockaddr *) &their_addr,
						&len);
				if (new_fd < 0) {
					perror("accept");
					continue;
				}
				else
				{
					printf("new fd\n");
				}
				setnonblocking(new_fd);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = new_fd;
				if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, new_fd, &ev) < 0) {
					fprintf(stderr, "add socket '%d' add error %s\n",
							new_fd, strerror(errno));
					return -1;
				}
				curfds++;
			} else {
				ret = handle_message(events[n].data.fd);
				if (ret < 1 && errno != 11) {
					if(epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[n].data.fd,
								&ev) < 0)
					{
					 fprintf(stderr, "del socket%d error%s\n",events[n].data.fd, strerror(errno));
					}
					curfds--;
					close(events[n].data.fd);
				}
			}
		}
	}

	return -1;
}
void * Server::workThread( void * arg )
{
	Server *pMe = (Server *)arg;
	pMe->doWork();
	return NULL;
}
int Server::Start()
{
	pthread_t thread_id;
	if(pthread_create(&thread_id, NULL, workThread, this) != 0){
		return -1;
	}
	return -1;
}
