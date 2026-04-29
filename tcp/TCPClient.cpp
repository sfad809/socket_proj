#include "TCPClient.h"

TCPClient::TCPClient()
	: _sock(INVALID_SOCKET)
{
#if defined(_WIN32) || defined(_WIN64)
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

TCPClient::~TCPClient()
{
	Disconnect();
#if defined(_WIN32) || defined(_WIN64)
	WSACleanup();
#endif
}

bool TCPClient::Connect(const char *ip, int port)
{
	if(IsConnected()) return true;

	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(_sock == INVALID_SOCKET) return false;

	struct sockaddr_in serveraddr {};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons((unsigned short)port);
	if(inet_pton(AF_INET, ip, &serveraddr.sin_addr) != 1)
	{
		Disconnect();
		return false;
	}

	if(connect(_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}

	return true;
}

void TCPClient::Disconnect()
{
	if(_sock == INVALID_SOCKET) return;

	closesocket(_sock);
	_sock = INVALID_SOCKET;
}

bool TCPClient::IsConnected() const
{
	return _sock != INVALID_SOCKET;
}

int TCPClient::Send(const char *buf, int len)
{
	if(!IsConnected()) return SOCKET_ERROR;
	return send(_sock, buf, len, 0);
}

int TCPClient::Recv(char *buf, int len)
{
	if(!IsConnected()) return SOCKET_ERROR;
	return recv(_sock, buf, len, 0);
}