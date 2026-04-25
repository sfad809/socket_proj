#include "../common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

char *SERVERIP = (char *)"127.0.0.1";

int main(int argc, char *argv[])
{
	int retval;
	if(argc > 1) SERVERIP = argv[1];

#if defined(_WIN32) || defined(_WIN64)
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;
#endif

	// socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect
	struct sockaddr_in serveraddr {};
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

	char buf[BUFSIZE + 1];
	int len;

	while(1)
	{
		printf("\n[TCP Client] send:");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL) break;

		len = (int)strlen(buf);
		if(buf[len-1] == '\n') buf[len-1] = '\0';
		len = strlen(buf);
		if(len == 0) break;

		retval = send(sock, buf, len, 0);
		if(retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}
		printf("[TCP Client] send %d bytes\n", retval);

		retval = recv(sock, buf, retval, MSG_WAITALL);
		if(retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if(retval == 0) break;
		buf[retval] = '\0';
		printf("[TCP Client] recieved: %s(%d byte)\n", buf, retval);
	}
	closesocket(sock);
	
#if defined(_WIN32) || defined(_WIN64)
	WSACleanup();
#endif
	return 0;
}