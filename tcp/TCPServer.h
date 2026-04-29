#pragma once
#include "../common.h"

class TCPServer
{
public:
	TCPServer();
	~TCPServer();

	bool Start(int port);
	void Stop();
	bool IsRunning() const;

	SOCKET Accept(struct sockaddr_in *clientaddr, socklen_t *addrlen);
	int Send(SOCKET clientSock, const char *buf, int len);
	int Recv(SOCKET clientSock, char *buf, int len);
	void CloseClient(SOCKET clientSock);

private:
	SOCKET _listenSock;
};
