#include "tcp/TCPServer.h"

#define SERVERPORT 9000
#define BUFSIZE    512

int main()
{
	TCPServer server;
	if(!server.Start(SERVERPORT)) err_quit("server.Start()");

	printf("\n[TCP Server] Server Initialized: Port=%d\n", SERVERPORT);

	while(1)
	{
		struct sockaddr_in clientaddr;
		socklen_t addrlen = sizeof(clientaddr);
		SOCKET clientSock = server.Accept(&clientaddr, &addrlen);
		if(clientSock == INVALID_SOCKET)
		{
			err_display("server.Accept()");
			break;
		}

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP Server] Client Connected: IP=%s, Port=%d\n",
			addr, ntohs(clientaddr.sin_port));

		char buf[BUFSIZE + 1];
		while(1)
		{
			int retval = server.Recv(clientSock, buf, BUFSIZE);
			if(retval == SOCKET_ERROR)
			{
				err_display("server.Recv()");
				break;
			}
			if(retval == 0) break;

			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

			retval = server.Send(clientSock, buf, retval);
			if(retval == SOCKET_ERROR)
			{
				err_display("server.Send()");
				break;
			}
		}

		server.CloseClient(clientSock);
		printf("[TCP Server] Client Quit: IP=%s, Port=%d\n",
			addr, ntohs(clientaddr.sin_port));
	}

	server.Stop();
	return 0;
}
