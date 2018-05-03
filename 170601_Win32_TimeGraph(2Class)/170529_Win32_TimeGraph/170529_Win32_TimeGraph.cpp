// 170529_Win32_TimeGraph.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "170529_Win32_TimeGraph.h"
#include "Windows.h"
#include "CQueue.h"
#include "CMonitorGraphUnit.h"
#include <cstdlib>
#include <ctime>

#define MAX_LOADSTRING 100

HINSTANCE g_hInst;         
WNDCLASSEXW wcex;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
CMonitorGraphUnit *p1;
CMonitorGraphUnit *p2;
CMonitorGraphUnit *p3;
CMonitorGraphUnit *p4;


LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY170529_WIN32_TIMEGRAPH, szWindowClass, MAX_LOADSTRING);

	MSG msg;
	HWND hWnd;
	g_hInst = hInstance;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY170529_WIN32_TIMEGRAPH));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);


	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_MAXIMIZE,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT crt;
	PAINTSTRUCT ps;

    switch (message)
    {
	case WM_CREATE:
	{
		p1 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(100, 100, 100), CMonitorGraphUnit::LINE_SINGLE, 10, 10, 200, 200);
		p2 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(50, 50, 50), CMonitorGraphUnit::LINE_SINGLE, 220, 10, 200, 200);
		p3 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(40, 40, 120), CMonitorGraphUnit::LINE_SINGLE, 430, 10, 400, 200);
		p4 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(30, 30, 100), CMonitorGraphUnit::LINE_SINGLE, 10, 220, 300, 250);

		SetTimer(hWnd, 1, 25, NULL);
		srand((unsigned int)time(NULL));
		break;
	}
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			p1->InsertData(rand() % 100);
			p2->InsertData(rand() % 100);
			p3->InsertData(rand() % 100);
			p4->InsertData(rand() % 100);
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
    case WM_DESTROY:
		KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
