// 170529_Win32_TimeGraph.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "170529_Win32_TimeGraph.h"
#include "Windows.h"
#include "CQueue.h"
#include <cstdlib>
#include <ctime>

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
MSG msg;

HDC hMemDC;
HBITMAP hMemBitmap;
HBITMAP hMemBitmapOld;

CQueue<int> g_QData(100);
int g_iHeight;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

	// 1. ������ Ŭ���� ���� & ���
	MyRegisterClass(hInstance);

	// 2. ������ ���� & ��� 
	if (!InitInstance(hInstance, nCmdShow))
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	// 3. �޽��� ó��
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
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.
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

// 4. ������ ���ν���
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
		SetTimer(hWnd, 1, 1, NULL);	// ����° ���ڴ� ms ����
		srand(time(NULL));

		hdc = GetDC(hWnd);	// �ش� �����쿡 ���� ȭ�� dc ȹ��
		if(hMemBitmap == NULL)	hMemBitmap = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);	
		// ������ ȹ���� dc�� ȭ�� ũ�⸦ �̿��ؼ� �޸𸮻� ��Ʈ��(�۾� ����) ����
		hMemDC			= CreateCompatibleDC(hdc);	// ������ ȹ���� dc�� ȣȯ�Ǵ� �۾� ������ dc ����
		hMemBitmapOld	= (HBITMAP)SelectObject(hMemDC, hMemBitmap);	// �۾������� dc�� ��Ʈ���� ����

		// ������
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
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
		/////////////////////////////////////////////////////////////////
		SetMapMode(hMemDC, MM_ISOTROPIC);
		SetWindowExtEx(hMemDC, crt.right, crt.bottom, NULL);
		GetClientRect(hWnd, &crt);
		SetViewportExtEx(hMemDC, crt.right, crt.bottom, NULL);

		// �׷��� �׸���
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
