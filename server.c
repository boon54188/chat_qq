#include "head.h"

void analyze_type(sLoginInfo *send, int newfd)
{
	int count;
	for(count = 0; count < p[count].type_flag != 0;count++)
	{
		if(send->type == p[count].type_flag)
		{
			p[count].fun(send, newfd);
			break;
		}
	}
}

void *pthread_func(void *arg)
{
	int newfd = *((int *)arg);
	char buf[BUF_SIZE] = {0};
	int nread = 0;
	sLoginInfo send, *p_info;
	p_info = &send;
	while(1)
	{
		nread = read(newfd, &send, sizeof(send));
		if(nread <= 0)
		{
			DEBUG("recv msg is empty\n");
			break;
		}

		analyze_type(p_info, newfd);
		memset(&send, 0, sizeof(send));
	}
	client_exit(p_info, newfd);
}

int main(void)
{
	int sockfd;
	struct sockaddr_in serv_addr;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < ERR)
		pri_err("socket");

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, IP, &serv_addr.sin_addr);

	//端口重用
	int on = 1;
	if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < ERR)
		pri_err("setsockopt");
	if((bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < ERR)
		pri_err("bind");
	if((listen(sockfd, MAX_USER)) < ERR)
		pri_err("listen");

	printf("waitting client connection…………\n");

	accept_client(sockfd);
}

void accept_client(int sockfd)
{
	int newfd, count_fd = 0;
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(struct sockaddr_in);

	while(1)
	{
		if((newfd = accept(sockfd, (struct sockaddr *)&client_addr, &len)) < ERR)
			pri_err("accept");
		printf("%s	connect, port %d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));

		while(count_fd < MAX_USER)
		{
			if(clients[count_fd].sockfd == SOCKET_OK)
			{
				clients[count_fd].sockfd = newfd;
				DEBUG("clients[%d]sockfd = %d\n", clients[count_fd].sockfd);
				break;
			}
		}
		if((pthread_create(&clients[count_fd].tid, NULL, pthread_fun,&clients[count_fd].sockfd)) < ERR)
			pri_err("pthread_create");
	}
	close(sockfd)
	close(newfd);
}

void pri_err(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
