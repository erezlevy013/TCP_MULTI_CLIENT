#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ADDR "127.0.0.1"
#define PORT 1234

#define SUM_CLIENTS 1000
#define FAIL -1
#define SUCCESS 0


typedef struct sockaddr_in SocAdd;
typedef struct sockaddr sockaddr;

int CreateClient()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) 
	{
		perror("socket failed");
		return FAIL;
	}
	return sock;
}
int Connect(int _sock)
{
	int clientSock;
	SocAdd serverAdd;

	memset(&serverAdd, 0, sizeof(serverAdd));
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAdd.sin_port = htons(PORT);	

	clientSock = connect(_sock, (sockaddr *) &serverAdd, sizeof(serverAdd));
	if( clientSock < 0 ) 
	{
		perror("connect failed");
		close(_sock);
		return FAIL;
	}
	return _sock;
}

int Recvfrom(int _sock)
{
	char buffer[64];
	int read_bytes;
	read_bytes = recv( _sock, buffer, sizeof(buffer), 0 );
	if(read_bytes <= 0)
	{
		perror("recvfrom failed");	
		close(_sock);
		return FAIL;
	}
	buffer[read_bytes] = '\0';
	printf("read bytes %d ,Client: %s \n",read_bytes, buffer);
	return SUCCESS;
}

int SendTo(int _sock, int _indx)
{
	int sentBytes;
	char data[64]; 
	sprintf(data, "Hi from client %d", _indx);
	sentBytes = send(_sock, data , strlen(data) , 0);
	if (sentBytes < 0 || sentBytes != strlen(data)) 
	{
		perror("sendto failed");
		close(_sock);
		return FAIL;
	}
	printf("Message sent. %d \n", _indx);
	return SUCCESS;
}


int ManClient()
{
	int i=0, arrClients[SUM_CLIENTS] = {0}, cliSock;
	int num = 5000;
	while(1)
	{
		if(arrClients[i] == 0)
		{
			if((rand() % 100) < 30 )
			{
				cliSock = CreateClient();
				if(cliSock > 0)
				{
					if((cliSock = Connect(cliSock)) > 0)
					{
						arrClients[i] = cliSock;
					}
					
				}
			}
		}
		else
		{
			if((rand() % 100) < 5 )
			{
				close(arrClients[i]);
				arrClients[i] = 0;
			}
			else if((rand() % 100) < 30)
			{
				if(SendTo(cliSock, i) < 0)
				{
					arrClients[i] = 0;
				}
				else if(Recvfrom(cliSock) < 0)
				{
					arrClients[i] = 0;
				}
			}
		}
		i = (i+1) %  SUM_CLIENTS;
		/*printf("num = %d \n", num);*/
		
		
	}
	return SUCCESS;
}


int main()
{
	int sock, i;
	ManClient();
	/*ClientStart( sock );*/
	
	return SUCCESS;
}
