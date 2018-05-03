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

int g_TestP1;
int g_TestP2;

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
	PAINTSTRUCT ps;

    switch (message)
    {
	case WM_CREATE:
	{
		p1 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(50, 50, 50), CMonitorGraphUnit::BAR_SINGLE_VERT, 10, 10, 200, 200, 1);		// CPU
		p2 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(10, 40, 10), CMonitorGraphUnit::LINE_MULTI, 10, 220, 820, 500, 11);			// CCU
		p3 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(20, 20, 60), CMonitorGraphUnit::LINE_SINGLE, 220, 10, 200, 200, 1);			// NETWORK 
		p4 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(50, 50, 50), CMonitorGraphUnit::LINE_SINGLE, 430, 10, 400, 200, 1);			// DB_QUERRY
		
		p1->SetDataColumnInfo(1, 1, L"1번 서버");		// 서버ID, 데이터ID, 서버이름

		p2->SetDataColumnInfo(1, 1, L"1번 서버 쿠엑");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(2, 1, L"2번 서버 으엑극");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(3, 1, L"3번 서버 크에엑엌");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(4, 1, L"4th Server qqqqqq");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(5, 1, L"5678901234567890");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(6, 1, L"6번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(7, 1, L"7번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(8, 1, L"8번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(9, 1, L"9번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(10, 1, L"10번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(11, 1, L"0123456789012345");		// 서버ID, 데이터ID, 서버이름

		p3->SetDataColumnInfo(1, 1, L"1번 서버");		// 서버ID, 데이터ID, 서버이름
		p4->SetDataColumnInfo(1, 1, L"1번 서버");		// 서버ID, 데이터ID, 서버이름

		SetTimer(hWnd, 1, 25, NULL);
		srand((unsigned int)time(NULL));
		break;
	}
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			g_TestP1 += 1;
			if (g_TestP1 > 100)
				g_TestP1 = 0;
			p1->InsertData(1, 1, g_TestP1);


			g_TestP2 += 5000/128;
			if (g_TestP2 > 5000)
				g_TestP2 = 0;
			p2->InsertData(1, 1, rand() % 100);			// 서버ID, 데이터ID, 데이터값
			p2->InsertData(2, 1, rand() % 100 + 500);
			p2->InsertData(3, 1, rand() % 100 + 1000);
			p2->InsertData(4, 1, rand() % 100 + 1500);
			p2->InsertData(5, 1, rand() % 100 + 2000);
			p2->InsertData(6, 1, rand() % 100 + 2500);
			p2->InsertData(7, 1, rand() % 100 + 3000);
			p2->InsertData(8, 1, rand() % 100 + 3500);
			p2->InsertData(9, 1, rand() % 100 + 4000);
			p2->InsertData(10, 1, rand() % 100 + 4500);
			p2->InsertData(11, 1, g_TestP2);

			p3->InsertData(1, 1, rand() % 100);
			p4->InsertData(1, 1, rand() % 100);
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
