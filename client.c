#include "client.h"

int main(int argc, const char *argv[])
{
	if(argc != 3){
		fprintf(stderr, "Usage: %s <IP> <PORT>\n", argv[0]);
		return -1;
	}
	socket_init(argv);

	int n;
	while(1){
		printf("*****************************************\n");
		printf("* 1:register 2:login 3:forgetpwd 4:exit *\n");
		printf("*****************************************\n");
		printf("please choose : ");

		scanf("%d", &n);
		getchar();
		
		switch(n){
			case 1:
				write(fd, &n, sizeof(int));
				do_register(&msg);
				break;
			case 2:
				write(fd, &n, sizeof(int));
				if(do_login(&msg) == 1){
					do_operation(&msg);
				}
				break;
				case 3:
				write(fd, &n, sizeof(int));
				do_forget(&msg);
				break;
		 	case 4:
				write(fd, &n, sizeof(int));
				return 0;
				break;
			default:
				printf("please input again!!!\n");
				break;
		}
	}
	return 0;
}
