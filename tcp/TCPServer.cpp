#include "TCPServer.h"

TCPServer::TCPServer()
	: _listenSock(INVALID_SOCKET)
{
#if defined(_WIN32) || defined(_WIN64)
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

TCPServer::~TCPServer()
{
	Stop();
#if defined(_WIN32) || defined(_WIN64)
	WSACleanup();
#endif
}

bool TCPServer::Start(int port)
{
	if(IsRunning()) return true;

	_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if(_listenSock == INVALID_SOCKET) return false;

	struct sockaddr_in serveraddr {};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);

	if(bind(_listenSock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		Stop();
		return false;
	}

	if(listen(_listenSock, SOMAXCONN) == SOCKET_ERROR)
	{
		Stop();
		return false;
	}

	return true;
}

void TCPServer::Stop()
{
	if(_listenSock == INVALID_SOCKET) return;

	closesocket(_listenSock);
	_listenSock = INVALID_SOCKET;
}

bool TCPServer::IsRunning() const
{
	return _listenSock != INVALID_SOCKET;
}

SOCKET TCPServer::Accept(struct sockaddr_in *clientaddr, socklen_t *addrlen)
{
	if(!IsRunning()) return INVALID_SOCKET;
	return accept(_listenSock, (struct sockaddr *)clientaddr, addrlen);
}

int TCPServer::Send(SOCKET clientSock, const char *buf, int len)
{
	if(clientSock == INVALID_SOCKET) return SOCKET_ERROR;
	return send(clientSock, buf, len, 0);
}

int TCPServer::Recv(SOCKET clientSock, char *buf, int len)
{
	if(clientSock == INVALID_SOCKET) return SOCKET_ERROR;
	return recv(clientSock, buf, len, 0);
}

void TCPServer::CloseClient(SOCKET clientSock)
{
	if(clientSock == INVALID_SOCKET) return;
	closesocket(clientSock);
}
