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

HWND g_hWnd;
HINSTANCE g_hInst;         
WNDCLASSEXW wcex;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HDC hMemDC;
HBITMAP hMemBitmap;
HBITMAP hMemBitmapOld;

CMonitorGraphUnit *p1;
CMonitorGraphUnit *p2;
CMonitorGraphUnit *p3;
CMonitorGraphUnit *p4;
CMonitorGraphUnit *p5;
CMonitorGraphUnit *p6;

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


	g_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1920, 1080, nullptr, nullptr, hInstance, nullptr);
	if (!g_hWnd)
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

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
	RECT crt;

	/*HPEN hPen;
	HPEN hOldPen;
	HBRUSH hBrush;
	HBRUSH hOldBrush;*/

	GetClientRect(hWnd, &crt);

    switch (message)
    {
	case WM_CREATE:
	{
		p1 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(5, 20, 5), CMonitorGraphUnit::BAR_SINGLE_VERT, 10, 10, 200, 200, 1);		// CPU
		p2 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(5, 20, 5), CMonitorGraphUnit::LINE_MULTI, 10, 220, 820, 500, 8);			// CCU
		p3 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(5, 20, 5), CMonitorGraphUnit::LINE_SINGLE, 220, 10, 200, 200, 1);			// NETWORK 
		p4 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(5, 20, 5), CMonitorGraphUnit::LINE_SINGLE, 430, 10, 400, 200, 1);			// MEMORY
		p5 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(5, 20, 5), CMonitorGraphUnit::BAR_COLUMN_VERT, 1250, 10, 200, 200, 2);	// DB_QUERRY
		p6 = new CMonitorGraphUnit(g_hInst, hWnd, RGB(5, 20, 5), CMonitorGraphUnit::BAR_COLUMN_VERT, 840, 10, 400, 200, 4);		// PROCESS

		p1->SetDataColumnInfo(1, 1, L"1번 서버");		// 서버ID, 데이터ID, 서버이름

		p2->SetDataColumnInfo(1, 1, L"1번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(2, 1, L"2번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(3, 1, L"3번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(4, 1, L"4th Serv");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(5, 1, L"5678901");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(6, 1, L"6번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(7, 1, L"7번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(8, 1, L"8번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(9, 1, L"9번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(10, 1, L"10번 서버");		// 서버ID, 데이터ID, 서버이름
		p2->SetDataColumnInfo(11, 1, L"0123456");		// 서버ID, 데이터ID, 서버이름

		p3->SetDataColumnInfo(1, 1, L"1번 서버");		// 서버ID, 데이터ID, 서버이름

		p4->SetDataColumnInfo(1, 1, L"1번 서버");		// 서버ID, 데이터ID, 서버이름

		p5->SetDataColumnInfo(1, 1, L"1번 서버");		// 서버ID, 데이터ID, 서버이름
		p5->SetDataColumnInfo(2, 1, L"2번 서버");		// 서버ID, 데이터ID, 서버이름

		p6->SetDataColumnInfo(1, 1, L"1번 서버");		// 서버ID, 데이터ID, 서버이름
		p6->SetDataColumnInfo(2, 1, L"2번 서버");		// 서버ID, 데이터ID, 서버이름
		p6->SetDataColumnInfo(3, 1, L"3번 서버");		// 서버ID, 데이터ID, 서버이름
		p6->SetDataColumnInfo(4, 1, L"4th Serv");		// 서버ID, 데이터ID, 서버이름


		SetTimer(hWnd, 1, 1000, NULL);
		SetTimer(hWnd, 2, 500, NULL);

		/*hdc = GetDC(hWnd);
		if (hMemBitmap == NULL)	hMemBitmap = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
		hMemDC = CreateCompatibleDC(hdc);	
		hMemBitmapOld = (HBITMAP)SelectObject(hMemDC, hMemBitmap);	
		ReleaseDC(hWnd, hdc);*/

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

			g_TestP2 += 39;
			if (g_TestP2 > 5000)	g_TestP2 = 0;
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

			p5->InsertData(1, 1, rand() % 500);
			p5->InsertData(2, 1, rand() % 1000);

			if (g_TestP1 > 100)	g_TestP1 = 0;
			p6->InsertData(1, 1, rand () % 30);
			p6->InsertData(2, 1, rand () % 50);
			p6->InsertData(3, 1, rand () % 100);
			p6->InsertData(4, 1, g_TestP1);
			break;
		case 2:
			break;
		}
		break;
	case UM_ALERT:
		/*hPen = CreatePen(PS_SOLID, 1, 0x0000ff);
		hOldPen = (HPEN)SelectObject(hMemDC, hPen);
		hBrush = CreateSolidBrush(0x0000ff);
		hOldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

		Rectangle(hMemDC, 0, 0, crt.right, crt.bottom);

		SelectObject(hMemDC, hOldPen);
		DeleteObject(hPen);
		SelectObject(hMemDC, hOldBrush);
		DeleteObject(hBrush);
		InvalidateRect(hWnd, &crt, TRUE);*/
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		SetMapMode(hMemDC, MM_ANISOTROPIC);
		SetWindowExtEx(hMemDC, crt.right, crt.bottom, NULL);
		GetClientRect(hWnd, &crt);
		SetViewportExtEx(hMemDC, crt.right, crt.bottom, NULL);
	
		/*BitBlt(hdc, 0, 0, crt.right, crt.bottom, hMemDC, 0, 0, SRCCOPY);
		PatBlt(hMemDC, 0, 0, crt.right, crt.bottom, PATCOPY);*/

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
