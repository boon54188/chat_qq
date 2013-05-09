#include"server.h"
extern int init_user(char *file[]);
extern int check_online(char *file[]);

int init_user(char *file[])
{
	int count;
	if(check_online(file))
		return IS_ONLINE;
	for(count = 0; count < MAX_USER; count++)
	{
		if(clients[count].online == NO_ONLINE)
		{
			strcpy(clients[count].user_name, file[0]);
			strcpy(clients[count].user_passwd, file[1]);
			clients[count].online = IS_ONLINE;
			break;
		}
	}
	return NO_ONLINE;
}

int check_online(char *file[])
{
	int count;
	for(count = 0; count < MAX_USER; count++)
	{
		if(clients[count].online == IS_ONLINE)
			return IS_ONLINE;
	}
	return NO_ONLINE;
}
