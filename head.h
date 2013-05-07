#ifndef _HEADER_H
#define _HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ERR			0
#define SOCKET_OK	0

#define IP				"10.5.22.10"
#define PORT			12345
#define FILENAME		"user"
#define USER_INFO_SIZE	25
#define MAX_USER		10
#define BUF_SIZE		1024

#ifdef DEBUG
#define DEBUG(msg……)	fprintf(stderr, msg)
#else
#define DEBUG(msg……)
#endif

enum func {
	USER_REGISTER = 1, //用户注册
	USER_LOGIN,        //用户登录
	USER_QUIT          //用户退出
};

enum user_login {
	REGIST_FAILED,                //注册失败
	REGIST_EXITED,                //注册的用户已经存在

	USER_LOGIN_FAILED,            //登录失败  
	USER_LOGIN_SUCCESS,           //登录成功
	USER_LOGIN_FAILED_ONLINE,     //登录的用户已在线  
	USER_LOGIN_PASSWD_ERROR       //登录密码错误
};

typedef struct login_info {
	int type;                 //类型（注册/登录）
	char name[USER_INFO_SIZE];            //用户名
	char passwd[USER_INFO_SIZE];          //用户密码 
}sLoginInfo;

typedef struct client_info{
	int sockfd,
	pthread_t tid;
};

client_info clients[MAX_USER];

#endif
