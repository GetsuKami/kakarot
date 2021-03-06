#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>


#define PORT 4401
#define MAXPENDING 2

/*
 * TCP不提供帧同步，这使得它对于面向字节流的协议是完美的。这个TCP与UDP的一个重要区别。
 * UDP是面向消息的协议，它保留发送者和接收者之间的消息边界。
 * TCP是一个面向流的协议，他假定在通信的数据是无结构的。
 * */

void process_conn_server(int s)
{
	ssize_t size = 0;
	char buffer[1024];

	memset(buffer,0,1024);


	while(1)
	{
		size = read(s,buffer,1024);
		if(size == 0)
		{
			return ;
		}
		sprintf(buffer, "%d bytes altogether\n", (int)size);
		write(s,buffer,strlen(buffer)+1);
		puts(buffer);
	}
}
int main(void)
{
	int sock_server, sock_clnt;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	pid_t pid;

	sock_server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	

	if (sock_server < 0)
	{
		fprintf(stdout,"socket() error\n");
		return -1;
	}
	else
	{
		puts("创建socket 描述符成功.....");
	}

  int ret,on = 1;
  ret = setsockopt(sock_server,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  //  允许地址重用,取消程序ctrl + c结束后的TIME_WAIT状态。
  if(ret < 0)
  {
    fprintf(stdout,"setsockopt() error.\n");
    return -1;
  }
  else
  {
    puts("setsockopt() success...\n");
  }

	memset(&server_addr,0,sizeof(struct sockaddr_in));
	memset(&server_addr,0,sizeof(struct sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	int err = bind(sock_server,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(err < 0)
	{
		fprintf(stdout,"bind() error\n");
		return -1;
	}
	else
	{
		puts("绑定端口成功......");
	}

	err = listen(sock_server,MAXPENDING);
	if(err < 0)
	{
		fprintf(stdout,"listen() error\n");
		return -1;
	}
	else
	{
		puts("侦听中........");
	}

	while(1)
	{
		socklen_t addrlen = sizeof(struct sockaddr);
		sock_clnt = accept(sock_server,(struct sockaddr *)&client_addr, &addrlen);
		// 将accept()注释之后，client　connect()依然可以连接成功。
		// 代表accept() 发生在TCP三次握手之后。

		if(sock_clnt < 0)
		{
			fprintf(stdout,"accept() error\n");
			sleep(10);
			continue;
		}

		fprintf(stdout,"连接成功...\n");
		
		pid = fork();
		if(0 == pid)
		{
			process_conn_server(sock_clnt);
			close(sock_server);
		}
		else
		{
			close(sock_clnt);
		}
	}
}
