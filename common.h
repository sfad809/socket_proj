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

// compatibilities
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#define closesocket(s) close(s)

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
