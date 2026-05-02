#pragma once
#include <stdio.h>
#include <stdlib.h>

#if defined(__unix__)
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>  // defined errno
#include <string.h> // char* strerror(int errnum)

#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // host-network, p-network
#include <netdb.h>      // IP, DNS

#include <pthread.h>

// compatibilities
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#define closesocket(s) close(s)
#define HANDLE pthread_t

inline void err_quit(const char *msg)
{
	char *msgbuf = strerror(errno);
	printf("[%s] %s\n", msg, msgbuf);
	exit(1);
}

inline void err_display(const char *msg)
{
	char *msgbuf = strerror(errno);
	printf("[%s] %s\n", msg, msgbuf);
}
#else
//#include <windows.h> >> include in winsock2
#include <tchar.h>
//	ANSI    : func...A(), char    == ""
//	UNICODE : func...W(), wchar_t == L""

#include <winsock2.h>
#include <ws2tcpip.h> // sockaddr, host-network, p-n

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// compatibilities
typedef int socklen_t;

inline void err_quit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL
	);
	MessageBoxA(NULL, (char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

inline void err_display(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL
	);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
#endif

inline void close_tcp_server(SOCKET &sock, bool release)
{
	if(sock == INVALID_SOCKET) return;
	
	closesocket(sock);
	sock = INVALID_SOCKET;

#if defined(_WIN32) || defined(_WIN64)
	if(release) WSACleanup();
#endif
}

inline bool init_tcp_server(SOCKET &sock, u_short port)
{
#if defined(_WIN32) || defined(_WIN64)
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) { err_display("socket()"); return false; }

	struct sockaddr_in serveraddr {};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);

	if(bind(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		err_display("bind()");
		close_tcp_server(sock, true);
		return false;
	}

	if(listen(sock, SOMAXCONN) == SOCKET_ERROR)
	{
		err_display("listen()");
		close_tcp_server(sock, true);
		return false;
	}

	return true;
}

inline bool init_tcp_client(SOCKET &sock, u_short port)
{
#if defined(_WIN32) || defined(_WIN64)
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) { err_display("socket()"); return false; }

	struct sockaddr_in serveraddr {};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);

	if(connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		err_display("connect()");
		close_tcp_server(sock, true);
		return false;
	}
	return true;
}