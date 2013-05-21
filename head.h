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
#define BUF_SIZE		512


#define	USER_REGISTER	1		//用户注册
#define	USER_LOGIN		2		//用户登录
#define	PUBLIC_CHAT		3		//全部聊天
#define	PRIVAT_CHAT		4		//私人聊天
#define	GROUP_CHAT		5		//群聊
#define SEND_FILE		6		//发送文件

#define USER_QUIT		9		//用户退出
#define	USER_ONLINE		10		//在线用户
#define HEART_DATA		11		//heart
#define USER_LINE		12		//用户上线

#define LINK_FLAG		20		//p2p连接成功


typedef char	int8;
typedef unsigned short u_int16;
typedef int	int32;
typedef unsigned int u_int32;

enum user_login {
	REGIST_FAILED = 50,				//注册失败
	REGIST_EXITED,					//注册的用户已经存在
	REGIST_SUCCESS,					//注册成功

	USER_LOGIN_FAILED,				//登录失败  
	USER_LOGIN_SUCCESS,				//登录成功
	USER_LOGIN_FAILED_ONLINE,		//登录的用户已在线  
	USER_LOGIN_PASSWD_ERROR			//登录密码错误
};

typedef struct login_info{
	int8 user[USER_INFO_SIZE];		//用户名
	union {
		int8 passwd[USER_INFO_SIZE];		//用户密码 
		int8 msg[BUF_SIZE];
	};
	u_int16 port;
	u_int32 recv_ip;
	int32 type;								//类型（注册/登录）
	int32 srnm;
}sLoginInfo;


#endif
