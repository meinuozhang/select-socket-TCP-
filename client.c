#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<errno.h>
int main()
{
   int read_fd=0;
   int write_fd=1;
   int max_fd=0;
   fd_set read_set;
   fd_set write_set;

   int sock=socket(AF_INET,SOCK_STREAM,0);
   if(sock<0)
   {
	   perror("socket");
	   exit(1);
   }
    struct sockaddr_in remote;
	remote.sin_family=AF_INET;
	remote.sin_port=htons(8080);
	remote.sin_addr.s_addr=inet_addr("192.168.198.128");

	int new_sock=connect(sock,(struct sockaddr *)&remote,sizeof(remote));
	if(new_sock<0)
	{
		perror("connect");
		exit(1);
	}
	if(sock >read_fd)
	   max_fd=sock;
	else
		max_fd=read_fd;
	 while(1)
	 {
		 FD_ZERO(&read_set);//empty
		 FD_ZERO(&write_set);
		 FD_SET(read_fd,&read_set);//set
		 FD_SET(sock,&write_set);//

		 switch(select(max_fd+1,&read_set,&write_set,NULL,NULL))
		 {
			 case 0://timeout	
			 printf("timeout...\n");
				 break;
			case -1://error
			 printf("there are bug\n");
			 printf("error...\n");
				 break;
			default://data ready
				 {
				 if(FD_ISSET(read_fd,&read_set))//judge
				 {
					 char buf[1024];
						 ssize_t _size=read(read_fd,buf,sizeof(buf)-1);
						 if(_size>0)
						 {
						 buf[_size]='\0';
						 printf("echo:%s\n",buf);
						 }
				    	 if(FD_ISSET(sock,&write_set))
					     {
							 //printf("write successful\n");
						  send(sock,buf,strlen(buf),0);
                       // printf("%d\n",_size);
						 }
				
					 }
				 }
				 break;
		 }
	 }
	return 0;
}

