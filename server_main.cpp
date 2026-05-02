#include "common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

int main()
{
	SOCKET server_sock;
	if(!init_tcp_server(server_sock, SERVERPORT)) return 1;

	printf("\n[TCP Server] Server Initialized: Port=%d\n", SERVERPORT);

	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(clientaddr);
	SOCKET client_sock;
	while(1)
	{
		if(accept(server_sock, (struct sockaddr *)&clientaddr, &addrlen)  == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP Server] Client Connected: IP=%s, Port=%d\n", addr, ntohs(clientaddr.sin_port));

		char buf[BUFSIZE+1];
		int retval;
		while(1)
		{
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if(retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			if(retval == 0) break;

			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

			retval = send(client_sock, buf, retval, 0);
			if(retval == SOCKET_ERROR)
			{
				err_display("server.Send()");
				break;
			}
		}
		close_tcp_server(client_sock, false);
		
		printf("[TCP Server] Client Quit: IP=%s, Port=%d\n",
			addr, ntohs(clientaddr.sin_port));
	}
	close_tcp_server(server_sock, true);
	return 0;
}
