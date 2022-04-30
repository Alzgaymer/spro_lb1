#include "headers.h"
#include "resource.h"


int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	//null terminates proc variables
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	WNDCLASSEX wcex;
	//fill window class
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	//register main window 
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);
		throw;
	}
	HWND hwnd;
	hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		_T("WinProject"),
		WS_OVERLAPPEDWINDOW,
		//centerized window
		MaxPixelsFSX / 2 - MaxPixelsFSX / 4, MaxPixelsFSY / 2 - MaxPixelsFSY / 4,
		MaxPixelsFSX / 2, MaxPixelsFSY / 2,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hwnd)
	{
		MessageBox(NULL,
			_T("Call to project::Create_Window failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);
		throw;
	}
	hwndHandle = hwnd;
	ShowWindow(hwnd,
		nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	switch (message)
	{
	case WM_COMMAND:

		switch (wParam)
		{
		case ID_CREATE_PROCESS:
			if (!ProcessOpened)
			{
				ProcessOpened = true;

				CreateProcess(
					_T("C:\\Windows\\System32\\notepad.exe"), NULL, NULL, NULL, TRUE, NULL, NULL, NULL,
					&si,
					&pi
				);
			}
			break;
		case ID_CLOSE_PROCESS:
			if (ProcessOpened)
			{
				ProcessOpened = false;
				TerminateProcess(pi.hProcess, 1);
			}
			else MessageBox(hWnd,
				_T("There is no process"),
				_T("Process menu warning"), MB_OK);
			break;
		case ID_EXIT_PROCESS:
			if (ProcessOpened)
			{
				TerminateProcess(pi.hProcess, 1);
				ProcessOpened = false;
			}
			ExitProcess(36);
			break;
		default:
			break;
		}
		break;
	case WM_CREATE:
		InitializeCriticalSection(&cs);
		
		CreateThread(0, 0, &RefreshTime, 0, 0, 0);
		CreateThread(0, 0, &TimeOut,hWnd, 0, 0);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &rt);
		break;
	case WM_DESTROY:
		isexit = true;
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
		PostQuitMessage(69);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

}

DWORD WINAPI TimeOut(CONST LPVOID lParam)
{
	using namespace std::chrono_literals;
	

	while (!isexit)
	{	
		//get window dc
		HDC WindowDC = GetDC((HWND)lParam);

		HDC TemporaryDC = CreateCompatibleDC(WindowDC);

		HBITMAP BitmapDC = CreateCompatibleBitmap(WindowDC, rt.right - rt.left, rt.bottom - rt.top);
		//take bitmap as a dc
		SelectObject(TemporaryDC, BitmapDC);
		
		//SelectObject(dc, GetStockObject(DC_PEN));
		//SetDCPenColor(dc, RGB(255, 255, 0));
		EnterCriticalSection(&cs);
		//								gitting wstr size by subtraction pointers
		TextOut(TemporaryDC, 0, 0, t.wstr.c_str(), t.wstr.end() - t.wstr.begin());
		//copy dc(bitmap) to hdc(our screen)
		BitBlt(WindowDC, 0, 0, rt.right - rt.left, rt.bottom - rt.top, TemporaryDC, 0, 0, SRCCOPY);
		LeaveCriticalSection(&cs);
		//release memory
		DeleteDC(TemporaryDC);
		DeleteObject(BitmapDC);
		
	}
	ExitThread(25);
}

DWORD WINAPI RefreshTime(CONST LPVOID)
{
	while (isexit == false)
	{
		

#pragma warning(disable : 4996)
		//C style define time
		const size_t buffersize = 14;
		char buffer[buffersize];
		time_t seconds = time(NULL);
		tm* timeinfo = localtime(&seconds);
		const char* format = "%Y %I:%M:%S";
		strftime(buffer, buffersize, format, timeinfo);
		std::wstring wstr2(buffersize, L'#');
		mbstowcs(&wstr2[0], buffer, buffersize);
		//entering critical section
		EnterCriticalSection(&cs);
		t.wstr = wstr2;

		LeaveCriticalSection(&cs);
	}

	ExitThread(26);
}