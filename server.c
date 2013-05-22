#include "server.h"


int32 main(void)
{
	int32 sockfd;
	struct sockaddr_in serv_addr;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == ERR)
		pri_err("socket");

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	//inet_pton(AF_INET, IP, &serv_addr.sin_addr);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//端口重用
	int32 on = 1;
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

void accept_client(int32 sockfd)
{
	int32 newfd, count = 0;
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
		if((pthread_create(&clients[count].tid, NULL, pthread_func,&clients[count].sockfd)) == ERR)
			pri_err("pthread_create");
	}
	close(sockfd);
	close(newfd);
}

void client_exit(sLoginInfo *send, int32 exit_sockfd)
{
	int32 count_fd;
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
	int32 newfd = *((int32 *)arg);
	int8 buf[BUF_SIZE] = {0};
	int32 nread = 0;
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

void analyze_type(sLoginInfo *send, int32 newfd)
{
	int32 count;
	for(count = 0; count < pair[count].flag != 0;count++)
	{
		if(send->type == pair[count].flag)
		{
			pair[count].func(send, newfd);
			break;
		}
	}
}

void register_user(sLoginInfo *send, int32 newfd)
{
	pthread_mutex_lock(&g_mutex);
	int8 *file[3];
	int8 read_buf[BUF_SIZE] = {0};
	int8 all_buf[BUF_SIZE] = {0};
	int32 i;
	int32 fd, nwrite, enter_write;
	int32 user_login_flag = 0;
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
		write(newfd, send,sizeof(sLoginInfo));
	}else{
		sprintf(all_buf, "%s:%s",send->user,send->passwd);
		nwrite = write(fd, all_buf,strlen(all_buf)+1);
		enter_write = write(fd, "\n", 1);
		if(nwrite != 0 && enter_write != 0)
		{
			send->type = REGIST_SUCCESS;//type 52
			write(newfd, send, sizeof(sLoginInfo));
		}else{
			send->type = REGIST_FAILED;//type 50
			write(newfd, send, sizeof(sLoginInfo));
		}
	}
	pthread_mutex_unlock(&g_mutex);
	close(fd);
}

void check_login(sLoginInfo *send, int32 newfd)
{
	int8 *file[3];
	int8 read_buf[BUF_SIZE] = {0};
	int8 all_buf[BUF_SIZE];
	int32 i;
	int32 fd, nwrite, enter_write;
	int32 user_login_flag = 0;
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
	write(newfd, send, sizeof(sLoginInfo));
	get_online_user(send,newfd );
	close(fd);
}

/**/
void get_online_user(sLoginInfo *send, int32 newfd)
{
	int32 count;
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
			write(newfd, send, sizeof(sLoginInfo));
			//strcat(user_buf,buf);
		}
	}
	if(strcmp(user_buf,"") == OK)
	{
		memcpy(send->user, no_user_online,sizeof(no_user_online));
		write(newfd, send, sizeof(sLoginInfo));
	}
	//else
	//	write(newfd, user_buf, strlen(user_buf)+ 1);
}

void private_chat(sLoginInfo *send, int32 newfd)
{
	int8 dest[BUF_SIZE] = {0};
	int8 no_user_online[] = {"user on online! or no user\n"};
	int32 dest_fd;
	int32 subnet;
	subnet = diff_subnet(send, newfd);

	dest_fd = get_sockfd(send->user);
	if(dest_fd == NO_NAME)//target fd 0
	{
		memcpy(send->msg, no_user_online, sizeof(no_user_online));
		write(newfd, send, sizeof(sLoginInfo));

	}else{
		if(subnet /* case ?:&& link ok*/)
		{
			write(newfd, send, sizeof(sLoginInfo));
			/*case ?:read link ok*/
			/*ansewer ?*/
			read(newfd, send, sizeof(sLoginInfo));
			if(send->type == LINK_NO)
				return ;
			else{
				write(dest_fd, send, sizeof(sLoginInfo));
			}
	
		}else{
				//format_buf(dest,send->msg, newfd);
				//write(dest_fd, dest, strlen(dest)+1);//error newfd
				write(dest_fd, send, sizeof(sLoginInfo));
		}
	}
}

void public_chat(sLoginInfo *send, int32 newfd)
{
	int32 count;
	for(count = 0; count < MAX_USER; count++)
	{
		if(clients[count].sockfd == SOCKET_NULL)
			continue;
		else
			write(clients[count].sockfd, send->msg, BUF_SIZE);
	}
	
}

void trans_file(sLoginInfo *send, int32 newfd)
{
	int32 dest_fd = get_sockfd(send->user);

	if(subnet)
	{
		write(newfd, send, sizeof(sLoginInfo));
		/*	?	*/
		read(newfd, send, sizeof(sLoginInfo));
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


void pri_err(int8 *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
