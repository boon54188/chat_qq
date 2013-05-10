#include"server.h"
extern void format_buf(char *dest, char *content, int newfd);
extern int get_sockfd(char dest[]);

int get_sockfd(char dest[])
{
	int count;
	for(count = 0; count < MAX_USER; count++)
	{
		if(strcmp(clients[count].user_name, dest) == OK)
			return clients[count].sockfd;
	}
	return 0;
}

void format_buf(char *dest, char *content, int newfd)
{
	int count ;
	time_t current_time;
	for(count = 0; count < MAX_USER;count++)
	{
		if(clients[count].sockfd == newfd)
			break;
	}
	time(&current_time);
	sprintf(dest,"\t\t----------------------------------------\n\t\tname:%s\t%s \t\t----------------------------------------\n\t\t %s",clients[count].user_name,ctime(&current_time),content);
}
