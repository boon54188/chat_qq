#include "head.h"
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>
#include<time.h>

#define ERR			-1
#define SOCKET_OK	0
#define OK			0
#define IS_ONLINE	1
#define NO_ONLINE	0


#define FILENAME		"user"
#define MAX_USER		10

#ifdef DEBUG
#define DEBUG(msg...)	fprintf(stderr, msg)
#else
#define DEBUG(msg...)
#endif

typedef struct info{
	int sockfd;
	pthread_t tid;
	char user_name[USER_INFO_SIZE];
	char user_passwd[USER_INFO_SIZE];
	int online;
}client_info;



typedef struct function{
	int flag;
	void (*func)();
}flag_pairing;

void pri_err(char *msg);
void accept_client(int sockfd);
void *pthread_func(void *arg);

void analyze_type(sLoginInfo *send, int newfd);
void register_user(sLoginInfo *send, int newfd);
void check_login(sLoginInfo *send, int newfd);

void client_exit(sLoginInfo *send, int exit_sockfd);



client_info clients[MAX_USER];
pthread_mutex_t  g_mutex = PTHREAD_MUTEX_INITIALIZER;

flag_pairing pair[]={
	{USER_REGISTER,	register_user},
	{USER_LOGIN,	check_login},
	{USER_QUIT,		client_exit},
	{0,NULL}
};
