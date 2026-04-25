#include "Win32App.h"
#include <stdexcept>
#include <windows.h>
#include <tchar.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

HWND _hWnd;

void* Win32App::GetHwnd() { return _hWnd; }

Win32App::Win32App(const char *className, void *hInstance, void *WndProc, int nCmdShow)
{
	HINSTANCE _hInstance = static_cast<HINSTANCE>(hInstance);
	WNDPROC _wndProc = static_cast<WNDPROC>(WndProc);

	WNDCLASSA wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = _wndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = _hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_WAIT);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = className;
	if(!RegisterClassA(&wndclass)) throw std::runtime_error("RegisterClass()");
	
	_hWnd = CreateWindowA(className, className,
		WS_OVERLAPPEDWINDOW, 0, 0, 600, 700, NULL, NULL, _hInstance, NULL);
	if(!_hWnd) throw std::runtime_error("CreateWindow()");

	ShowWindow(_hWnd, nCmdShow);
	UpdateWindow(_hWnd);
}

void Win32App::Run()
{
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}