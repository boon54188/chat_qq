#ifndef _HEADER_H
#define _HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define IP				"10.5.22.10"
#define PORT			12345
#define USER_INFO_SIZE	25
#define BUF_SIZE		1024


#define	USER_REGISTER	1		//用户注册
#define	USER_LOGIN		2		//用户登录
#define USER_QUIT		3		//用户退出
#define	PUBLIC_CHAT		4		//全部聊天
#define	PRIVAT_CHAT		5		//私人聊天
#define	GROUP_CHAT		6		//群聊
#define SEND_FILE		7		//发送文件
#define	USER_ONLINE		8		//在线用户


enum user_login {
	REGIST_FAILED = 10,				//注册失败
	REGIST_EXITED,					//注册的用户已经存在
	REGIST_SUCCESS,					//注册成功

	USER_LOGIN_FAILED,				//登录失败  
	USER_LOGIN_SUCCESS,				//登录成功
	USER_LOGIN_FAILED_ONLINE,		//登录的用户已在线  
	USER_LOGIN_PASSWD_ERROR			//登录密码错误
};

typedef struct login_info{
	int type;								//类型（注册/登录）
	char login_name[USER_INFO_SIZE];		//用户名
	char login_passwd[USER_INFO_SIZE];		//用户密码 
}sLoginInfo;


#endif
