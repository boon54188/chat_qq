#include"../server.h"
void list_file(sLoginInfo *send, int newfd)
{
	DIR *dir;
	struct dirent *dp;
	if((dir = opendir("~/file")) == NULL)
		pri_err("opendir");
	memset(send->file.buff,0 ,sizeof(send->file.buff));

	while((dp = readdir(dir)) != NULL)
	{
		if((strcmp(dp->d_name, ".") == OK) || strcmp(dp->d_name, "..") == OK)
			continue;
		strcat(send->file.buff, dp->d_name);
		strcat(send->file.buff, " ");
	}
	write(newfd, send->file.buff,strlen(send->file.buff)+1 );
	close(newfd);
}

void down_file(sLoginInfo *send, int32 newfd)
{
	int32 fd, nread = 0;

	if((fd = open(send->file.pwd, O_RDONLY)) == ERR)
		pri_err("open down file");

	memset(send->file.buff, 0, sizeof(send->file.buff));

	while((nread = read(fd, send->file.buff, sizeof(send->file.buff))) > 0)
	{
		usleep(500);// wait 0.5 ms
		write(newfd, &send->file.buff, nread);
	}
	close(fd);
}

void upload_file(sLoginInfo *send, int32 newfd)
{
	int32 fd, nread = 0;

	if((fd = open(send->file.pwd, O_CREAT|O_WRONLY|O_TRUNC, 0644)) == ERR)
		pri_err("open upload file");

	memset(send->file.buff, 0, sizeof(send->file.buff));

	while((nread = read(newfd, send->file.buff, sizeof(send->file.buff))) > 0)
		write(fd, &send->file.buff, nread);

	close(fd);
}
