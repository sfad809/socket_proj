#include "common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

#if defined(WIN32)
DWORD WINAPI ProcessClient(LPVOID arg)
#else
void* ProcessClient(void *arg)
#endif
{
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	socklen_t addrlen;

	int retval;
	char addr[INET_ADDRSTRLEN];
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

	close_tcp_server(client_sock, false);
	printf("[TCP Server] Client disconnected: IP=%s, Port=%d\n", addr, ntohs(clientaddr.sin_port));
	return 0;
}

int main()
{
	SOCKET server_sock;
	if(!init_tcp_server(server_sock, SERVERPORT)) return 1;
	printf("\n[TCP Server] Ready to accept clients on port %d.\n", SERVERPORT);

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(clientaddr);

	HANDLE hThread;
	while(1)
	{
		client_sock = accept(server_sock, (struct sockaddr *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP Server] Client Connected: IP=%s, Port=%d\n", addr, ntohs(clientaddr.sin_port));

#if defined(WIN32)
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if(hThread) close_tcp_server(client_sock, false);
		else CloseHandle(hThread);
#else
		if (pthread_create(&hThread, NULL, ProcessClient, (void*)client_sock) != 0)
		{
			err_display("pthread_create()");
			closesocket(client_sock);
		}
		else pthread_detach(hThread);
#endif
	}
	close_tcp_server(server_sock, true);
	return 0;
}