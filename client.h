#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>

typedef struct{
	int id;
	char name[32];
	char sex[16];
	int age;
	char phone[16];
	char address[32];
	char passwd[16];
	char remark[16];
}MSG;
MSG msg;
int fd;

void socket_init(const char *argv[])
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == fd){
		perror("socket");
		exit(-1);
	}

	struct sockaddr_in serveraddr = {0};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));
	int len = sizeof(serveraddr);
	if(-1 == connect(fd, (struct sockaddr*)&serveraddr, len)){
		perror("connect");
		close(fd);
		exit(-1);
	}
}

int do_register(MSG *msg)
{
	printf("input your name: ");
	scanf("%s", msg->name);
	getchar();
	printf("input your sex: ");
	scanf("%s", msg->sex);
	getchar();
	printf("input your age: ");
	scanf("%d", &msg->age);
	getchar();
	printf("input your phone: ");
	scanf("%s", msg->phone);
	printf("input your address: ");
	scanf("%s", msg->address);
	getchar();
	printf("input your passwd: ");
	scanf("%s", msg->passwd);
	getchar();
	printf("input your remark: ");
	scanf("%s", msg->remark);

	if(send(fd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send.\n");
		return -1;
	}

	char buf[64] = {};
	read(fd, buf, 64);
	printf("%s\n", buf);

	return 0;
}

int do_login(MSG *msg)
{
	bzero(msg, sizeof(MSG));
	printf("input your name: ");
	scanf("%s", msg->name);
	getchar();
	printf("input your passwd: ");
	scanf("%s", msg->passwd);
	getchar();

	if(send(fd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send.\n");
		return -1;
	}

	char buf[32] = {};
	read(fd, buf, 32);
	if(strncmp(buf, "OK", 2) == 0){
		printf("Login success!\n");
		return 1;
	}
	else{
		printf("%s\n", buf);
	}
	return 0;
}

int do_forget(MSG *msg)
{
	bzero(msg, sizeof(MSG));
	printf("input your name: ");
	scanf("%s", msg->name);
	getchar();
	printf("input your remark: ");
	scanf("%s", msg->remark);
	getchar();

	if(send(fd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send.\n");
		return -1;
	}

	char buf[64] = {};
	read(fd, buf, 64);
	printf("%s\n", buf);

	return 0;
}

int do_update(MSG *msg)
{
	printf("input new passwd: ");
	scanf("%s", msg->passwd);
	getchar();

	if(send(fd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send.\n");
        return -1;
	}
	
	char buf[32] = {};
	read(fd, buf, 32);
	printf("%s\n", buf);
	
	return 0;
}

int do_search(MSG *msg)
{
	char buf[64] = {};

	if(send(fd, msg, sizeof(MSG), 0) < 0){
		printf("fail to send.\n");
		return -1;
	}

	while(1){
		read(fd, buf, 64);
		if(strncmp(buf, "done", 4) == 0){
			break;
		}
		printf("%s\n", buf);
	}
	return 0;
}
int do_operation(MSG *msg)
{
	int num;
	printf("*****************************************\n");
	printf("* 1:update        2:search       3:exit *\n");
	printf("*****************************************\n");
	printf("please choose : ");

	scanf("%d", &num);
	getchar();
	
	switch(num){
		case 1:
			num = 5;
			write(fd, &num, sizeof(int));
			do_update(msg);
			return 0;
			break;
		case 2:
			num = 6;
			write(fd, &num, sizeof(int));
			do_search(msg);
			return 0;
			break;
		case 3:
			return 0;
			break;
		default:
			printf("please input again!!!\n");
			break;
	}
}
#endif 
