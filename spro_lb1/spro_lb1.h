#pragma once
#include "headers.h"
#pragma region global variables
static TCHAR szWindowClass[] = _T("Title");
static const auto MaxPixelsFSX = GetSystemMetrics(SM_CXFULLSCREEN);
static const auto MaxPixelsFSY = GetSystemMetrics(SM_CYFULLSCREEN);

static STARTUPINFO si;
static PROCESS_INFORMATION pi;
static bool ProcessOpened = false;
static bool isexit = false;
static HWND hwndHandle;
using std::vector;
static CRITICAL_SECTION cs;
static RECT rt;
//static vector<HWND> arr{ 14 };
#pragma endregion

LRESULT CALLBACK WndProc(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);
DWORD WINAPI TimeOut(CONST LPVOID);
DWORD WINAPI RefreshTime(CONST LPVOID);

struct time {

	std::wstring wstr;

} t;