#include <string.h>
#include "common.h"

#define MAIN_BUTTON_ID      100
#define IP_EDIT_ID          200
#define PORT_EDIT_ID        201
#define CONNECT_BUTTON_ID   202
#define LOG_EDIT_ID         203
#define MESSAGE_EDIT_ID     204
#define SEND_BUTTON_ID      205

#define IP_LABEL_ID         300
#define PORT_LABEL_ID       301

HINSTANCE _hInstance;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ClientWndProc(HWND, UINT, WPARAM, LPARAM);

HWND _hMainButton;
HWND _hClientWnd;
HWND _hIpEdit;
HWND _hPortEdit;
HWND _hConnectButton;
HWND _hLogEdit;
HWND _hMessageEdit;
HWND _hSendButton;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	_hInstance = hInstance;

	WNDCLASSA mainClass {};
	mainClass.style = CS_HREDRAW | CS_VREDRAW;
	mainClass.lpfnWndProc = WndProc;
	mainClass.hInstance = hInstance;
	mainClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	mainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	mainClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	mainClass.lpszClassName = "tcp_main_window";
	if(!RegisterClassA(&mainClass))
	{
		MessageBoxA(NULL, "RegisterClassA(tcp_main_window) failed", "Initialize Error", MB_ICONERROR | MB_OK);
		return 1;
	}

	WNDCLASSA childClass {};
	childClass.style = CS_HREDRAW | CS_VREDRAW;
	childClass.lpfnWndProc = ClientWndProc;
	childClass.hInstance = hInstance;
	childClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	childClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	childClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	childClass.lpszClassName = "tcp_client_child";
	if(!RegisterClassA(&childClass))
	{
		MessageBoxA(NULL, "RegisterClassA(tcp_client_child) failed", "Initialize Error", MB_ICONERROR | MB_OK);
		return 1;
	}

	HWND hWnd = CreateWindowA("tcp_main_window", "TCP Client Launcher",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 700,
		NULL, NULL, hInstance, NULL);
	if(!hWnd)
	{
		MessageBoxA(NULL, "CreateWindowA(tcp_main_window) failed", "Initialize Error", MB_ICONERROR | MB_OK);
		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

void AppendLog(const char *fmt, ...)
{
	if(!_hLogEdit) return;

	char text[2048];
	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(text, sizeof(text), _TRUNCATE, fmt, args);
	va_end(args);

	int len = GetWindowTextLengthA(_hLogEdit);
	SendMessageA(_hLogEdit, EM_SETSEL, len, len);
	SendMessageA(_hLogEdit, EM_REPLACESEL, FALSE, (LPARAM)text);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		_hMainButton = CreateWindowA("button", "Open TCP Client",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hWnd, (HMENU)MAIN_BUTTON_ID, _hInstance, NULL);
	return 0;
	case WM_SIZE:
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		int buttonW = 180;
		int buttonH = 40;
		MoveWindow(_hMainButton, (width - buttonW) / 2, (height - buttonH) / 2,
			buttonW, buttonH, TRUE);
	}
	return 0;
	case WM_COMMAND:
	{
		if(LOWORD(wParam) == MAIN_BUTTON_ID)
		{
			if(_hClientWnd)
			{
				ShowWindow(_hClientWnd, SW_SHOW);
				SetForegroundWindow(_hClientWnd);
			}
			else
			{
				_hClientWnd = CreateWindowA("tcp_client_child", "TCP Client",
					WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					CW_USEDEFAULT, CW_USEDEFAULT, 620, 520,
					hWnd, NULL, _hInstance, NULL);
			}
			return 0;
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
	return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

SOCKET _sock = INVALID_SOCKET;
LRESULT CALLBACK ClientWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
	{
		CreateWindowA("static", "IP Address",
			WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0, hWnd, (HMENU)IP_LABEL_ID, _hInstance, NULL);
		_hIpEdit = CreateWindowA("edit", "127.0.0.1",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
			0, 0, 0, 0, hWnd, (HMENU)IP_EDIT_ID, _hInstance, NULL);
		CreateWindowA("static", "Port",
			WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0, hWnd, (HMENU)PORT_LABEL_ID, _hInstance, NULL);
		_hPortEdit = CreateWindowA("edit", "9000",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
			0, 0, 0, 0, hWnd, (HMENU)PORT_EDIT_ID, _hInstance, NULL);
		_hConnectButton = CreateWindowA("button", "Connect",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hWnd, (HMENU)CONNECT_BUTTON_ID, _hInstance, NULL);
		_hLogEdit = CreateWindowA("edit", "",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
			ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
			0, 0, 0, 0, hWnd, (HMENU)LOG_EDIT_ID, _hInstance, NULL);
		_hMessageEdit = CreateWindowA("edit", "",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
			0, 0, 0, 0, hWnd, (HMENU)MESSAGE_EDIT_ID, _hInstance, NULL);
		_hSendButton = CreateWindowA("button", "Send",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hWnd, (HMENU)SEND_BUTTON_ID, _hInstance, NULL);

		AppendLog("TCP client window ready\r\n");
	}
	return 0;
	case WM_SIZE:
	{
		const int margin = 12;
		const int labelW = 80;
		const int editH = 24;
		const int buttonW = 86;
		const int gap = 8;

		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		int y = margin;

		MoveWindow(GetDlgItem(hWnd, IP_LABEL_ID), margin, y + 4, labelW, 20, TRUE);
		MoveWindow(_hIpEdit, margin + labelW, y, 180, editH, TRUE);
		MoveWindow(GetDlgItem(hWnd, PORT_LABEL_ID), margin + labelW + 180 + gap, y + 4, 42, 20, TRUE);
		MoveWindow(_hPortEdit, margin + labelW + 180 + gap + 42, y, 70, editH, TRUE);
		MoveWindow(_hConnectButton, width - margin - buttonW, y - 1, buttonW, editH + 2, TRUE);

		y += editH + margin;
		int logH = height - y - editH - margin * 3;
		if(logH < 100) logH = 100;
		MoveWindow(_hLogEdit, margin, y, width - margin * 2, logH, TRUE);

		y += logH + margin;
		MoveWindow(_hMessageEdit, margin, y, width - margin * 3 - buttonW, editH, TRUE);
		MoveWindow(_hSendButton, width - margin - buttonW, y - 1, buttonW, editH + 2, TRUE);
	}
	return 0;
	case WM_COMMAND:
	{
		switch(LOWORD(wParam))
		{
		case CONNECT_BUTTON_ID:
		{
			if(_sock != INVALID_SOCKET)
			{
				AppendLog("[TCP] already connected\r\n");
				return 0;
			}

			char ip[64];
			char portText[16];
			GetWindowTextA(_hIpEdit, ip, sizeof(ip));
			GetWindowTextA(_hPortEdit, portText, sizeof(portText));

			int port = atoi(portText);
			AppendLog("[TCP] connecting to %s:%d...\r\n", ip, port);
			if(!init_tcp_client(_sock, ip, port))
			{
				AppendLog("[TCP] connect failed\r\n");
				return 0;
			}

			EnableWindow(_hConnectButton, FALSE);
			AppendLog("[TCP] connected\r\n");
			return 0;
		}
		case SEND_BUTTON_ID:
		{
			if(_sock == INVALID_SOCKET)
			{
				AppendLog("[TCP] connect first\r\n");
				return 0;
			}

			char buf[512];
			GetWindowTextA(_hMessageEdit, buf, sizeof(buf));

			int len = (int)strlen(buf);
			if(len == 0) return 0;

			int retval = send(_sock, buf, len, 0);
			if(retval == SOCKET_ERROR)
			{
				AppendLog("[TCP] send failed\r\n");
				close_tcp_server(_sock, true);
				EnableWindow(_hConnectButton, TRUE);
				return 0;
			}

			AppendLog("[send] %s\r\n", buf);
			SetWindowTextA(_hMessageEdit, "");

			retval = recv(_sock, buf, len, 0);
			if(retval == SOCKET_ERROR)
			{
				AppendLog("[TCP] recv failed\r\n");
				close_tcp_server(_sock, true);
				EnableWindow(_hConnectButton, TRUE);
				return 0;
			}
			if(retval == 0)
			{
				AppendLog("[TCP] disconnected by server\r\n");
				close_tcp_server(_sock, true);
				EnableWindow(_hConnectButton, TRUE);
				return 0;
			}

			buf[retval] = '\0';
			AppendLog("[recv] %s\r\n", buf);
			return 0;
		}
		case MESSAGE_EDIT_ID:
			if(HIWORD(wParam) == EN_MAXTEXT) AppendLog("[input] message is too long\r\n");
			return 0;
		}
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
	return 0;
	case WM_DESTROY:
		close_tcp_server(_sock, true);
		_hClientWnd = NULL;
		_hIpEdit = NULL;
		_hPortEdit = NULL;
		_hConnectButton = NULL;
		_hLogEdit = NULL;
		_hMessageEdit = NULL;
		_hSendButton = NULL;
	return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
