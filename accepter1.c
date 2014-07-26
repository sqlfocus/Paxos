#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
//#include <conio.h>
#include <setjmp.h>

jmp_buf Jump_Buffer;

#define try if(!setjmp(Jump_Buffer))
#define catch else
#define throw longjmp(Jump_Buffer,1)

char *confirm_proposer = "1 proposer Confirmed!";
char *confirm_send = "1 Send Confirm!";
char *reject_proposer = "0 proposer Rejected!";
char *reject_send = "0 Send Reject";
char *reject_format = "0 Format Wrong";
char *data = "Server to Client String!\n";
int sn=0;

int check_proposer(char data , int* sn)
{
	printf("now sn is %d",*sn);
	int rev=data-'0';
	if(0<=rev&&rev<=9&&rev>*sn)
		{
			*sn=rev;
			printf("proposer id is %d now sn is %d\n",rev,*sn);
			return 1;
		}
	printf("proposer id is %d now sn is %d\n",rev,*sn);
	return 0;
}

void* do_loop(void *clientdata)
{
	printf("create thread successful\n");
	int client_sockfd=*((int *)clientdata);
	
	int fd1;
	struct sockaddr_in address1;
	int address_len1;
	int rtval1;
	int len1;
	fd1 = socket(AF_INET, SOCK_STREAM, 0);

	address1.sin_family = AF_INET;
	address1.sin_addr.s_addr = inet_addr("127.0.0.1");
	address1.sin_port = htons(8888);
	address_len1 = sizeof(address1);
	rtval1 = connect(fd1, (struct sockaddr *)&address1, address_len1);
	if(rtval1 == -1) 
	{
		printf("connect to learner1 failed\n");
	}
	else
	{
		printf("learner1 connected\n");
	}
	int fd2;
	struct sockaddr_in address2;
	int address_len2;
	int rtval2;
	int len2;

	fd2 = socket(AF_INET, SOCK_STREAM, 0);

	address2.sin_family = AF_INET;
	address2.sin_addr.s_addr = inet_addr("127.0.0.1");
	address2.sin_port = htons(8889);
	address_len2 = sizeof(address2);
	rtval2 = connect(fd2, (struct sockaddr *)&address2, address_len2);
	if(rtval2 == -1) 
	{
		printf("connect to learner2 failed\n");
	}
	else
	{
		printf("learner2 connected\n");
	}

	try{
	while(1)
	{
		char data2[100];
		memset((void*)data2,0,100);
		read(client_sockfd, (void *)data2, 99);
		if(strlen(data2)==0)
			pthread_exit(NULL);
		printf("Server read line:%s\n",data2);
		if(data2[0]=='0')
		{
			if(check_proposer(data2[2],&sn))
			{
				write(client_sockfd, (void *)confirm_proposer, strlen(confirm_proposer));
				printf("Server send line:%s\n",confirm_proposer);
			}
			else
			{
				write(client_sockfd, (void *)reject_proposer, strlen(reject_proposer));
				printf("Server send line:%s\n",reject_proposer);
			}
		}
		else
			if(data2[0]=='1')
			{
				printf("the id now is %d\n",data2[2]-'0');
				if(data2[2]-'0'==sn)
				{
					printf("Server confirm send\n");
					write(client_sockfd, (void *)confirm_send, strlen(data));

					write(fd1,(void*)data2, strlen(data2));
					write(fd2,(void*)data2, strlen(data2));
				}
				else
				{
					printf("server reject send\n");
					write(client_sockfd, (void *)reject_send, strlen(reject_proposer));
				}
			}
			else
			{
				write(client_sockfd, (void *)reject_format, strlen(reject_proposer));
			}
	}
	}
	catch{
	pthread_exit(NULL);}
}

int main()
{
	int fd;
	int address_len;
	struct sockaddr_in address;
	int listenfd;
	fd = socket(AF_INET, SOCK_STREAM ,0);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(7777);
	address_len = sizeof(address);
	bind(fd, (struct sockaddr *)&address, address_len);

	listenfd = listen(fd,100);

	while(1)
	{
		struct sockaddr_in client_address;
		int len;
		int client_sockfd;

		printf("waiting...\n");
		fflush(stdout);

		len = sizeof(client_address);
		client_sockfd = accept(fd,(struct sockaddr *)&client_address, &len);
		printf("come here\n");
		int ret;
		pthread_t p_thread;
		ret=pthread_create(&p_thread,NULL,do_loop,(void *) &client_sockfd);
		
	}
	return 0;
}

