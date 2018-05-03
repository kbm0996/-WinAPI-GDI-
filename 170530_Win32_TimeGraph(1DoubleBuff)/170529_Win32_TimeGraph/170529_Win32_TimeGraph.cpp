// 170529_Win32_TimeGraph.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "170529_Win32_TimeGraph.h"
#include "Windows.h"
#include "CQueue.h"
#include <cstdlib>
#include <ctime>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
MSG msg;

HDC hMemDC;
HBITMAP hMemBitmap;
HBITMAP hMemBitmapOld;

CQueue<int> g_QData(100);
int g_iHeight;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY170529_WIN32_TIMEGRAPH, szWindowClass, MAX_LOADSTRING);

	// 1. 윈도우 클래스 세팅 & 등록
	MyRegisterClass(hInstance);

	// 2. 윈도우 생성 & 출력 
	if (!InitInstance(hInstance, nCmdShow))
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	// 3. 메시지 처리
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY170529_WIN32_TIMEGRAPH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;/*MAKEINTRESOURCEW(IDC_MY170529_WIN32_TIMEGRAPH);*/
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// 4. 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT crt;
	GetClientRect(hWnd, &crt);

	HPEN hPen;
	HPEN hOldPen;

	static int originx;
	static int originy;
	static int destx;
	static int desty;

    switch (message)
    {
	case WM_CREATE:
		SetTimer(hWnd, 1, 1, NULL);	// 세번째 인자는 ms 단위
		srand(time(NULL));

		hdc = GetDC(hWnd);	// 해당 윈도우에 대한 화면 dc 획득
		if(hMemBitmap == NULL)	hMemBitmap = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);	
		// 위에서 획득한 dc와 화면 크기를 이용해서 메모리상에 비트맵(작업 영역) 생성
		hMemDC			= CreateCompatibleDC(hdc);	// 위에서 획득한 dc와 호환되는 작업 영역의 dc 생성
		hMemBitmapOld	= (HBITMAP)SelectObject(hMemDC, hMemBitmap);	// 작업영역의 dc와 비트맵을 연결

		// 릴리즈
		ReleaseDC(hWnd, hdc);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			if (!g_QData.Enqueue(g_iHeight))
				g_QData.Dequeue();
			g_iHeight = crt.bottom - rand() % 100;
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
    case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
		/////////////////////////////////////////////////////////////////
		SetMapMode(hMemDC, MM_ISOTROPIC);
		SetWindowExtEx(hMemDC, crt.right, crt.bottom, NULL);
		GetClientRect(hWnd, &crt);
		SetViewportExtEx(hMemDC, crt.right, crt.bottom, NULL);

		// 그래프 그리기
		hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		hOldPen = (HPEN)SelectObject(hMemDC, hPen);
		originx = 0;
		originy = g_QData.GetFrontValue();
		destx = originx;
		desty = originy;

		g_QData.InitTemp();
		while (g_QData.MyPeek(&originy))
		{
			MoveToEx(hMemDC, destx*crt.right/100, desty, NULL);
			LineTo(hMemDC, originx*crt.right/100, originy);
			destx = originx;
			desty = originy;
			originx++;
		}
		DeleteObject(SelectObject(hMemDC, hOldPen));
		/////////////////////////////////////////////////////////////////
		GetClientRect(hWnd, &crt);
		BitBlt(hdc, 0, 0, crt.right, crt.bottom, hMemDC, 0, 0, SRCCOPY);
		PatBlt(hMemDC, 0, 0, crt.right, crt.bottom, PATCOPY);

		EndPaint(hWnd, &ps);
		break;
	}
    case WM_DESTROY:
		KillTimer(hWnd, 1);

		SelectObject(hMemDC, hMemBitmapOld);
		DeleteObject(hMemBitmap);
		DeleteDC(hMemDC);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
