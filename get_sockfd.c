#include"server.h"
extern void format_buf(int8 *dest, int8 *content, int32 newfd);
extern int32 get_sockfd(int8 dest[]);

int32 get_sockfd(int8 dest[])
{
	int32 count;
	for(count = 0; count < MAX_USER; count++)
	{
		if(strcmp(clients[count].user_name, dest) == OK)
			return clients[count].sockfd;
	}
	return 0;
}


void format_buf(int8 *dest, int8 *content, int32 newfd)
{
	int32 count ;
	time_t current_time;
	for(count = 0; count < MAX_USER;count++)
	{
		if(clients[count].sockfd == newfd)
			break;
	}
	time(&current_time);
	sprintf(dest,"\t\t----------------------------------------\n\t\tname:%s\t%s \t\t----------------------------------------\n\t\t %s",clients[count].user_name,ctime(&current_time),content);
}
