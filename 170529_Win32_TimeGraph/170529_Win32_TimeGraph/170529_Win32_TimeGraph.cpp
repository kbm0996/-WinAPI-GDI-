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
CQueue<int> g_QData(100);
int g_iHeight;
int g_iStep;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY170529_WIN32_TIMEGRAPH, szWindowClass, MAX_LOADSTRING);

	if (!MyRegisterClass(hInstance))
	{
		DWORD err = GetLastError();
		return FALSE;
	}
	if (!InitInstance(hInstance, nCmdShow))
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
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
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY170529_WIN32_TIMEGRAPH);
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	HPEN hPen;
	HPEN hOldPen;

	int originx;
	int originy;
	int destx;
	int desty;

    switch (message)
    {
	case WM_CREATE: // #1 윈도우 메시지를 활용한 방법 
		SetTimer(hWnd, 1, 1, NULL);	// 세번째 인자는 밀리세컨드 단위
		SetTimer(hWnd, 2, 100, NULL);
		SetTimer(hWnd, 3, 500, NULL);
		SetTimer(hWnd, 4, 900, NULL);
		g_iHeight = 500;
		g_iStep = -1;
		srand(time(NULL));
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, TRUE);
		switch (wParam)
		{
		case 1:
		{
			if (!g_QData.Enqueue(rand() % 20 + g_iHeight))
				g_QData.Dequeue();
			break;
		}
		case 2:
				g_iHeight += g_iStep * rand() % 10;
			break;
		case 3:
			if (g_iHeight < 500)
				g_iHeight += rand() % 50;
			if (g_iHeight > 300)
				g_iHeight -= rand() % 50;
			break;
		case 4:
			if (g_iHeight < 300)
				g_iStep *= -1;
			if (g_iHeight > 600)
				g_iStep *= -1;
			break;
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
		hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		hOldPen = (HPEN)SelectObject(hdc, hPen);
		originx = 0;
		originy = g_QData.GetFrontValue();
		destx = originx;
		desty = originy;

		g_QData.InitTemp();
		while (g_QData.MyPeek(&originy))
		{
			MoveToEx(hdc, destx * 10, desty, NULL);
			LineTo(hdc, originx * 10, originy);
			destx = originx;
			desty = originy;
			originx++;
		}
		DeleteObject(SelectObject(hdc, hOldPen));

		EndPaint(hWnd, &ps);
		break;
	}
    case WM_DESTROY:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		KillTimer(hWnd, 3);
		KillTimer(hWnd, 4);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
