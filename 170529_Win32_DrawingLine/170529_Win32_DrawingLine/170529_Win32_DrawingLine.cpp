#include "stdafx.h"
#include "Windowsx.h"
#include "170529_Win32_DrawingLine.h"

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
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

    while (GetMessage(&msg, nullptr, 0, 0))	// �޽��� ����
		// GetMessage�� ��ȯ���� int�̸� WM_QUIT �޽����� ������ FALSE(0)�� ��ȯ�Ѵ�. �׷��� while���� ���ǹ����� ���� ���̴�.
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
	// ������Ʈ�� Ŀ������ switch-case�� ������ �Ը� �������
	// �ʿ��� �������� switch-case���� �������� ���� �Լ� ��ܿ� ������ �����ϴ� ���� �׳��� ȿ������
	// hWnd  â, message �޽���, wParam �ɼ�, lParam ���콺 ��ǥ
{
	PAINTSTRUCT ps;
	HDC hdc;	// DC(Divce Context) ������ GDI�� ��� DC�� ���ؼ� �̷������. // soen.kr DC�� �ʿ伺
	int xpos = GET_X_LPARAM(lParam);	// lParam���κ��� ���콺�� ��ǥ�� �������� ��ũ�� �Լ�
	int ypos = GET_Y_LPARAM(lParam);

	switch (message)
	{
	case WM_CREATE:	// CreateWindow �� �� �ѹ� ȣ��Ǵ� �޽����̸� WndProc���� ����� �������� �ʱ�ȭ�� �����Ѵ�. �̿��� �ٸ� �޽������� WinMain ������ �޽��� �������� �߻��Ѵ�.
		break;
	case WM_COMMAND:	// ������ API�� �޽������� ������� ����ؾ��Ѵ�. �ϳ��� ����ؼ� msdn(f1)�� �����ص� ����.
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
			SelectObject(hdc, hPen);	// DC�� �� ���
			HPEN hOldPen = (HPEN)SelectObject(hdc, NULL);

			MoveToEx(hdc, g_MousePos.x, g_MousePos.y, NULL);
			LineTo(hdc, xpos, ypos);

			hPen = hOldPen;
			DeleteObject(hPen);	// ��� ���̱� ������ ��� �Ŀ��� ��� DeleteObject�� ������Ѵ�.
								// DeleteObject �� �ʿ䵵 ���� �ؼ��� �ȵǴ� ��ü GetStockObject(); // soen.kr ���� �ٲٱ� ����
			ReleaseDC(hWnd, hdc);	// �ٸ� ���μ��������� DC�� ����ؾ��ϹǷ� ���� ��ġ�� �ٷ� Ǯ������Ѵ�.
			// DC�� ���� �� ������ ����� �ƴϸ� �׋��׶� ���ŷ����� GetDC, ReleaseDC�� ������Ѵ�.
			// ���� �����ϰ� �ȴٸ� �ٸ� �����찡 �۵����� �ʴ� �� ����ġ ���� ������ �߻��� �� �ִ�.
			// ������ Ŭ���̾�Ʈ��� ������ ������ ����ϴ� ���� �ƴ϶� ������ OS ��ü�����ε� ����ϴ� ���̱� �����̴�.
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
	case WM_PAINT:	// â�� Ȱ��ȭ������, �ٸ� â�� �������ٰ� �ٽ� ��Ÿ������, �ش� â�� ���ȴٰ� �÷����� ���� �׼� �߻��� ȣ��Ǵ� �޽���
					// WM_PAINT�� ���Ƿ� ȣ���ϰ� ���� �� (�������� ���Ƿ� �ϰ� ������) InvalidateRect �Լ��� ����ϸ� �ȴ�. 
					// ��, �� �Լ��� WM_PAINT ���ο��� ȣ���ϸ� ���� ������ �Ǿ� �޽���ť�� �����Ǿ� ���߿��� ����ġ ���� ������ �߻��� �� �ִ�.
		hdc = BeginPaint(hWnd, &ps);	// GetDC�� �Բ� DC�� ȹ���� �� �ִ� ������ �Լ�. ��, BeginPaint�� WM_PAINT������ ��� ����.
										// �����찡 �׷������� OS�� �˸��� �Լ�. �̸� �����ϸ� OS���� ������ �������� ���� ���޵� ������ WM_PAINT �޽����� ��� �߻��ȴ�.
		// ������ BeginPaint�� EndPaint ���̿��� �̷�����߸� �Ѵ�.
		EndPaint(hWnd, &ps);
		// �������� ���°� ����������. 
		break;
	case WM_DESTROY:	// ������ �ݱ� �� ȣ��	
		PostQuitMessage(0);
		break;
	default:	// �츮�� ó������ ���� �޽������� �����쿡�� ó���ش޶�� ��û
		return DefWindowProc(hWnd, message, wParam, lParam);
		// �����츦 �ܴ´ٴ� ���� ���μ����� ���ᰡ �ƴϴ�. 
		// ������, �̸� ó������ ������ â�� �ݾƵ� ���μ����� ��� ���� ���̸� ���ķδ� ���μ����� ������ �� ����. - �۾������ڿ��� ���μ����� ���� ã�Ƽ� �����ؾ߸� �Ѵ�.
	}
	return 0;
}