#pragma once

class Win32App
{
public:
	Win32App(const char *className, void *hInstance, void *WndProc, int nCmdShow);
	void Run();
	
	static void* GetHwnd();
};