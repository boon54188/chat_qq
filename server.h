#ifndef __SERVER_H__
#define __SERVER_H__

#include "head.h"
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>
#include<time.h>
#include<dirent.h>
#include<unistd.h>
#include<errno.h>
#include<sys/epoll.h>

#define ERR			-1
#define SOCKET_NULL	0
#define OK			0
#define NO_NAME		0
#define IS_ONLINE	1
#define NO_ONLINE	0
#define DIFF_SUBNET 0
#define SAME_SUBNET 1
#define SEND_ON		1
#define LINK_OK		20


#define FILENAME		"user"
#define MAX_USER		10

#ifdef DEBUG
#define DEBUG(msg...)	fprintf(stderr, msg)
#else
#define DEBUG(msg...)
#endif

typedef struct info{
	int8 user_name[USER_INFO_SIZE];
	int8 user_passwd[USER_INFO_SIZE];
	u_int16 recv_port;
	u_int32 recv_ip;
	pthread_t tid;
	int sockfd;
	int32 online;
	int32 send_flag;
}client_info;

typedef struct udp_list{
	int8 login_name[USER_INFO_SIZE];
	u_int16 user_udp_port;
	u_int32 user_udp_ip;
	int srnm;
	struct udp_list *next;
}udp_link;


typedef struct function{
	int32 flag;
	void (*func)();
}flag_pairing;

void pri_err(int8 *msg);
void accept_client(int sockfd);
void *pthread_func(void *arg);

ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);

void analyze_type(sLoginInfo *send, int newfd);
void register_user(sLoginInfo *send, int newfd);
void check_login(sLoginInfo *send, int newfd);
void client_exit(sLoginInfo *send, int exit_sockfd);
void get_online_user(sLoginInfo *send, int newfd);
void private_chat(sLoginInfo *send, int newfd);
void public_chat(sLoginInfo *send, int newfd);
void trans_file(sLoginInfo *send, int newfd);
void get_send_flag(sLoginInfo *send, int newfd);
void select_all_chat(sLoginInfo *send, int newfd);

void *pthread_udp(void *arg);
void *udp_handle(void *arg);
udp_link *add_list(udp_link *head, sLoginInfo *send, struct sockaddr_in *recv_addr);
udp_link *check_list(udp_link *head, int8 *p);
udp_link *find_user(udp_link *head, struct sockaddr_in *recv_addr);
void traverse_list(udp_link *head, struct sockaddr_in *recv_addr, int sockfd, sLoginInfo *send);

client_info clients[MAX_USER];

static pthread_mutex_t  g_mutex = PTHREAD_MUTEX_INITIALIZER;
static flag_pairing pair[]={
	{USER_REGISTER, register_user},
	{USER_LOGIN,    check_login},
	{USER_QUIT,     client_exit},
	{USER_ONLINE,	get_online_user},
	{PRIVAT_CHAT,	private_chat},
	{PUBLIC_CHAT,	public_chat},
	{SEND_FILE,		trans_file},
	{GROUP_JOIN,	get_send_flag},
	{GROUP_CHAT,	select_all_chat},
	{0,NULL}
};




#endif
