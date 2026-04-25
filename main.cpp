#include "win32/Win32App.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <exception>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

HINSTANCE _hInstance;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_hInstance = hInstance;

	HWND hWnd;
	try
	{
		Win32App app("app", hInstance, WndProc, nCmdShow);
		app.Run();
	}
	catch(const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Initialize Error", MB_ICONERROR | MB_OK);
		return 1;
	}
	return 0;
}

HWND _hEdit;
void DisplayText(const char *fmt, ...)
{
	va_list arg;

	va_start(arg, fmt);
	char cbuf[1024];
	vsprintf(cbuf, fmt, arg);
	va_end(arg);

	int nLength = GetWindowTextLength(_hEdit);
	SendMessage(_hEdit, EM_SETSEL, nLength, nLength);
	SendMessageA(_hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR text = TEXT("메인 윈도우 작성");
	switch(msg)
	{
		case WM_CREATE:
		{
			_hEdit = CreateWindow(_T("edit"), NULL,
				WS_CHILD | WS_VISIBLE | WS_HSCROLL |
				WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
				ES_MULTILINE,
				0, 0, 0, 0, hWnd, (HMENU)100, _hInstance, NULL);
			DisplayText("Simple GUI Application Program.\r\n");
			DisplayText("Instance Handle: %s. \r\n", _hInstance);
		}
		return 0;
		case WM_SIZE:
			MoveWindow(_hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd,&ps);
			TextOut(hdc, 100, 100, text, lstrlen(text));
			EndPaint(hWnd, &ps);
		}
		return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}