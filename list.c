#include "server.h"

udp_link *add_list(udp_link *head, sLoginInfo *send, struct sockaddr_in *recv_addr)
{
	if(head == NULL) {
		udp_link *tmp = (udp_link *)malloc(sizeof(udp_link));
		memcpy(tmp->login_name, send->user, USER_INFO_SIZE);
		tmp->user_udp_port = recv_addr->sin_port;
		tmp->user_udp_ip = recv_addr->sin_addr.s_addr;
		tmp->srnm = 0;
		tmp->next = NULL;	
		return tmp;
	}
	else {
		udp_link *temp = head;
		while(1) {
			if(strcmp(temp->login_name, send->user) == 0) {
				memcpy(temp->login_name, send->user, USER_INFO_SIZE);
				temp->user_udp_port = recv_addr->sin_port;
				temp->user_udp_ip = recv_addr->sin_addr.s_addr;
				temp->srnm = 0;
				return head;
			}
			if(temp->next == NULL)
				break;
			temp = temp->next;		
		}
		udp_link *tmp = (udp_link *)malloc(sizeof(udp_link));
		memcpy(tmp->login_name, send->user, USER_INFO_SIZE);
		tmp->user_udp_port = recv_addr->sin_port;
		tmp->user_udp_ip = recv_addr->sin_addr.s_addr;
		temp->srnm = 0;
		tmp->next = NULL;	
		temp->next = tmp;
		return head;
	}
}

udp_link *check_list(udp_link *head,int8 *p)
{
	udp_link *tmp = head;
	while(tmp != NULL) {
		if(strcmp(tmp->login_name, p) == 0) 
			return tmp;
		tmp = tmp->next;	
	}
	return NULL;
}

udp_link *find_user(udp_link *head,struct sockaddr_in *recv_addr)
{
	udp_link *tmp = head;
	while(tmp != NULL) {
		if((tmp->user_udp_port == recv_addr->sin_port) && (tmp->user_udp_ip == recv_addr->sin_addr.s_addr))
			return tmp;
	
		tmp = tmp->next;	
	}
	return NULL;

}

void traverse_list(udp_link *head, struct sockaddr_in *recv_addr, int sockfd, sLoginInfo *send)
{
	struct sockaddr_in send_addr;
	send_addr.sin_family = AF_INET;
	udp_link *tmp = head;
	printf("send ip is %d.%d.%d.%d\n", recv_addr->sin_addr.s_addr&0xff,(recv_addr->sin_addr.s_addr>>8)&0xff,(recv_addr->sin_addr.s_addr>>16)&0xff,(recv_addr->sin_addr.s_addr>>24)&0xff);
	printf("*********************\n");
	while(tmp != NULL) {
		if((tmp->user_udp_ip != recv_addr->sin_addr.s_addr)) {
			send_addr.sin_port = tmp->user_udp_port;	
			send_addr.sin_addr.s_addr = tmp->user_udp_ip;	
			send->srnm = tmp->srnm+1;
			tmp->srnm += 1;
			printf("msg srnm is %d\n",send->srnm);
			if(sendto(sockfd, send, sizeof(sLoginInfo), 0, (struct sockaddr *)&send_addr, (socklen_t )(sizeof(struct sockaddr)))< 0)
				pri_err("sendto");
		}	
		tmp = tmp->next;	
	}
	printf("*********************\n");
}
