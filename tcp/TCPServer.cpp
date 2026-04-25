#include "../common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

int main(int argc, char *argv[])
{
	int retval;

#if defined(_WIN32) || defined(_WIN64)
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;
#endif

	// socket
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind
	struct sockaddr_in serveraddr {};
	serveraddr.sin_family      = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port        = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// client
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	char buf[BUFSIZE+1];

	if(inet_ntop(AF_INET, &serveraddr.sin_addr, buf, sizeof(buf)) == nullptr)
	{
		closesocket(listen_sock);
#if defined(_WIN32) || defined(_WIN64)
		WSACleanup();
#endif
		err_quit("inet_ntop()");
	}
	
	printf("\n[TCP Server] Server Initialized: IP:%s, Port=%d\n",
		buf, serveraddr.sin_port);
	while(1)
	{
		// accept
		addrlen = sizeof(clientaddr); // why do this all frame?
		client_sock = accept(listen_sock, (struct sockaddr *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		char addr[INET_ADDRSTRLEN]; // 22, 65
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP Server] Client Connected: IP=%s, Port=%d\n",
			addr, ntohs(clientaddr.sin_port));
		
		// 
		while(1)
		{
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if(retval == SOCKET_ERROR)
			{
				err_display("recv())");
				break;
			}
			else if(retval == 0) break;

			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

			retval = send(client_sock, buf, retval, 0);
			if(retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
		}
		closesocket(client_sock);
		printf("[TCP Server] Client Quit: IP=%s, Port=%d\n",
			addr, ntohs(clientaddr.sin_port));
	}
	closesocket(listen_sock);
#if defined(_WIN32) || defined(_WIN64)
	WSACleanup();
#endif
	return 0;
}