#include"server.h"
extern int32 init_user(int8 *file[]);
extern int32 check_online(int8 *file[]);

int32 init_user(int8 *file[])
{
	int32 count;
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

int32 check_online(int8 *file[])
{
	int32 count;
	for(count = 0; count < MAX_USER; count++)
	{
		if((strcmp(clients[count].user_name, file[0])== OK) && (clients[count].online == IS_ONLINE))
			return IS_ONLINE;
	}
	return NO_ONLINE;
}
