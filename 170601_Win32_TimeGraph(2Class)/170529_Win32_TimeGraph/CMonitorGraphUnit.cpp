#include "stdafx.h"
#include "CMonitorGraphUnit.h"
CMonitorGraphUnit::stHWNDtoTHIS	CMonitorGraphUnit::_HWND_This;
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
extern WNDCLASSEXW wcex;

int g_iWindowClassNumbering;

CMonitorGraphUnit::CMonitorGraphUnit(HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight) :_Data(128)
{
	PutThis();	//  hWnd-this ���̺� �� �ڽ� ������ Ŭ������ this ����
	
	_hInstance = hInstance;
	_hWndParent = hWndParent;
	_enGraphType = enType;
	_iWindowPosX = iPosX;
	_iWindowPosY = iPosY;
	_iWindowWidth = iWidth;
	_iWindowHeight = iHeight;
	_BackColor = BackColor;
	_enGraphType = enType;

	WCHAR szTitle[32];
	int iDataMax;
	int iDataAlert;
	switch (g_iWindowClassNumbering)
	{
	case 0:
		wcscpy_s(_szWindowClass, L"CPU");
		wcscpy_s(szTitle, L"CPU");
		iDataMax = 100;
		iDataAlert = 50;
		break;
	case 1:
		wcscpy_s(_szWindowClass, L"CCU");
		wcscpy_s(szTitle, L"����������");
		iDataMax = 5000;
		iDataAlert = NULL;
		break;
	case 2:
		wcscpy_s(_szWindowClass, L"NETWORK");	
		wcscpy_s(szTitle, L"��Ʈ��ũ");
		iDataMax = 500;
		iDataAlert = NULL;
		break;
	case 3:
		wcscpy_s(_szWindowClass, L" DB_QUERRY");
		wcscpy_s(szTitle, L"DB ����");
		iDataMax = 800;
		iDataAlert = NULL;
		break;
	default:
		break;
	}
	g_iWindowClassNumbering++;
	SetInformation(szTitle, iDataMax, iDataAlert);


	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_HELP);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszClassName = _szWindowClass;
	RegisterClassExW(&wcex);
	CreateWindow(_szWindowClass, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, _iWindowPosX, _iWindowPosY, _iWindowWidth, _iWindowHeight, _hWndParent, NULL, hInstance, NULL);      
	if (_hWnd == NULL)
	{
		DWORD err = GetLastError();
	}
	//   CreateWindow ��Ŀ���� - WndProc�� ���� WM_CREATE�� ���������� �����ؾ� NULL�� ��ȯ���� �ʰ� hWnd ���� ��ȯ��. 
	//  ��, ��ܺο��� PutThis�� this�� �����ϰ� CreateWindow�� ȣ���Ͽ� static WndProc�� �� ���� WndProc�� ù��° ������ hWnd���� ���̺� �ٽ� ����־� Create�� ���������� ������ �ٽ� ����.
	PutThis(); //  hWnd-this ���̺� �� �ڽ� ������ Ŭ������ hWnd ����

	//  * ���� ���Ŀ� �ڽ� �����츦 �����ϱ� ���� this�� hwnd�� ���� �����ϴ� ����

	HDC hdc = GetDC(_hWnd);
	GetClientRect(_hWnd, &_MemDCRect);
	_hMemBitmap = CreateCompatibleBitmap(hdc, _MemDCRect.right, _MemDCRect.bottom);
	_hMemDC = CreateCompatibleDC(hdc);
	_hMemBitmapOld = (HBITMAP)SelectObject(_hMemDC, _hMemBitmap);
	ReleaseDC(_hWnd, hdc);
}

CMonitorGraphUnit::~CMonitorGraphUnit()
{
}

void CMonitorGraphUnit::SetInformation(WCHAR * szTitle, int iDataMax, int iDataAlert)
{
	wcscpy_s(_szTitle , szTitle);
	_iDataMax = iDataMax;
	_iDataAlert = iDataAlert;
}

void CMonitorGraphUnit::SetLineColor(int iWeight, COLORREF BackColor)
{
}

LRESULT CMonitorGraphUnit::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CMonitorGraphUnit *pThis = CMonitorGraphUnit::GetThis(hWnd);
	if (pThis == NULL)	return 0;

	PAINTSTRUCT ps;
	HDC hdc;
	HDC hMemDC				= pThis->_hMemDC;
	RECT MemDCRect			= pThis->_MemDCRect;
	HPEN hLinePen			= pThis->_hLinePen;
	HPEN hOldPen;
	HBITMAP	hMemBitmap		= pThis->_hMemBitmap;
	HBITMAP	hMemBitmapOld	= pThis->_hMemBitmapOld;
	COLORREF BackColor		= pThis->_BackColor;
	CQueue<int> *Data		= &pThis->_Data;

	HBRUSH hBrush;
	HBRUSH hOldBrush;
	
	int originx;
	int originy;
	int destx;
	int desty;
	int iDataMax 			= pThis->_iDataMax;


	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hWnd);	
		GetClientRect(hWnd, &pThis->_MemDCRect);
		if (hMemBitmap == NULL)	
			hMemBitmap			= CreateCompatibleBitmap(hdc, pThis->_MemDCRect.right, pThis->_MemDCRect.bottom);
		pThis->_hMemDC			= CreateCompatibleDC(hdc);
		pThis->_hMemBitmapOld	= (HBITMAP)SelectObject(pThis->_hMemDC, pThis->_hMemBitmap);

		ReleaseDC(hWnd, hdc);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//////////////////////////////////////////////////////////////////////////////////////////////
		//	������ Ȯ���� ������ �� �ִ� ���� ��忡�� MM_ISOTROPIC�� MM_ANISOTROPIC �� ������ ������ 
		//  ������ ���� ��忡���� ������ Ȯ���� ������ �� ����.
		SetMapMode(hMemDC, MM_ISOTROPIC);
		SetWindowExtEx(hMemDC, MemDCRect.right, MemDCRect.bottom, NULL);
		GetClientRect(hWnd, &MemDCRect);
		SetViewportExtEx(hMemDC, MemDCRect.right, MemDCRect.bottom, NULL);
		//////////////////////////////////////////////////////////////////////////////////////////////

		pThis->Paint_Grid();
		hLinePen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
		hOldPen = (HPEN)SelectObject(hMemDC, hLinePen);
		originx = 0;
		originy = Data->GetFrontValue();
		destx = originx;
		desty = originy;
		Data->InitTemp();
		if (iDataMax < MemDCRect.bottom)
		{
			double number = (double)((MemDCRect.bottom - 30.0) / iDataMax);
			while (Data->MyPeek(&originy))
			{
				MoveToEx(hMemDC, destx * MemDCRect.right / 128, MemDCRect.bottom - desty * number, NULL);
				LineTo(hMemDC, originx * MemDCRect.right / 128, MemDCRect.bottom - originy  * number);
				destx = originx;
				desty = originy;
				originx++;
			}
		}
		else
		{
			double number = (double)(iDataMax / (MemDCRect.bottom - 30.0));
			while (Data->MyPeek(&originy))
			{
				MoveToEx(hMemDC, destx * MemDCRect.right / 128, MemDCRect.bottom - desty / number, NULL);
				LineTo(hMemDC, originx * MemDCRect.right / 128, MemDCRect.bottom  - originy / number);
				destx = originx;
				desty = originy;
				originx++;
			}
		}
		SelectObject(hMemDC, hOldPen);
		DeleteObject(hLinePen);
		pThis->Paint_Title();
		pThis->Paint_BarSingleHorz();
		pThis->FlipMemDC(hdc);
		pThis->ClearMemDC();

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:

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

BOOL CMonitorGraphUnit::InsertData(int iData)
{
	if (!_Data.Enqueue(iData))
	{
		_Data.Dequeue();
		return false;
	}
	InvalidateRect(_hWnd, NULL, FALSE);
	return true;
}

BOOL CMonitorGraphUnit::CreateGraphWindow(void)
{
	return 0;
}

void CMonitorGraphUnit::DestroyGraphWindow(void)
{
}

BOOL CMonitorGraphUnit::PutThis(void)
{
	for (int i = 0; i < dfMAXCHILD; ++i)
	{
		if (_HWND_This.pThis[i] == NULL)
		{
			_HWND_This.pThis[i] = this;
			return true;
		}
		else if (_HWND_This.pThis[i] == this)
		{
			_hWnd = _HWND_This.hWnd[i];
			return true;
		}
		
	}
	return false;
}

BOOL CMonitorGraphUnit::RemoveThis(HWND hWnd)
{
	return 0;
}

CMonitorGraphUnit * CMonitorGraphUnit::GetThis(HWND hWnd)
{
	for (int i = 0; i < dfMAXCHILD; ++i)
	{
		if (_HWND_This.hWnd[i] == NULL)
		{
			if (_HWND_This.pThis[i] != NULL)
			{
				_HWND_This.hWnd[i] = hWnd;
			}
			else
				break;
		}
		if (_HWND_This.hWnd[i] == hWnd)
		{
			return _HWND_This.pThis[i];
		}
	}
	return nullptr;
}

BOOL CMonitorGraphUnit::Alert(void)
{
	return 0;
}

void CMonitorGraphUnit::Paint_Title(void)
{

	HBRUSH hBrush = CreateSolidBrush(_BackColor * 4/5);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 4/5);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, 0, 0, _MemDCRect.right, 30); // NULLPEN���� �׸����� ���� ���� ������ ������ �׸����� �׵θ��� �׷����� �ʴ´�
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);

	HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	SetTextColor(_hMemDC, RGB(255, 255, 255));
	TextOut(_hMemDC, 5, 5, _szTitle, lstrlen(_szTitle));
	SetBkMode(_hMemDC, TRANSPARENT);
	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);

}

void CMonitorGraphUnit::Paint_Grid(void)
{
	HBRUSH hBrush = CreateSolidBrush(_BackColor);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, 0, 0, _MemDCRect.right, _MemDCRect.bottom); // NULLPEN���� �׸����� ���� ���� ������ ������ �׸����� �׵θ��� �׷����� �ʴ´�
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);
}

void CMonitorGraphUnit::Paint_BarSingleVert(void)
{
}

void CMonitorGraphUnit::Paint_BarSingleHorz(void)
{
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 21/10);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);

	SetROP2(_hMemDC, R2_XORPEN);
	MoveToEx(_hMemDC, 0, _MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / 4, NULL);
	LineTo(_hMemDC, _MemDCRect.right, _MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / 4);

	MoveToEx(_hMemDC, 0, _MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 2 / 4, NULL);
	LineTo(_hMemDC, _MemDCRect.right, _MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 2 / 4);

	MoveToEx(_hMemDC, 0, _MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 3 / 4, NULL);
	LineTo(_hMemDC, _MemDCRect.right, _MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 3 / 4);
	SetROP2(_hMemDC, R2_COPYPEN);

	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
}

void CMonitorGraphUnit::Paint_BarColumnVert(void)
{
}

void CMonitorGraphUnit::Paint_BarColumnHorz(void)
{
}

void CMonitorGraphUnit::Paint_LineSingle(void)
{
}

void CMonitorGraphUnit::Paint_LineMulti(void)
{
}

void CMonitorGraphUnit::Paint_Pie(void)
{
}

void CMonitorGraphUnit::ClearMemDC(void)
{
	PatBlt(_hMemDC, 0, 0, _MemDCRect.right, _MemDCRect.bottom, PATCOPY);
}

void CMonitorGraphUnit::FlipMemDC(HDC hdc)
{
	BitBlt(hdc, 0, 0, _MemDCRect.right, _MemDCRect.bottom, _hMemDC, 0, 0, SRCCOPY);
}
