#include "server.h"


int main(void)
{
	int sockfd, udp_sock;
	struct sockaddr_in serv_addr, udp_addr;
	pthread_t udp_tid;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == ERR)
		pri_err("socket");

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	//inet_pton(AF_INET, IP, &serv_addr.sin_addr);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//udp
	if((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) == ERR)
		pri_err("udp_sock");
	bzero(&udp_addr,sizeof(udp_addr));
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(UDP_PORT);
	udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(udp_sock, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) == ERR)
		pri_err("udp bind");
	if((pthread_create(&udp_tid, NULL, pthread_udp,(void *)&udp_sock)) == ERR);
		pri_err("create udp thread");

	//port reuse
	int on = 1;
	if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) == ERR)
		pri_err("setsockopt");
	if((bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == ERR)
		pri_err("bind");
	if((listen(sockfd, MAX_USER)) == ERR)
		pri_err("listen");

	printf("waitting client connection…………\n");

	accept_client(sockfd);

	return 0;
}

void accept_client(int sockfd)
{
	int newfd, count = 0;
	struct sockaddr_in client_addr;
	sLoginInfo *send;
	socklen_t len = sizeof(struct sockaddr_in);

	while(1)
	{
		if((newfd = accept(sockfd, (struct sockaddr *)&client_addr, &len)) == ERR)
			pri_err("accept");
		printf("user:%s	connect, port %d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
		

		while(count < MAX_USER)
		{
			if(clients[count].sockfd == SOCKET_NULL)//socket zero
			{
				clients[count].sockfd = newfd;
				clients[count].recv_ip = client_addr.sin_addr.s_addr;
				clients[count].recv_port = client_addr.sin_port;
				clients[count].send_flag = 0;//0 group chat no
				DEBUG("clients[%d]sockfd = %d\n", clients[count].sockfd);
				count++;
				break;
			}
		}
		if((pthread_create(&clients[count].tid, NULL, pthread_func,(void *)clients[count].sockfd)) == ERR)
			pri_err("pthread_create");
	}
	close(sockfd);
	close(newfd);
}

void client_exit(sLoginInfo *send, int exit_sockfd)
{
	int count_fd;
	for(count_fd = 0; count_fd < MAX_USER; count_fd++)
	{
		if(clients[count_fd].sockfd == exit_sockfd)
		{
				break;
		}
	}
	fprintf(stderr,"user:%s	exit.\n",clients[count_fd].user_name);
	memset(&clients[count_fd], 0, sizeof(clients[count_fd]));
	close(exit_sockfd);
}

void *pthread_func(void *arg)
{
	int newfd = (int)arg;
	int8 buf[BUF_SIZE] = {0};
	int nread = 0;
	sLoginInfo send, *p_info;
	p_info = &send;
	
	while(1)
	{
		nread = readn(newfd, &send, sizeof(send));
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

void analyze_type(sLoginInfo *send, int newfd)
{
	int count;
	for(count = 0; count < pair[count].flag != 0;count++)
	{
		if(send->type == pair[count].flag)
		{
			pair[count].func(send, newfd);
			break;
		}
	}
}

void register_user(sLoginInfo *send, int newfd)
{
	pthread_mutex_lock(&g_mutex);
	int8 *file[3];
	int8 read_buf[BUF_SIZE] = {0};
	int8 all_buf[BUF_SIZE] = {0};
	int i;
	int fd, nwrite, enter_write;
	int user_login_flag = 0;
	off_t off_len = 0;

	if((fd = open(FILENAME,O_CREAT|O_RDWR|O_APPEND,0644)) == ERR)
		pri_err("open");

	while(1)
	{
		lseek(fd, off_len, SEEK_SET);
		if(read(fd, read_buf, BUF_SIZE) == 0)
		{
			break;
		}else{
			i = 0;
			int8 str[BUF_SIZE] = {0};
			strcpy(str, read_buf);
			file[i++] = strtok(read_buf,":");
			while(file[i++] = strtok(NULL,":"))
				;
			if(strcmp(file[0], send->user) == OK)
			{
				user_login_flag = REGIST_EXITED;
				break;
			}
			off_len += strlen(str) + 2;
			memset(str, 0, sizeof(str));
		}
	}
	if(user_login_flag == REGIST_EXITED)
	{
		send->type = REGIST_EXITED; //type 51
		writen(newfd, send,sizeof(sLoginInfo));
	}else{
		sprintf(all_buf, "%s:%s",send->user,send->passwd);
		nwrite = write(fd, all_buf,strlen(all_buf)+1);
		enter_write = write(fd, "\n", 1);
		if(nwrite != 0 && enter_write != 0)
		{
			send->type = REGIST_SUCCESS;//type 52
			writen(newfd, send, sizeof(sLoginInfo));
		}else{
			send->type = REGIST_FAILED;//type 50
			writen(newfd, send, sizeof(sLoginInfo));
		}
	}
	pthread_mutex_unlock(&g_mutex);
	close(fd);
}

void check_login(sLoginInfo *send, int newfd)
{
	int8 *file[3];
	int8 read_buf[BUF_SIZE] = {0};
	int8 all_buf[BUF_SIZE];
	int i;
	int fd, nwrite, enter_write;
	int user_login_flag = 0;
	off_t off_len = 0;

	if((fd = open(FILENAME, O_RDONLY,0644)) == ERR)
		pri_err("open file");
	while(1)
	{
		lseek(fd, off_len, SEEK_SET);
		if(read(fd, read_buf,BUF_SIZE) == 0)
			break;
		off_len += strlen(read_buf) + 2;
		i = 0;
		file[i++] = strtok(read_buf,":");
		while(file[i++] = strtok(NULL, ":"))
			;
		if(strcmp(file[0], send->user) == OK)
		{
			if(strcmp(file[1], send->passwd) == OK)
			{
				bzero(send,sizeof(sLoginInfo));
				if(init_user(file))
					send->type= USER_LOGIN_FAILED_ONLINE;//user online
				else
				{
					send->type = USER_LOGIN_SUCCESS;
				}
				break;
			}else{

				bzero(send, sizeof(sLoginInfo));
				send->type = USER_LOGIN_PASSWD_ERROR;//passwd error
				break;
			}
		}else{
			bzero(send,sizeof(sLoginInfo));
			send->type = USER_LOGIN_FAILED;//no user name
		}
		memset(read_buf, 0,sizeof(read_buf));
	}
	writen(newfd, send, sizeof(sLoginInfo));
	get_online_user(send,newfd );
	close(fd);
}

/**/
void get_online_user(sLoginInfo *send, int newfd)
{
	int count;
	int8 buf[BUF_SIZE] = {0};
	int8 user_buf[BUF_SIZE] = {0};
	int8 no_user_online[] = {"on user online, only you!"};
	for(count = 0; count < MAX_USER; count++)
	{
		if((clients[count].sockfd != newfd) && (clients[count].online == IS_ONLINE))
		{
			sprintf(user_buf, "(user):%s\t", clients[count].user_name);
			send->srnm = clients[count].online;
			memcpy(send->user, clients[count].user_name, USER_INFO_SIZE);
			writen(newfd, send, sizeof(sLoginInfo));
			//strcat(user_buf,buf);
		}
	}
	if(strcmp(user_buf,"") == OK)
	{
		memcpy(send->user, no_user_online,sizeof(no_user_online));
		writen(newfd, send, sizeof(sLoginInfo));
	}
	//else
	//	write(newfd, user_buf, strlen(user_buf)+ 1);
}

void private_chat(sLoginInfo *send, int newfd)
{
	int8 dest[BUF_SIZE] = {0};
	int8 no_user_online[] = {"user on online! or no user\n"};
	int dest_fd;
	int32 subnet;
	subnet = diff_subnet(send, newfd);

	dest_fd = get_sockfd(send->user);
	if(dest_fd == NO_NAME)//target fd 0
	{
		memcpy(send->msg, no_user_online, sizeof(no_user_online));
		writen(newfd, send, sizeof(sLoginInfo));

	}else{
		if(subnet /* case ?:&& link ok*/)
		{
			writen(newfd, send, sizeof(sLoginInfo));
			/*case ?:readn link ok*/
			/*ansewer ?*/
			readn(newfd, send, sizeof(sLoginInfo));
			if(send->type == LINK_OK)
				return ;
			else{
				writen(dest_fd, send, sizeof(sLoginInfo));
			}
	
		}else{
				//format_buf(dest,send->msg, newfd);
				//write(dest_fd, dest, strlen(dest)+1);//error newfd
				writen(dest_fd, send, sizeof(sLoginInfo));
		}
	}
}

void public_chat(sLoginInfo *send, int newfd)
{
	int count;
	for(count = 0; count < MAX_USER; count++)
	{
		if(clients[count].sockfd == SOCKET_NULL)
			continue;
		else
			writen(clients[count].sockfd, send->msg, BUF_SIZE);
	}
	
}

void trans_file(sLoginInfo *send, int newfd)
{
	int dest_fd = get_sockfd(send->user);

	int32 subnet = diff_subnet(send, newfd);
	if(subnet)
	{
		writen(newfd, send, sizeof(sLoginInfo));
		/*	?	*/
		readn(newfd, send, sizeof(sLoginInfo));
		if(send->type == LINK_OK)
			return ;
		else{
			//file upload
			upload_file(send, newfd);
			down_file(send, dest_fd);
		}
	
	}else{
		//file upload
			upload_file(send, newfd);
			down_file(send, dest_fd);
		}

}

void select_all_chat(sLoginInfo *send, int newfd)
{
	int count;
	for(count=0; count<MAX_USER; count++)
	{
		if((clients[count].online == IS_ONLINE) && (clients[count].send_flag == SEND_ON))
		{
			writen(clients[count].sockfd, &send->msg, sizeof(sLoginInfo));
		}
	}
}

void get_send_flag(sLoginInfo *send, int newfd)
{
	int count;
	for(count = 0; count < MAX_USER; count++)
	{
		if((clients[count].online == IS_ONLINE) && (strcmp(send->user, clients[count].user_name) == OK))
			clients[count].send_flag = SEND_ON;//group send 1
	}
}

void *pthread_udp(void *arg)
{
	int sockfd = *(int *)arg;

	struct epoll_event ev;
	struct epoll_event events[MAX_USER];
	int epfd, nfds, i;
	pthread_t	thread;

	epfd = epoll_create(MAX_USER);
	socklen_t len = sizeof(struct sockaddr_in);
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;

	if(epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) < 0)
		pri_err("epoll_ctl");

	while(1)
	{
		nfds = epoll_wait(epfd, events, MAX_USER, 0);//0  immediate
			if(nfds == -1)
				pri_err("epoll_wait");
		for(i = 0; i < nfds; ++i)
		{
			if(events[i].data.fd == sockfd)
			{
				if(pthread_create(&thread, NULL, udp_handle ,(void *)&events[i].data.fd))
					pri_err("pthread_udp_create");
			}
		}

	}
	close(sockfd);

}

ssize_t readn(int fd, void *buf, size_t count)
{
	ssize_t nleft;
	ssize_t nread;
	int8	*ptr;

	ptr = buf;
	nleft = count;
	while(nleft > 0){
		if((nread = read(fd, ptr, nleft)) < 0){
			if(errno == EINTR)
				nread = 0;		//again
			else
				return (-1);
		}else if(nread == 0)
			break;				//eof
		nleft -= nread;
		ptr += nread;
	}
	return (count - nleft);
}

ssize_t writen(int fd, const void *buf, size_t count)
{
	size_t nleft;
	size_t nwrite;
	const int8 *ptr;

	ptr = buf;
	nleft = count;
	while(nleft > 0){
		if((nwrite = write(fd, ptr, nleft)) <= 0){
			if(errno == EINTR)
				nwrite = 0;		//again
			else
				return (-1);		//error
		}
		nleft -= nwrite;
		ptr += nwrite;
	}
	return count;
}

void pri_err(int8 *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
