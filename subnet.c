#include"server.h"
extern int32 diff_subnet(sLoginInfo *send, int32 newfd);
int32 diff_subnet(sLoginInfo *send, int32 newfd)
{
	int32 src_count, dest_count;

	for(src_count = 0; src_count < MAX_USER; src_count++)
	{
		if(clients[src_count].sockfd == newfd)	//self_ip
			break;
	}
	for(dest_count = 0; dest_count < MAX_USER; dest_count++)
	{
		if(strcmp(send->user, clients[dest_count].user_name) == OK)
		{
			send->recv_ip = clients[dest_count].recv_ip;
			send->port = clients[dest_count].recv_port;
			break;
		}
	}
	if(clients[src_count].recv_ip == clients[dest_count].recv_ip)
		return SAME_SUBNET; //#define 1
	else
		return DIFF_SUBNET; //#define 0
		
}
