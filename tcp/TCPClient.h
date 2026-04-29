#pragma once
#include "../common.h"

class TCPClient
{
public:
	TCPClient();
	~TCPClient();

	bool Connect(const char *ip, int port);
	void Disconnect();
	bool IsConnected() const;

	int Send(const char *buf, int len);
	int Recv(char *buf, int len);

private:
	SOCKET _sock;
};