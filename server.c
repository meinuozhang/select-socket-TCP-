#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>

#define port 8080
#define MAX_FD_NUM 20
int array_fd[MAX_FD_NUM];//for watch fd

int startup()
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("socket");
		exit(1);
	}
	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(port);
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind");
		exit(1);
	}
	if(listen(sock,5)<0)
	{
		perror("listen");
		exit(1);
	}
	return sock;
}
int main()
{
	int listen_sock=startup();

	struct sockaddr_in client;
    socklen_t len =sizeof(client);

	fd_set read_set;//READ 
	int max_fd=listen_sock;

	//init array
	int i=0;
	for(;i<MAX_FD_NUM;i++)
	{
	     array_fd[i]=-1;//set init key=-1
	}
	array_fd[0]=listen_sock;
	while(1)
	{
	  FD_ZERO(&read_set);//empty read_set
      for(i=0;i<MAX_FD_NUM;i++)
     {
        if(array_fd[i]>0)
        {
	      FD_SET(array_fd[i],&read_set);//set array_fd in red_set
          if(max_fd<array_fd[i])
		  {
			  max_fd=array_fd[i];//get max_fd
		  }
		}
	 }
	  switch(select(max_fd+1,&read_set,NULL,NULL,NULL))
	  {
		case 0://timeout
			printf("timeout...");
			break;
		case -1://error
			perror("select");
            break;
		default:
			for(i=0;i<MAX_FD_NUM;i++)
			{
				if(array_fd<0)
				{
					continue;
                }
				//new connect
				else if(array_fd[i]==listen_sock && FD_ISSET(array_fd[i],&read_set))
				{
					int new_sock=accept(array_fd[i],(struct sockaddr*)&client,&len);
					if(new_sock<0)
					{
						continue;
					}
					printf("get new connect: %d\n",new_sock);
					for(i=0;i<MAX_FD_NUM;i++)
					{
                      if(array_fd[i]==-1)
					  {
						  array_fd[i]=new_sock;
						  break;
					  }
					}
					  if(i==MAX_FD_NUM)
					  {
						  close(new_sock);
					  }
				}
				else
				{
					for(i=1;i<MAX_FD_NUM;i++)
					{
						if(array_fd[i]>0 && FD_ISSET(array_fd[i],&read_set))
						{
							char buf[1024];
							memset(buf,'\0',sizeof(buf));
							ssize_t _size=read(array_fd[i],buf,sizeof(buf)-1);
							if(_size<0)
							{		
								printf("read failed");
								close(array_fd[i]);
							}
							else if(_size==0)
							{
								printf("client close...\n");
								close(array_fd[i]);
							}
							else
							{
								printf("client: %s\n",buf);
							}
						}
					}
				}
			}
			break;
	  }
	}

	close(listen_sock);
	return 0;
}
