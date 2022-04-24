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
		RECT rt;
		GetClientRect(hWnd, &rt);
		for (size_t y = 0, i = 0; y < rt.bottom && i < arr.size(); y += 20, i++)
		{
			arr[i] = CreateWindow(_T("static"),
				t.wstr.c_str(),
				WS_CHILD | WS_VISIBLE,
				0, y,
				110, 20,
				hWnd, 0, 0, 0);
		}
		InitializeCriticalSection(&cs);

		CreateThread(0, 0, &RefreshTime, 0, 0, 0);
		CreateThread(0, 0, &TimeOut, 0, 0, 0);
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
	while (isexit == false) {
		EnterCriticalSection(&cs);
		for (size_t i = 0; i < arr.size(); i++)
		{
			SetWindowText(arr[i], t.wstr.c_str());
		}
		LeaveCriticalSection(&cs);

		//std::this_thread::sleep_for(6ms);
	}
	ExitThread(25);
}

DWORD WINAPI RefreshTime(CONST LPVOID)
{
	while (isexit == false)
	{
		EnterCriticalSection(&cs);

#pragma warning(disable : 4996)
		//C style define time
		char buffer[64];
		time_t seconds = time(NULL);
		tm* timeinfo = localtime(&seconds);
		const char* format = "%Y %I:%M:%S";
		strftime(buffer, 80, format, timeinfo);
		std::wstring wstr2(80, L'#');
		mbstowcs(&wstr2[0], buffer, 80);

		t.wstr = wstr2;

		LeaveCriticalSection(&cs);
	}

	ExitThread(26);
}