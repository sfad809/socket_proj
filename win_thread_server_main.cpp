#include "tcp/TCPServer.h"

#define SERVERPORT 9000
#define BUFSIZE    512

#if defined(WIN32)
DWORD WINAPI ProcessClient(LPVOID arg)
#else
void* ProcessClient(void *arg)
#endif
{
	SOCKET client_sock = *(SOCKET*)arg;
	int retval;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	socklen_t addrlen;

	char buf[BUFSIZE+1];
	
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr *)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while(1)
	{
		retval = recv(client_sock, buf, BUFSIZE, 0); 
		if(retval == SOCKET_ERROR)
		{
			err_display("recv()");
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
	return 0;
}

int main()
{
	TCPServer server;
	if(!server.Start(SERVERPORT)) err_quit("server.Start()");

	int retval;
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	SOCKET client_sock;

	HANDLE hThread;
	while(1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = server.Accept(&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP Server] Client Connected: IP=%s, Port=%d\n", addr, ntohs(clientaddr.sin_port));

#if defined(WIN32)
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)client_sock, 0, NULL);
		if(hThread == NULL) server.CloseClient(client_sock);
		else CloseHandle(hThread);a
#else
		pthread_create(&hThread, NULL, ProcessClient, (void*)&client_sock);
		pthread_detach(hThread);
#endif
	}
	server.Stop();
	return 0;
}
