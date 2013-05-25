#include"server.h"
void *udp_handle(void *arg)
{
	int sock = *(int *)arg;
    struct sockaddr_in recv_addr;
	socklen_t len = sizeof(struct sockaddr_in);
    struct udp_list *current;
    struct udp_list *current_user;
    struct udp_list *head = NULL;
    sLoginInfo msg;
    sLoginInfo return_msg;

	memset(&msg, 0, sizeof(sLoginInfo));
	recvfrom(sock, &msg, sizeof(sLoginInfo), 0, (struct sockaddr *)&recv_addr, (socklen_t *)&len);
	if((msg.type != HEART_DATA) && (msg.type != CHAT_RETURN)) { //接收到的是非心跳包和序列号时返回序列号
	return_msg.type = CHAT_RETURN;
	if(sendto(sock, &return_msg, sizeof(sLoginInfo), 0, (struct sockaddr *)&recv_addr, (socklen_t )(sizeof(struct sockaddr)))< 0)
		pri_err("sendto");
	}
	switch(msg.type) {
		case HEART_DATA:
						printf("add %s to list\n", msg.user);
						printf("HEART_DATA\n");
						head = add_list(head, &msg, &recv_addr);    //将用户和用户的IP，端口加入链表
						if(sendto(sock, &msg, sizeof(sLoginInfo), 0, (struct sockaddr *)&recv_addr, (socklen_t )(sizeof(struct sockaddr)))< 0){
							perror("sendto");
							exit(1);
						}
						printf("sendto OK\n");
						break;
		case PUBLIC_CHAT:
						printf("recv PUBLIC_CHAT\n");
						current = find_user(head, &recv_addr); //根据接受到的地址和端口查找用户
						memcpy(msg.user, current->login_name, 25);//将用户名字写入要发送MSG
						traverse_list(head, &recv_addr, sock, &msg);//便利链表，发给除发送用户的其他用户
/*****fa song gei meige kehu *****/
						break;
		case CHAT_REQ:
						current = check_list(head, msg.user);//找到要通信的用户
						current_user = find_user(head, &recv_addr);//找到请求通信的用户
/*******bijiao recv_addr li de IP he yonghyu de shifouyizhi*****/
						if(recv_addr.sin_addr.s_addr == current->user_udp_ip) { //外网IP相同，NAT通信
							msg.type = CHAT_NAT;
							msg.port = current->user_udp_port; //发给请求者要通信用户的IP和端口
							printf("A port is %d\n", msg.port);
							msg.recv_ip = current->user_udp_ip;
							printf("Aip is %d.%d.%d.%d\n", msg.recv_ip&0xff,(msg.recv_ip>>8)&0xff,(msg.recv_ip>>16)&0xff,(msg.recv_ip>>24)&0xff);
							sendto(sock, &msg, sizeof(sLoginInfo), 0, (struct sockaddr *)&recv_addr, (socklen_t )len);
							msg.port = recv_addr.sin_port; //发给要通信的用户 请求通信用户的IP和端口
							msg.recv_ip = recv_addr.sin_addr.s_addr;
							recv_addr.sin_port = current->user_udp_port;
							recv_addr.sin_addr.s_addr = current->user_udp_ip;
							printf("user name:%s\n",current->login_name);
							memcpy(msg.user, current_user->login_name, 25);
							printf("B port is %d\n", msg.port);
							printf("Bip is %d.%d.%d.%d\n", msg.recv_ip&0xff,(msg.recv_ip>>8)&0xff,(msg.recv_ip>>16)&0xff,(msg.recv_ip>>24)&0xff);
							sendto(sock, &msg, sizeof(sLoginInfo), 0, (struct sockaddr *)&recv_addr, (socklen_t )len);
							printf("type is CHAT_NAT\n");
						}else{
							msg.type = CHAT_INET;//外网IP不一直，INET通信,给2个用户返回CHAT_INET type
							sendto(sock, &msg, sizeof(sLoginInfo), 0, (struct sockaddr *)&recv_addr, (socklen_t )len);
							memcpy(msg.user, current_user->login_name, USER_INFO_SIZE);
							recv_addr.sin_port = current->user_udp_port;
							recv_addr.sin_addr.s_addr = current->user_udp_ip;
							sendto(sock, &msg, sizeof(sLoginInfo), 0, (struct sockaddr *)&recv_addr, (socklen_t )len);
							printf("type is CHAT_INET\n");
						}
						break;
		case PRIVAT_CHAT: //收到消息进行转发
						current_user = check_list(head, msg.user);//找到接收消息的用户
						current = find_user(head, &recv_addr);   //找到发消息的用户
						printf("send user is：%s\n", msg.user);
						memcpy(msg.user, current->login_name, USER_INFO_SIZE);
						recv_addr.sin_port = current_user->user_udp_port;  //将消息发出去
						recv_addr.sin_addr.s_addr = current_user->user_udp_ip;
						msg.srnm = current_user->srnm+1;
						current_user->srnm += 1;
						sendto(sock, &msg, sizeof(sLoginInfo), 0, (struct sockaddr *)&recv_addr, (socklen_t )len);
						break;
		case SEND_FILE:
						current_user = check_list(head, msg.user); //找到接收的用户后进行转发
						recv_addr.sin_port = current_user->user_udp_port;
						recv_addr.sin_addr.s_addr = current_user->user_udp_ip;
						msg.srnm = current_user->srnm+1;
						current_user->srnm += 1;
						sendto(sock, &msg, sizeof(sLoginInfo), 0, (struct sockaddr *)&recv_addr, (socklen_t )len);
						break;
		default:
						break;
	}
}
