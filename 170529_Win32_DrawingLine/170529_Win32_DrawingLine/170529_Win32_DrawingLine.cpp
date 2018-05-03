#include "stdafx.h"
#include "Windowsx.h"
#include "170529_Win32_DrawingLine.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
struct stMousePos
{
	int x, y;
};
stMousePos g_MousePos;
UINT g_PenWidth, g_PenColor;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY170529_WIN32_DRAWINGLINE, szWindowClass, MAX_LOADSTRING);

	if (!MyRegisterClass(hInstance))
	{
		DWORD err = GetLastError();
		return FALSE;
	}
    if (!InitInstance (hInstance, nCmdShow))
    {
		DWORD err = GetLastError();
        return FALSE;
    }

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))	// 메시지 루프
		// GetMessage의 반환값은 int이며 WM_QUIT 메시지를 받으면 FALSE(0)을 반환한다. 그래서 while문의 조건문으로 들어가는 것이다.
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

    return (int) msg.wParam;
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY170529_WIN32_DRAWINGLINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY170529_WIN32_DRAWINGLINE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

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
	// 프로젝트가 커질수록 switch-case가 많아져 규모가 비대해짐
	// 필요한 변수들은 switch-case마다 선언하지 말고 함수 상단에 줄줄이 나열하는 것이 그나마 효율적임
	// hWnd  창, message 메시지, wParam 옵션, lParam 마우스 좌표
{
	PAINTSTRUCT ps;
	HDC hdc;	// DC(Divce Context) 윈도우 GDI는 모두 DC를 통해서 이루어진다. // soen.kr DC의 필요성
	int xpos = GET_X_LPARAM(lParam);	// lParam으로부터 마우스의 좌표를 가져오는 매크로 함수
	int ypos = GET_Y_LPARAM(lParam);

	switch (message)
	{
	case WM_CREATE:	// CreateWindow 시 딱 한번 호출되는 메시지이며 WndProc에서 사용할 변수들의 초기화를 수행한다. 이외의 다른 메시지들은 WinMain 내부의 메시지 루프에서 발생한다.
		break;
	case WM_COMMAND:	// 윈도우 API의 메시지들은 어느정도 기억해야한다. 하나만 기억해서 msdn(f1)을 참조해도 좋다.
	{
		int wmId = LOWORD(wParam);

		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			HPEN hPen = CreatePen(PS_SOLID, g_PenWidth, g_PenColor);
			hdc = GetDC(hWnd);
			SelectObject(hdc, hPen);	// DC에 펜 등록
			HPEN hOldPen = (HPEN)SelectObject(hdc, NULL);

			MoveToEx(hdc, g_MousePos.x, g_MousePos.y, NULL);
			LineTo(hdc, xpos, ypos);

			hPen = hOldPen;
			DeleteObject(hPen);	// 계속 쌓이기 때문에 사용 후에는 즉시 DeleteObject를 해줘야한다.
								// DeleteObject 할 필요도 없고 해서도 안되는 객체 GetStockObject(); // soen.kr 배경색 바꾸기 참조
			ReleaseDC(hWnd, hdc);	// 다른 프로세스에서도 DC를 사용해야하므로 일을 마치면 바로 풀어줘야한다.
			// DC는 저장 및 보관의 대상이 아니며 그떄그때 번거롭지만 GetDC, ReleaseDC를 해줘야한다.
			// 만일 보관하게 된다면 다른 윈도우가 작동하지 않는 등 예기치 못한 에러가 발생할 수 있다.
			// 윈도우 클라이언트라는 공간이 유저만 사용하는 것이 아니라 윈도우 OS 자체적으로도 사용하는 것이기 때문이다.
			g_MousePos.x = xpos;
			g_MousePos.y = ypos;
		}
		break;
	case WM_LBUTTONDOWN:
		g_MousePos.x = xpos;
		g_MousePos.y = ypos;
		break;
	case WM_RBUTTONDOWN:
		g_PenWidth = rand() % 5;
		g_PenColor = RGB(rand() % 256, rand() % 256, rand() % 256);
		break;
	case WM_PAINT:	// 창이 활성화됐을때, 다른 창에 가려졌다가 다시 나타났을때, 해당 창을 내렸다가 올렸을때 등의 액션 발생시 호출되는 메시지
					// WM_PAINT를 임의로 호출하고 싶을 떄 (랜더링을 임의로 하고 싶을때) InvalidateRect 함수를 사용하면 된다. 
					// 단, 이 함수를 WM_PAINT 내부에서 호출하면 무한 루프가 되어 메시지큐에 누적되어 나중에는 예기치 못한 에러가 발생할 수 있다.
		hdc = BeginPaint(hWnd, &ps);	// GetDC와 함께 DC를 획득할 수 있는 유이한 함수. 단, BeginPaint는 WM_PAINT에서만 사용 가능.
										// 윈도우가 그려졌음을 OS에 알리는 함수. 이를 제거하면 OS에게 정보를 전달하지 못해 전달될 때까지 WM_PAINT 메시지가 계속 발생된다.
		// 로직은 BeginPaint와 EndPaint 사이에서 이루어져야만 한다.
		EndPaint(hWnd, &ps);
		// 고정적인 형태가 정해져있음. 
		break;
	case WM_DESTROY:	// 윈도우 닫기 시 호출	
		PostQuitMessage(0);
		break;
	default:	// 우리가 처리하지 않은 메시지들을 윈도우에게 처리해달라고 요청
		return DefWindowProc(hWnd, message, wParam, lParam);
		// 윈도우를 단는다는 것은 프로세스의 종료가 아니다. 
		// 때문에, 이를 처리하지 않으면 창을 닫아도 프로세스가 계속 실행 중이며 이후로는 프로세스를 제어할 수 없다. - 작업관리자에서 프로세스를 직접 찾아서 종료해야만 한다.
	}
	return 0;
}