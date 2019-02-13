#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#define DATABASE "staff.db"

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

sqlite3 *db;
MSG msg;
void do_register(sqlite3 *db,int clientfd, MSG *msg)
{
	char sql[128] = {};
	char *errmsg;
	char **resultp;
	int ncolumn;
	int nrow;

    sprintf(sql, "select * from user_info;");

	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg)!= SQLITE_OK){
		printf("%s\n", errmsg);
	}

	recv(clientfd, msg, sizeof(MSG), 0);
	sprintf(sql, "insert into user_info values(%d, '%s', '%s', %d, '%s', '%s','%s', '%s');",nrow+1, msg->name,\
			msg->sex, msg->age, msg->phone, msg->address, msg->passwd, msg->remark);
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
		write(clientfd, "\nStaff already exist!\nRegister again\n", 64);
	}
	else{
		printf("Register success!\n");
		write(clientfd, "Register success!\n", 64);
	}
	return ;
}

int do_login(sqlite3 *db,int clientfd, MSG *msg)
{
	char sql[128] = {};
	char *errmsg;
	char **resultp;
	int ncolumn;
	int nrow;

	recv(clientfd, msg, sizeof(MSG), 0);

	sprintf(sql, "select * from user_info where name = '%s' and passwd = '%s';",\
			msg->name, msg->passwd);
	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
		return -1;
	}

	if(nrow == 1){
		write(clientfd, "OK", 32);
	}
	else{
		write(clientfd, "usr/passwd wrong!\n", 32);
	}
	return 0;
}

int do_forget(sqlite3 *db,int clientfd, MSG *msg)
{
	char sql[128] = {};
	char *errmsg;
	char **resultp;
	int ncolumn;
	int nrow;

	recv(clientfd, msg, sizeof(MSG), 0);

	sprintf(sql, "select * from user_info where name = '%s' and remark = '%s';",\
			msg->name, msg->remark);
	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
		return -1;
	}
	
	char buf[64] = {};
	if(nrow == 1){
		sprintf(buf, "your passwd is %s", resultp[14]);
		write(clientfd, buf, 64);
	}
	else{
		write(clientfd, "usr/remark wrong!\n", 64);
	}
	return 0;
}

int do_update(sqlite3 *db, int clientfd, MSG *msg)
{
	char sql[128] = {};
	char *errmsg;

	recv(clientfd, msg, sizeof(MSG), 0);

	sprintf(sql, "update user_info set passwd='%s' where name='%s';",\
			msg->passwd, msg->name);
	
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
	}
	
	write(clientfd, "change passwd success!\n", 32);
	return 0;
}

int do_search(sqlite3 *db, int clientfd, MSG *msg)
{
	char sql[128] = {};
	char *errmsg;
	char **resultp;
	int ncolumn;
	int nrow;

	recv(clientfd, msg, sizeof(MSG), 0);

	sprintf(sql, "select * from user_info where name = '%s';", msg->name);
	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
		return -1;
	}
	
	char buf[64] = {};
	sprintf(buf, "id      :   %s", resultp[8]);
	write(clientfd, buf, 64);
	sprintf(buf, "name    :   %s", resultp[9]);
	write(clientfd, buf, 64);
	sprintf(buf, "sex     :   %s", resultp[10]);
	write(clientfd, buf, 64);
	sprintf(buf, "age     :   %s", resultp[11]);
	write(clientfd, buf, 64);
	sprintf(buf, "phone   :   %s", resultp[12]);
	write(clientfd, buf, 64);
	sprintf(buf, "address :   %s", resultp[13]);
	write(clientfd, buf, 64);

	write(clientfd, "done", 64);
	return 0;
}

void *serv_rutine(void *arg)
{
	int clientfd = *(int *)arg;
	int num;
	while(1){
		read(clientfd, &num, sizeof(int));
//		if(-1 == ret || 0 == ret)
//			break;
		switch(num){
			case 1:
				do_register(db,clientfd,&msg);
				break;
			case 2:
			 	do_login(db, clientfd, &msg);
				break;
			case 3:
				do_forget(db, clientfd, &msg);
				break;
			case 4:
				close(clientfd);
				printf("exit!\n");
				pthread_exit(NULL);
				break;
			case 5:
				do_update(db, clientfd, &msg);
				break;
			case 6:
				do_search(db, clientfd, &msg);
				break;
			default:
				break;
		}
	}
}

int main(int argc, const char *argv[])
{
	char *errmsg;
	int cmd;
	
	if(argc != 3){
		fprintf(stderr, "Usage: %s <IP> <PORT>\n", argv[0]);
		return -1;
	}

	if(sqlite3_open(DATABASE, &db) != SQLITE_OK){
		printf("%s\n", sqlite3_errmsg(db));
		return -1;
	}
	else{
		printf("Open DATABASE success.\n");
	}

	if(sqlite3_exec(db, "create table user_info(id Integer, name char primary key, sex char,age Integer, phone char, address char, passwd char, remark char);",\
				NULL, NULL, &errmsg) != SQLITE_OK){
		printf("%s\n", errmsg);
	}
	else{
		printf("create table or open success.\n");
	}

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == listenfd){
		perror("socket");
		return -1;
	}
	
	struct sockaddr_in serveraddr = {0}, clientaddr = {0};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));
	int len = sizeof(serveraddr);
	if(-1 == bind(listenfd, (struct sockaddr*)&serveraddr, len)){
		perror("connect");
		return -1;
	}
	listen(listenfd, 10);

	int clientfd;
	while(1){
		clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
		if(-1 == clientfd){
			perror("accept");
			return -1;
		}
		pthread_t t;
		pthread_create(&t, NULL, serv_rutine, (void *)&clientfd);
		pthread_detach(t);
	}
	close(listenfd);
	return 0;
}
