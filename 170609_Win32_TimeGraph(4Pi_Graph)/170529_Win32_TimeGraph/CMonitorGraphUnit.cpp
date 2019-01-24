#include "stdafx.h"
#include "CMonitorGraphUnit.h"
#include <cmath>

#define SAFE_DELETE(p) if(p!=NULL){free(p);p=NULL;}
#define dfCOLUMN_NAME_LEN 32

CMonitorGraphUnit::stHWNDtoTHIS	CMonitorGraphUnit::_HWND_This;
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
extern WNDCLASSEXW wcex;
extern HWND g_hWnd;

int g_iWindowClassNumbering;

CMonitorGraphUnit::CMonitorGraphUnit(HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight, int iColumnCnt) 
{
	WCHAR szTitle[32];
	int iDataMax;
	int iDataAlert;
	WCHAR szUnit[32];

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
		

	if ((_enGraphType == BAR_SINGLE_VERT) | (_enGraphType == BAR_SINGLE_HORZ) |  (_enGraphType == LINE_SINGLE))
	{
		_iColumnCnt = 1;
	}
	else
	{
		_iColumnCnt = iColumnCnt;
	}

	/* �׷��� �߰��Ҷ� �պ��� �� ��*/
	switch (g_iWindowClassNumbering)
	{
	case 0:
		wcscpy_s(_szWindowClass, L"CPU");
		wcscpy_s(szTitle, L"CPU");
		iDataMax = 100;
		iDataAlert = 50;
		wcscpy_s(szUnit, L"unit");
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(dfDATA_CNT);
		}
		break;
	case 1:
		wcscpy_s(_szWindowClass, L"CCU");
		wcscpy_s(szTitle, L"����������");
		iDataMax = 5000;
		iDataAlert = NULL;
		wcscpy_s(szUnit, L"unit");
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(dfDATA_CNT);
		}
		break;
	case 2:
		wcscpy_s(_szWindowClass, L"CCUrate");
		wcscpy_s(szTitle, L"���������ں���");
		iDataMax = 5000;
		iDataAlert = NULL;
		wcscpy_s(szUnit, L"unit");
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(dfDATA_CNT);
		}
		break;
	case 3:
		wcscpy_s(_szWindowClass, L"NETWORK");	
		wcscpy_s(szTitle, L"��Ʈ��ũ");
		iDataMax = 500;
		iDataAlert = NULL;
		wcscpy_s(szUnit, L"unit");
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(dfDATA_CNT);
		}
		break;
	case 4:
		wcscpy_s(_szWindowClass, L"MEMORY");
		wcscpy_s(szTitle, L"�޸�");
		iDataMax = 800;
		iDataAlert = NULL;
		wcscpy_s(szUnit, L"unit");
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(dfDATA_CNT);
		}
		break;
	case 5:
		wcscpy_s(_szWindowClass, L"DB_BUFFER");
		wcscpy_s(szTitle, L"DB ����");
		iDataMax = 1000;
		iDataAlert = NULL;
		wcscpy_s(szUnit, L"unit");
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(dfDATA_CNT);
		}
		break;
	case 6:
		wcscpy_s(_szWindowClass, L"PROCESS");
		wcscpy_s(szTitle, L"���μ���");
		iDataMax = 100;
		iDataAlert = NULL;
		wcscpy_s(szUnit, L"unit");
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(dfDATA_CNT);
		}
		break;
	default:
		break;
	}
	g_iWindowClassNumbering++;
	SetInformation(szTitle, iDataMax, iDataAlert, szUnit);

	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = _hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_HELP);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszClassName = _szWindowClass;
	RegisterClassExW(&wcex);
	CreateWindow(_szWindowClass, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, _iWindowPosX, _iWindowPosY, _iWindowWidth, _iWindowHeight, _hWndParent, NULL, hInstance, NULL);      
	if (_hWnd == NULL)
	{
		DWORD err = GetLastError();
	}

	PutThis();


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

void CMonitorGraphUnit::SetInformation(WCHAR * szTitle, int iDataMax, int iDataAlert, WCHAR * szUnit)
{
	wcscpy_s(_szTitle , szTitle);
	_iDataMax = iDataMax;
	_iDataMaxOrigin = iDataMax;
	_iDataAlert = iDataAlert;
	wcscpy_s(_szUnitName, szUnit);
}

void CMonitorGraphUnit::SetDataColumnInfo(ULONG64 u64ServerID, int iType, WCHAR * szName)
{
	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
		{
			_Data.u64ServerID[Cnt] = u64ServerID;
			wcscpy_s(_Data.szName[Cnt], szName);
			_Data.iType[Cnt] = iType;
			break;
		}
	}
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
	HBITMAP	hMemBitmap		= pThis->_hMemBitmap;
	HBITMAP	hMemBitmapOld	= pThis->_hMemBitmapOld;
	COLORREF BackColor		= pThis->_BackColor;

	switch (message)
	{
	case WM_CREATE:
		pThis->CreateGraphWindow();
		SetTimer(hWnd, 1, 500, NULL);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			if (pThis->_bAlertMode)
				SendMessage(g_hWnd, UM_ALERT, 0, 0);
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		SetMapMode(hMemDC, MM_ANISOTROPIC);
		SetWindowExtEx(hMemDC, MemDCRect.right, MemDCRect.bottom, NULL);
		GetClientRect(hWnd, &MemDCRect);
		SetViewportExtEx(hMemDC, MemDCRect.right, MemDCRect.bottom, NULL);
		
		switch (pThis->_enGraphType) 
		{
		case BAR_SINGLE_VERT:
			pThis->Paint_BarSingleVert();
			break;
		case BAR_SINGLE_HORZ:
			pThis->Paint_BarSingleHorz();
			break;
		case BAR_COLUMN_VERT:
			pThis->Paint_BarColumnVert();
			break;
		case BAR_COLUMN_HORZ:
			pThis->Paint_BarColumnHorz();
			break;
		case LINE_SINGLE:
			pThis->Paint_LineSingle();
			break;
		case LINE_MULTI:
			pThis->Paint_LineMulti();
			break;
		case PIE:
			pThis->Paint_Pie();
			break;
		}

		pThis->FlipMemDC(hdc);
		pThis->ClearMemDC();

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		pThis->DestroyGraphWindow();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CMonitorGraphUnit::InsertData(ULONG64 u64ServerID, int iType, int iData)
{
	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
			break;
		if (_Data.u64ServerID[Cnt] == u64ServerID)
		{
			if (_Data.iType[Cnt] == iType)
			{
				if (!_Data.Queue[Cnt]->Enqueue(iData))
				{
					_Data.Queue[Cnt]->Dequeue();
					_Data.Queue[Cnt]->Enqueue(iData);
				}
				InvalidateRect(_hWnd, NULL, FALSE);
			}
		}
	}
}

BOOL CMonitorGraphUnit::CreateGraphWindow(void)
{
	HDC hdc = GetDC(_hWnd);
	GetClientRect(_hWnd, &_MemDCRect);
	if (_hMemBitmap == NULL)
		_hMemBitmap = CreateCompatibleBitmap(hdc, _MemDCRect.right, _MemDCRect.bottom);
	_hMemDC = CreateCompatibleDC(hdc);
	_hMemBitmapOld = (HBITMAP)SelectObject(_hMemDC, _hMemBitmap);

	ReleaseDC(_hWnd, hdc);
	return true;
}

void CMonitorGraphUnit::DestroyGraphWindow(void)
{
	SelectObject(_hMemDC, _hMemBitmapOld);
	DeleteObject(_hMemBitmap);

	DeleteDC(_hMemDC);
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

BOOL CMonitorGraphUnit::Alert(int iValue)
{
	if (_iDataAlert != NULL && iValue >= _iDataAlert)
	{
		_bAlertMode = TRUE;
		return true;
	}
	_bAlertMode = FALSE;
	return false;
}

void CMonitorGraphUnit::Paint_Background(void)
{
	//------------------------------------------------------
	// ����
	//------------------------------------------------------
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

void CMonitorGraphUnit::Paint_Title(void)
{
	//------------------------------------------------------
	// Ÿ��Ʋ
	//------------------------------------------------------
	HBRUSH hBrush = CreateSolidBrush(_BackColor * 2);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 7/5);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, 0, 0, _MemDCRect.right, dfTITLE_HEIGHT); // NULLPEN���� �׸����� ���� ���� ������ ������ �׸����� �׵θ��� �׷����� �ʴ´�
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);


	SetTextAlign(_hMemDC, TA_LEFT);
	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	
	TextOut(_hMemDC, 5, 5, _szTitle, lstrlen(_szTitle));
	SetBkMode(_hMemDC, TRANSPARENT);
	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
}

void CMonitorGraphUnit::Paint_Grid(void)
{
	//------------------------------------------------------
	// �׸��� ����
	//------------------------------------------------------
	HPEN hLinePen = CreatePen(PS_SOLID, 1, (COLORREF)(_BackColor * 6));
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	SetBkMode(_hMemDC, TRANSPARENT);
	SetROP2(_hMemDC, R2_XORPEN);
	MoveToEx(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / dfGRID_VERT_NUM), NULL);
	LineTo(_hMemDC, _MemDCRect.right, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / dfGRID_VERT_NUM));

	MoveToEx(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 2 / dfGRID_VERT_NUM), NULL);
	LineTo(_hMemDC, _MemDCRect.right, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 2 / dfGRID_VERT_NUM));

	MoveToEx(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 3 / dfGRID_VERT_NUM), NULL);
	LineTo(_hMemDC, _MemDCRect.right, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 3 / dfGRID_VERT_NUM));
	SetROP2(_hMemDC, R2_COPYPEN);

	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);

	//------------------------------------------------------
	// �׸��� ��ġ
	//------------------------------------------------------
	HFONT hFont = CreateFont(dfFONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	WCHAR szLineNumber[10];

	_itow_s((_iDataMax * 1 / dfGRID_VERT_NUM), szLineNumber, 10);
	TextOut(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / 4), szLineNumber, lstrlen(szLineNumber));

	_itow_s((_iDataMax * 2 / dfGRID_VERT_NUM), szLineNumber, 10, 10);
	TextOut(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / 2), szLineNumber, lstrlen(szLineNumber));

	_itow_s((_iDataMax * 3 / dfGRID_VERT_NUM), szLineNumber, 10, 10);
	TextOut(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 3 / 4), szLineNumber, lstrlen(szLineNumber));

	_itow_s((_iDataMax), szLineNumber, 10, 10);
	TextOut(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0)), szLineNumber, lstrlen(szLineNumber));

	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
}

void CMonitorGraphUnit::Paint_Grid_BarColumnVert(void)
{
	//------------------------------------------------------
	// �׸��� ����
	//------------------------------------------------------
	HPEN hLinePen = CreatePen(PS_SOLID, 1, (COLORREF)(_BackColor * 6));
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	SetBkMode(_hMemDC, TRANSPARENT);
	SetROP2(_hMemDC, R2_XORPEN);
	MoveToEx(_hMemDC, 0, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0) * 1 / dfGRID_VERT_NUM), NULL);
	LineTo(_hMemDC, _MemDCRect.right, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0) * 1 / dfGRID_VERT_NUM));

	MoveToEx(_hMemDC, 0, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0) * 2 / dfGRID_VERT_NUM), NULL);
	LineTo(_hMemDC, _MemDCRect.right, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0) * 2 / dfGRID_VERT_NUM));

	MoveToEx(_hMemDC, 0, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0) * 3 / dfGRID_VERT_NUM), NULL);
	LineTo(_hMemDC, _MemDCRect.right, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0) * 3 / dfGRID_VERT_NUM));
	SetROP2(_hMemDC, R2_COPYPEN);

	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);

	//------------------------------------------------------
	// �׸��� ��ġ
	//------------------------------------------------------
	HFONT hFont = CreateFont(dfFONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	WCHAR szLineNumber[10];

	_itow_s((_iDataMax * 1 / dfGRID_VERT_NUM), szLineNumber, 10);
	TextOut(_hMemDC, 0, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0) * 1 / 4), szLineNumber, lstrlen(szLineNumber));

	_itow_s((_iDataMax * 2 / dfGRID_VERT_NUM), szLineNumber, 10, 10);
	TextOut(_hMemDC, 0, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0) * 1 / 2), szLineNumber, lstrlen(szLineNumber));

	_itow_s((_iDataMax * 3 / dfGRID_VERT_NUM), szLineNumber, 10, 10);
	TextOut(_hMemDC, 0, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0) * 3 / 4), szLineNumber, lstrlen(szLineNumber));

	_itow_s((_iDataMax), szLineNumber, 10, 10);
	TextOut(_hMemDC, 0, (int)((_MemDCRect.bottom - 20) - (_MemDCRect.bottom - 50.0)), szLineNumber, lstrlen(szLineNumber));

	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
}

void CMonitorGraphUnit::Paint_Index_LineMulti(void)
{
	//------------------------------------------------------
	// �ε��� ����
	//------------------------------------------------------
	HBRUSH hBrush = CreateSolidBrush(_BackColor * 7 /5);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 7 / 5);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, _MemDCRect.right - 155, dfTITLE_HEIGHT, _MemDCRect.right, _MemDCRect.bottom); // NULLPEN���� �׸����� ���� ���� ������ ������ �׸����� �׵θ��� �׷����� �ʴ´�
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);

	//------------------------------------------------------
	// �ε��� 
	//------------------------------------------------------
	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
			break;

		COLORREF RGB;
		switch (Cnt)
		{
		case 0:
			RGB = 0x0000ff;
			break;
		case 1:
			RGB = 0x00ffff;
			break;
		case 2:
			RGB = 0x00ff00;
			break;
		case 3:
			RGB = 0xffff00;
			break;
		case 4:
			RGB = 0xff0000;
			break;
		case 5:
			RGB = 0xff00ff;
			break;
		case 6:
			RGB = 0x00008c;
			break;
		case 7:
			RGB = 0x008c8c;
			break;
		case 8:
			RGB = 0x008c00;
			break;
		case 9:
			RGB = 0x8c8c00;
			break;
		case 10:
			RGB = 0x8c008c;
			break;
		case 11:
			RGB = 0xffff8c;
			break;
		case 12:
			RGB = 0xff8c8c;
			break;
		case 13:
			RGB = 0xff8cff;
			break;
		case 14:
			RGB = 0x8c8cff;
			break;
		case 15:
			RGB = 0x8cff8c;
			break;
		}
		HFONT hFont = CreateFont(dfFONT_INDEX_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
		HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
		SetTextColor(_hMemDC,RGB);
		TextOut(_hMemDC, _MemDCRect.right - 140, dfTITLE_HEIGHT + 10 + dfFONT_INDEX_SIZE * Cnt, L"��", 1);
		TextOut(_hMemDC, _MemDCRect.right - 120, dfTITLE_HEIGHT + 10 + dfFONT_INDEX_SIZE * Cnt, (LPCWSTR)_Data.szName[Cnt], lstrlen((LPCWSTR)_Data.szName[Cnt]));
		SelectObject(_hMemDC, hOldFont);
		DeleteObject(hFont);
	}
}

void CMonitorGraphUnit::Paint_Index_Pie(void)
{
	//------------------------------------------------------
	// �ε��� 
	//------------------------------------------------------
	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
			break;

		COLORREF RGB;
		HFONT hFont = CreateFont(dfFONT_INDEX_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
		HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
		SetTextColor(_hMemDC, 0);
		TextOut(_hMemDC, _MemDCRect.right - 139, dfTITLE_HEIGHT + 11 + dfFONT_INDEX_SIZE * Cnt, L"��", 1);
		TextOut(_hMemDC, _MemDCRect.right - 99, dfTITLE_HEIGHT + 11 + dfFONT_INDEX_SIZE * Cnt, (LPCWSTR)_Data.szName[Cnt], lstrlen((LPCWSTR)_Data.szName[Cnt]));

		switch (Cnt)
		{
		case 0:
			RGB = 0x0000ff;
			break;
		case 1:
			RGB = 0x00ffff;
			break;
		case 2:
			RGB = 0x00ff00;
			break;
		case 3:
			RGB = 0xffff00;
			break;
		case 4:
			RGB = 0xff0000;
			break;
		case 5:
			RGB = 0xff00ff;
			break;
		case 6:
			RGB = 0x00008c;
			break;
		case 7:
			RGB = 0x008c8c;
			break;
		case 8:
			RGB = 0x008c00;
			break;
		case 9:
			RGB = 0x8c8c00;
			break;
		case 10:
			RGB = 0x8c008c;
			break;
		case 11:
			RGB = 0xffff8c;
			break;
		case 12:
			RGB = 0xff8c8c;
			break;
		case 13:
			RGB = 0xff8cff;
			break;
		case 14:
			RGB = 0x8c8cff;
			break;
		case 15:
			RGB = 0x8cff8c;
			break;
		}

		SetTextColor(_hMemDC, RGB);
		TextOut(_hMemDC, _MemDCRect.right - 140, dfTITLE_HEIGHT + 10 + dfFONT_INDEX_SIZE * Cnt, L"��", 1);
		TextOut(_hMemDC, _MemDCRect.right - 100, dfTITLE_HEIGHT + 10 + dfFONT_INDEX_SIZE * Cnt, (LPCWSTR)_Data.szName[Cnt], lstrlen((LPCWSTR)_Data.szName[Cnt]));
		SelectObject(_hMemDC, hOldFont);
		DeleteObject(hFont);
	}
}

void CMonitorGraphUnit::Paint_Index_BarColumnVert(void)
{
	//------------------------------------------------------
	// �ε��� ����
	//------------------------------------------------------
	HBRUSH hBrush = CreateSolidBrush(_BackColor * 7 / 5);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 7 / 5);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, 0, _MemDCRect.bottom - 20, _MemDCRect.right, _MemDCRect.bottom); // NULLPEN���� �׸����� ���� ���� ������ ������ �׸����� �׵θ��� �׷����� �ʴ´�
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);

	//------------------------------------------------------
	// �ε��� 
	//------------------------------------------------------
	int iGraphWidth = (_MemDCRect.right - (dfBAR_REMAIN * 2) - (dfBAR_GAP * (_iColumnCnt - 1))) / _iColumnCnt;
	int iGraphEndX = dfBAR_REMAIN + iGraphWidth;
	int iGraphStartX = dfBAR_REMAIN;
	COLORREF RGB;

	SetTextAlign(_hMemDC, TA_CENTER);
	HFONT hFont = CreateFont(dfFONT_INDEX_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);


	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		switch (Cnt)
		{
		case 0:
			RGB = 0x0000ff;
			break;
		case 1:
			RGB = 0x00ffff;
			break;
		case 2:
			RGB = 0x00ff00;
			break;
		case 3:
			RGB = 0xffff00;
			break;
		case 4:
			RGB = 0xff0000;
			break;
		case 5:
			RGB = 0xff00ff;
			break;
		case 6:
			RGB = 0x00008c;
			break;
		case 7:
			RGB = 0x008c8c;
			break;
		case 8:
			RGB = 0x008c00;
			break;
		case 9:
			RGB = 0x8c8c00;
			break;
		case 10:
			RGB = 0x8c008c;
			break;
		case 11:
			RGB = 0xffff8c;
			break;
		case 12:
			RGB = 0xff8c8c;
			break;
		case 13:
			RGB = 0xff8cff;
			break;
		case 14:
			RGB = 0x8c8cff;
			break;
		case 15:
			RGB = 0x8cff8c;
			break;
		}
		SetTextColor(_hMemDC, RGB);

		if (_Data.iType[Cnt] == NULL)
			break;

		TextOut(_hMemDC, iGraphStartX + iGraphWidth / 2, (_MemDCRect.bottom) - dfFONT_TITLE_SIZE, (LPCWSTR)_Data.szName[Cnt], lstrlen((LPCWSTR)_Data.szName[Cnt]));
		
		iGraphStartX += iGraphWidth + dfBAR_GAP;
		iGraphEndX += iGraphWidth + dfBAR_GAP;
	}
	SetTextAlign(_hMemDC, TA_LEFT);

	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
}

void CMonitorGraphUnit::Paint_BarSingleVert(void)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, _BackColor);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hPen);
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 150, 0));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);

	double height;
	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	WCHAR szValue[15];
	int iValue;

	_Data.Queue[0]->MyPeek(&iValue);
	
	if (!Alert(iValue))
		SetTextColor(_hMemDC, 0xffffff);
	else 
		SetTextColor(_hMemDC, 0xff);

	Paint_Background();

	_itow_s(iValue, szValue, 10);
	SetTextAlign(_hMemDC, TA_CENTER);
	if (_iDataMax < _MemDCRect.bottom)
	{
		height = (double)((_MemDCRect.bottom - 30.0) / _iDataMax);
		Rectangle(_hMemDC, _MemDCRect.right / 5, (int)(_MemDCRect.bottom - iValue * height), _MemDCRect.right * 4 / 5, _MemDCRect.bottom);
		if (iValue * height <= dfFONT_TITLE_SIZE)
			TextOut(_hMemDC, _MemDCRect.right / 2, _MemDCRect.bottom - dfFONT_TITLE_SIZE, szValue, lstrlen(szValue));
		else
			TextOut(_hMemDC, _MemDCRect.right / 2, (int)(_MemDCRect.bottom - iValue * height), szValue, lstrlen(szValue));
	}
	else
	{
		height = (double)(_iDataMax / (_MemDCRect.bottom - 30.0));
		Rectangle(_hMemDC, _MemDCRect.right / 5, (int)(_MemDCRect.bottom - iValue / height), _MemDCRect.right * 4 / 5, _MemDCRect.bottom);
		if (iValue / height <= dfFONT_TITLE_SIZE)
			TextOut(_hMemDC, _MemDCRect.right / 2, _MemDCRect.bottom - dfFONT_TITLE_SIZE, szValue, lstrlen(szValue));
		else
			TextOut(_hMemDC, _MemDCRect.right / 2, (int)(_MemDCRect.bottom - iValue / height), szValue, lstrlen(szValue));
	}

	SetTextAlign(_hMemDC, TA_LEFT);

	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hPen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);

	ControlDataMax();	// ��Ȳ�� ���� iDataMax�� ����

	Paint_Title();
	Paint_Grid();
}

void CMonitorGraphUnit::Paint_BarSingleHorz(void)
{
	Paint_Background();


	Paint_Title();
	Paint_Grid();
}

void CMonitorGraphUnit::Paint_BarColumnVert(void)
{
	int iGraphWidth = (_MemDCRect.right - (dfBAR_REMAIN * 2) - (dfBAR_GAP * (_iColumnCnt - 1))) / _iColumnCnt;
	int iGraphEndX = dfBAR_REMAIN + iGraphWidth;
	int iGraphStartX = dfBAR_REMAIN;
	double height;
	int iValue;

	HPEN hPen = CreatePen(PS_SOLID, 1, _BackColor);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hPen);
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 150, 0));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);

	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	WCHAR szValue[32];

	Paint_Background();

	SetTextColor(_hMemDC, 0xffffff); //settextcolor
	SetTextAlign(_hMemDC, TA_CENTER);

	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
			break;
		_Data.Queue[Cnt]->MyPeek(&iValue);
		_itow_s(iValue, szValue, 10);
		if (_iDataMax < _MemDCRect.bottom)
		{
			height = (double)((_MemDCRect.bottom - 50.0) / _iDataMax);
			Rectangle(_hMemDC, iGraphStartX, (int)((_MemDCRect.bottom - 20) - iValue * height), iGraphEndX, _MemDCRect.bottom - 20);	// 20 �ϴ� �ε��� ����

			if (iValue * height <= dfFONT_TITLE_SIZE)
				TextOut(_hMemDC, iGraphStartX + iGraphWidth / 2, (_MemDCRect.bottom - 20) - dfFONT_TITLE_SIZE, szValue, lstrlen(szValue));
			else
				TextOut(_hMemDC, iGraphStartX + iGraphWidth / 2, (int)((_MemDCRect.bottom - 20) - iValue * height), szValue, lstrlen(szValue));
		}
		else
		{
			height = (double)(_iDataMax / (_MemDCRect.bottom - 50.0));
			Rectangle(_hMemDC, iGraphStartX, (int)((_MemDCRect.bottom - 20) - iValue / height), iGraphEndX, _MemDCRect.bottom - 20);	// 20 �ϴ� �ε��� ����

			if (iValue / height <= dfFONT_TITLE_SIZE)
				TextOut(_hMemDC, iGraphStartX + iGraphWidth / 2, (_MemDCRect.bottom - 20) - dfFONT_TITLE_SIZE, szValue, lstrlen(szValue));
			else
				TextOut(_hMemDC, iGraphStartX + iGraphWidth / 2, (int)((_MemDCRect.bottom - 20) - iValue / height), szValue, lstrlen(szValue));
		}

		iGraphStartX += iGraphWidth + dfBAR_GAP;
		iGraphEndX += iGraphWidth + dfBAR_GAP;
	}
	SetTextAlign(_hMemDC, TA_LEFT);

	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hPen);

	ControlDataMax();	// ��Ȳ�� ���� iDataMax�� ����

	Paint_Title();
	Paint_Grid_BarColumnVert();
	Paint_Index_BarColumnVert();
}

void CMonitorGraphUnit::Paint_BarColumnHorz(void)
{
	Paint_Background();


	Paint_Title();
	Paint_Grid();
}

void CMonitorGraphUnit::Paint_LineSingle(void)
{
	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	WCHAR szValue[15];
	HPEN hOldPen;

	double height;
	int originx;
	int originy;
	int destx;
	int desty;

	int iValue;

	Paint_Background();
	Paint_Title();

	_Data.Queue[0]->MyPeek(&iValue);

	if (!Alert(iValue))
		SetTextColor(_hMemDC, 0xffffff);
	else
		SetTextColor(_hMemDC, 0xff);

	
	_itow_s(iValue, szValue, 10);
	
	SetTextAlign(_hMemDC, TA_RIGHT);
	TextOut(_hMemDC, 95, 5, szValue, lstrlen(szValue));
	SetTextAlign(_hMemDC, TA_LEFT);
	TextOut(_hMemDC, 100, 5, _szUnitName, lstrlen(_szUnitName));

	_hLinePen = CreatePen(PS_SOLID, 2, RGB(0, 222, 0));
	hOldPen = (HPEN)SelectObject(_hMemDC, _hLinePen);
	originx = 0;
	originy = _Data.Queue[0]->GetFrontValue();
	destx = originx;
	desty = originy;

	_Data.Queue[0]->InitTemp();
	if (_iDataMax < _MemDCRect.bottom)	// MAX�� â ���̺��� ���� ��� ����
	{
		height = (double)((_MemDCRect.bottom - 30.0) / _iDataMax);
		while (_Data.Queue[0]->MyPeek(&originy))
		{
			MoveToEx(_hMemDC, destx * _MemDCRect.right / dfDATA_CNT, (int)(_MemDCRect.bottom - desty * height), NULL);	//_GraphRect �̿��ϵ��� ����
			LineTo(_hMemDC, originx * _MemDCRect.right / dfDATA_CNT, (int)(_MemDCRect.bottom - originy * height));
			destx = originx;
			desty = originy;
			originx++;
		}
	}
	else								// MAX�� â ���̺��� Ŭ ��� ���� ����
	{
		height = (double)(_iDataMax / (_MemDCRect.bottom - 30.0));
		while (_Data.Queue[0]->MyPeek(&originy))
		{
			MoveToEx(_hMemDC, destx * _MemDCRect.right / dfDATA_CNT, (int)(_MemDCRect.bottom - desty / height), NULL);
			LineTo(_hMemDC, originx * _MemDCRect.right / dfDATA_CNT, (int)(_MemDCRect.bottom - originy / height));
			destx = originx;
			desty = originy;
			originx++;
		}
	}
	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(_hLinePen);

	ControlDataMax();	// ��Ȳ�� ���� iDataMax�� ����

	Paint_Grid();
}

void CMonitorGraphUnit::Paint_LineMulti(void)
{
	COLORREF RGB;
	HPEN hOldPen;

	double height;
	double width = _MemDCRect.right - 150;	// ���� ����(150)�� ������ �ʺ�
	int originx[dfMAXCOLUMN];
	int originy[dfMAXCOLUMN];
	int destx[dfMAXCOLUMN];
	int desty[dfMAXCOLUMN];

	SetTextColor(_hMemDC, 0xffffff); //settextcolor
	Paint_Background();
	
	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
			break;

		switch (Cnt)
		{
		case 0:
			RGB = 0x0000ff;
			break;
		case 1:
			RGB = 0x00ffff;
			break;
		case 2:
			RGB = 0x00ff00;
			break;
		case 3:
			RGB = 0xffff00;
			break;
		case 4:
			RGB = 0xff0000;
			break;
		case 5:
			RGB = 0xff00ff;
			break;
		case 6:
			RGB = 0x00008c;
			break;
		case 7:
			RGB = 0x008c8c;
			break;
		case 8:
			RGB = 0x008c00;
			break;
		case 9:
			RGB = 0x8c8c00;
			break;
		case 10:
			RGB = 0x8c008c;
			break;
		case 11:
			RGB = 0xffff8c;
			break;
		case 12:
			RGB = 0xff8c8c;
			break;
		case 13:
			RGB = 0xff8cff;
			break;
		case 14:
			RGB = 0x8c8cff;
			break;
		case 15:
			RGB = 0x8cff8c;
			break;
		}
		_hLinePen = CreatePen(PS_SOLID, 2, RGB);
		hOldPen = (HPEN)SelectObject(_hMemDC, _hLinePen);
		originx[Cnt] = 0;
		originy[Cnt] = _Data.Queue[Cnt]->GetFrontValue();
		destx[Cnt] = originx[Cnt];
		desty[Cnt] = originy[Cnt];

		_Data.Queue[Cnt]->InitTemp();
		if (_iDataMax < _MemDCRect.bottom)	// MAX�� â ���̺��� ���� ��� ����
		{
			height = (double)((_MemDCRect.bottom - 30.0) / _iDataMax);
			for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
			{
				while (_Data.Queue[Cnt]->MyPeek(&originy[Cnt]))
				{
					MoveToEx(_hMemDC, (int)(destx[Cnt] * width / dfDATA_CNT), (int)(_MemDCRect.bottom - desty[Cnt] * height), NULL);
					LineTo(_hMemDC, (int)(originx[Cnt] * width / dfDATA_CNT), (int)(_MemDCRect.bottom - originy[Cnt] * height));
					destx[Cnt] = originx[Cnt];
					desty[Cnt] = originy[Cnt];
					originx[Cnt]++;
				}
			}
		}
		else								// MAX�� â ���̺��� Ŭ ��� ���� ����
		{
			height = (double)(_iDataMax / (_MemDCRect.bottom - 30.0));
			for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
			{
				while (_Data.Queue[Cnt]->MyPeek(&originy[Cnt]))
				{
					MoveToEx(_hMemDC, (int)(destx[Cnt] * width / dfDATA_CNT), (int)(_MemDCRect.bottom - desty[Cnt] / height), NULL);
					LineTo(_hMemDC, (int)(originx[Cnt] * width / dfDATA_CNT), (int)(_MemDCRect.bottom - originy[Cnt] / height));
					destx[Cnt] = originx[Cnt];
					desty[Cnt] = originy[Cnt];
					originx[Cnt]++;
				}
			}
		}
		SelectObject(_hMemDC, hOldPen);
		DeleteObject(_hLinePen);
	}


	ControlDataMax();	// ��Ȳ�� ���� iDataMax�� ����

	Paint_Title();
	Paint_Grid();
	Paint_Index_LineMulti();
}

void CMonitorGraphUnit::Paint_Pie(void)
{
	HPEN hPen = CreatePen(PS_NULL, 0, 0);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hPen);
	HBRUSH hBrush;
	HBRUSH hOldBrush;
	COLORREF RGB;

	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("���� ���"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	WCHAR szValue[32];

	Paint_Background();

	////////////////////////////////////////////////////////////
	//
	//  ������ API���� ���� �׷����� Pie��� GDI �Լ��� ����Ͽ� �ս��� �׸� �� �ִ�.
	//
	//  * Pie �Լ�
	//	
	//	BOOL Pie(
	//		_In_ HDC hdc,			// ��� ���
	//		_In_ int nLeftRect,		// ��ü �� ���� ������ǥ
	//		_In_ int nTopRect,		// ��ü �� ���� �����ǥ
	//		_In_ int nRightRect,	// ��ü �� ���� ������ǥ
	//		_In_ int nBottomRect,	// ��ü �� ���� �ϴ���ǥ
	//		_In_ int nXRadial1,		// �� �׷��� ���� X (ȣ�� ������)
	//		_In_ int nYRadial1,		// �� �׷��� ���� Y
	//		_In_ int nXRadial2,		// �� �׷��� �� X	(ȣ�� ����)
	//		_In_ int nYRadial2		// �� �׷��� �� Y
	//	);
	//
	//  nXRadial, nYRadial�� �ش� ��ǥ�� ���� ���� �׸��ٱ⺸�ٴ� ���⸸ �������ִ� ����
	//
	//    ���� ���� �����͸� ����� �� ���� �׷����� �׸����� ������ ���� ������ �ʿ��ϴ�.
	//	 #���� �߾� ������ ���������� ����
	//	 1. �׸����� �ϴ� �� �׷����� ���� ����
	//	 2. ���� �׷����� �����μ� �����͸� ǥ���ϹǷ� �� �������� ����(�����)�� ���Ѵ�
	//	 3. �� ������ ���� ���� ���� ���Ѵ� (100% = 360�� = 2���� = 6.2831)
	//	 4. sin, cos�� Ȱ���Ͽ� ���ȿ� ���� ��ǥ�� ���Ѵ�. (���⸸ ������ �ǹǷ� ���̴� ũ�� �������)
	//	 5. 4���� ����� 0, 0 ������ ��ǥ�̹Ƿ� ������ ��ǥ�� �׷��� �����ġ�� ����
	//	 6. �귯�ø� �����Ͽ� ���� ���� �� ������ ��ǥ�� Pie �Լ� ȣ��
	//
	////////////////////////////////////////////////////////////

	int iValue;
	int iTotalValue = 0;						// �� ������ ��ġ �� 
	//int iDiameter = _MemDCRect.bottom - 30;			// �� ����
	//if (_MemDCRect.right < _MemDCRect.bottom - 30)
	//	iDiameter = _MemDCRect.right;

	int iGraphDrawX = 0 ;		// �׷����� �׸� HDC ��ǥ
	int iGraphDrawY = 30;

	//int iX = iDiameter;							// ������ X, Y
	//int iY = iDiameter / 2;						// �߾� �������� ��ǥ
	int iX = _MemDCRect.right;
	int iY = _MemDCRect.bottom - 30;

	int iNX = iX;								// ���� �� ��ǥ & ���� ���� ��ǥ
	int iNY = iY;

	double dfAccuRadian = 0;					// ���� �ջ� ���� (���� �׸����)

	
	//---------------------------------------------------
	// ���տ� ���� �� �������� ������ ������ �˾Ƴ���.
	// 360�� -> 6.2831 ����.
	// ��ü : 6.2831 = �� ������ : N Radian
	//---------------------------------------------------

	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
			break;
		_Data.Queue[Cnt]->Peek(&iValue);
		iTotalValue += iValue;
	}

	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
			break;
		_Data.Queue[Cnt]->Peek(&iValue);
		if (iValue == 0)
			continue;
		switch (Cnt)
		{
		case 0:
			RGB = 0x0000ff;
			break;
		case 1:
			RGB = 0x00ffff;
			break;
		case 2:
			RGB = 0x00ff00;
			break;
		case 3:
			RGB = 0xffff00;
			break;
		case 4:
			RGB = 0xff0000;
			break;
		case 5:
			RGB = 0xff00ff;
			break;
		case 6:
			RGB = 0x00008c;
			break;
		case 7:
			RGB = 0x008c8c;
			break;
		case 8:
			RGB = 0x008c00;
			break;
		case 9:
			RGB = 0x8c8c00;
			break;
		case 10:
			RGB = 0x8c008c;
			break;
		case 11:
			RGB = 0xffff8c;
			break;
		case 12:
			RGB = 0xff8c8c;
			break;
		case 13:
			RGB = 0xff8cff;
			break;
		case 14:
			RGB = 0x8c8cff;
			break;
		case 15:
			RGB = 0x8cff8c;
			break;
		}
		hBrush = CreateSolidBrush(RGB);
		hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
		_itow_s(iValue * 100 / iTotalValue, szValue, 10);
		
		
		//---------------------------------------------------
		// �ش� ���� ���� ���� ���ϱ� (����ġ)
		// ������ ����ġ�� ���ϸ�, ���� �׸��� ��ǥ�� ������ǥ�� �������� �׸���.
		//---------------------------------------------------
		// iTotalData : 6.2831 = iData : dfRadian)
		dfAccuRadian += iValue * 6.2831 / iTotalValue;

		// ���� �׷������� 1.0�� ��ǥ �������� ���ȿ� ���� X �� ��, �׸��� * 100���� Ȯ��
		/*iX = iDiameter / 2 + (int)(cos(dfAccuRadian) * 100);
		iY = iDiameter / 2 + (int)(sin(dfAccuRadian) * 100);*/
		iX = _MemDCRect.right / 2 + (int)(cos(dfAccuRadian) * 100);
		iY = (_MemDCRect.bottom - 30) / 2 + (int)(sin(dfAccuRadian) * 100);

		// iDiameter / 2 + �κ��� 0,0 �������� ��� ��, ���� �߽��� �̵�

		//----------------------------------------------------
		// ���� �� ���, �׸񺰷� �÷��� �ٸ��� �Ϸ��� �귯�ø� �ٲپ� �ش�.
		//----------------------------------------------------
		Pie(_hMemDC, iGraphDrawX, iGraphDrawY, iGraphDrawX + _MemDCRect.right, iGraphDrawY + (_MemDCRect.bottom - 30),
			iGraphDrawX + iX,
			iGraphDrawY + iY,
			iGraphDrawX + iNX,
			iGraphDrawY + iNY);
		SetTextAlign(_hMemDC, TA_CENTER);
		SetTextColor(_hMemDC, 0);
		TextOut(_hMemDC, iGraphDrawX + _MemDCRect.right / 2 + (int)(cos(dfAccuRadian - (iValue / 2 * 6.2831 / iTotalValue)) * 150) + 2, iGraphDrawY + (_MemDCRect.bottom - 30) / 2 + (int)(sin(dfAccuRadian - (iValue / 2 * 6.2831 / iTotalValue)) * 150) + 2, szValue, lstrlen(szValue));
		SetTextColor(_hMemDC, 0xffffff);
		TextOut(_hMemDC, iGraphDrawX + _MemDCRect.right / 2 + (int)(cos(dfAccuRadian - (iValue / 2 * 6.2831 / iTotalValue)) * 150) + 0, iGraphDrawY + (_MemDCRect.bottom - 30) / 2 + (int)(sin(dfAccuRadian - (iValue / 2 * 6.2831 / iTotalValue)) * 150) + 0, szValue, lstrlen(szValue));

		iNX = iX;
		iNY = iY;
		SelectObject(_hMemDC, hOldBrush);
		DeleteObject(hBrush);
	}
	
	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hPen);
	Paint_Title();
	Paint_Index_Pie();
}


void CMonitorGraphUnit::ControlDataMax(void)
{
	int iValue;
	int iValueMax = 0;

	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
			break;

		_Data.Queue[Cnt]->InitTemp();
		while (_Data.Queue[Cnt]->MyPeek(&iValue))
		{
			if (iValueMax < iValue)
				iValueMax = iValue;
		}
		
	}


	if (iValueMax < _iDataMax /2)
	{
		if(_iDataMax - 100 != 0)
			_iDataMax -= 100;
		return;
	}

	if (iValueMax > _iDataMax)
	{
		_iDataMax += 100;
		return;
	}
}

void CMonitorGraphUnit::ClearMemDC(void)
{
	PatBlt(_hMemDC, 0, 0, _MemDCRect.right, _MemDCRect.bottom, PATCOPY);
}

void CMonitorGraphUnit::FlipMemDC(HDC hdc)
{
	BitBlt(hdc, 0, 0, _MemDCRect.right, _MemDCRect.bottom, _hMemDC, 0, 0, SRCCOPY);
}
