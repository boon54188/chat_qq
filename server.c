#include "server.h"


int main(void)
{
	int sockfd;
	struct sockaddr_in serv_addr;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == ERR)
		pri_err("socket");

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	//inet_pton(AF_INET, IP, &serv_addr.sin_addr);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//端口重用
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
	int newfd, count_fd = 0;
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(struct sockaddr_in);

	while(1)
	{
		if((newfd = accept(sockfd, (struct sockaddr *)&client_addr, &len)) == ERR)
			pri_err("accept");
		printf("user:%s	connect, port %d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));

		while(count_fd < MAX_USER)
		{
			if(clients[count_fd].sockfd == SOCKET_OK)
			{
				clients[count_fd].sockfd = newfd;
				DEBUG("clients[%d]sockfd = %d\n", clients[count_fd].sockfd);
				break;
			}
		}
		if((pthread_create(&clients[count_fd].tid, NULL, pthread_func,&clients[count_fd].sockfd)) == ERR)
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
	char *file[3];
	char read_buf[BUF_SIZE] = {0};
	char all_buf[BUF_SIZE] = {0};
	int i,back_type = 0;
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
			char str[BUF_SIZE] = {0};
			strcpy(str, read_buf);
			file[i++] = strtok(read_buf,":");
			while(file[i++] = strtok(NULL,":"))
				;
			if(strcmp(file[0], send->login_name) == OK)
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
		back_type = REGIST_EXITED; //type 51
		write(newfd, &back_type,sizeof(int));
	}else{
		sprintf(all_buf, "%s:%s",send->login_name,send->login_passwd);
		nwrite = write(fd, all_buf,strlen(all_buf)+1);
		enter_write = write(fd, "\n", 1);
		if(nwrite != 0 && enter_write != 0)
		{
			back_type = REGIST_SUCCESS;//type 52
			write(newfd, &back_type, sizeof(int));
		}else{
			back_type = REGIST_FAILED;//type 50
			write(newfd, &back_type, sizeof(int));
		}
	}
	pthread_mutex_unlock(&g_mutex);
	close(fd);
}

void check_login(sLoginInfo *send, int newfd)
{
	char *file[3];
	char read_buf[BUF_SIZE] = {0};
	char all_buf[BUF_SIZE];
	int i,back_type = 0;
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
		if(strcmp(file[0], send->login_name) == OK)
		{
			if(strcmp(file[1], send->login_passwd) == OK)
			{
				bzero(&back_type,sizeof(int));
				if(init_user(file))
					back_type = USER_LOGIN_FAILED_ONLINE;//user online
				else
				{
					back_type = USER_LOGIN_SUCCESS;
				}
				break;
			}else{

				bzero(&back_type, sizeof(int));
				back_type = USER_LOGIN_PASSWD_ERROR;//passwd error
				break;
			}
		}else{
			bzero(&back_type,sizeof(int));
			back_type = USER_LOGIN_FAILED;//no user name
		}
		memset(read_buf, 0,sizeof(read_buf));
	}
	write(newfd, &back_type, sizeof(int));
	get_online_user(send,newfd );
	close(fd);
}

void get_online_user(sLoginInfo *send, int newfd)
{
	int count;
	char buf[BUF_SIZE] = {0};
	char user_buf[BUF_SIZE] = {0};
	char no_user_online[] = {"on user online, only you!"};
	for(count = 0; count < MAX_USER; count++)
	{
		if((clients[count].sockfd != newfd) && (clients[count].online == IS_ONLINE))
		{
			sprintf(buf, "(user):%s\t", clients[count].user_name);
			write(newfd, buf, strlen(buf)+1);
			//strcat(user_buf,buf);
		}
	}
	if(strcmp(user_buf,"") == OK)
		write(newfd, no_user_online, strlen(no_user_online)+1);
	//else
	//	write(newfd, user_buf, strlen(user_buf)+ 1);
}


void pri_err(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
