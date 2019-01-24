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

	PutThis();	//  hWnd-this 테이블에 현 자식 윈도우 클래스의 this 삽입
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

	/* 그래프 추가할때 손봐야 할 곳*/
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
		wcscpy_s(szTitle, L"동시접속자");
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
		wcscpy_s(szTitle, L"동시접속자비율");
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
		wcscpy_s(szTitle, L"네트워크");
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
		wcscpy_s(szTitle, L"메모리");
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
		wcscpy_s(szTitle, L"DB 버퍼");
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
		wcscpy_s(szTitle, L"프로세스");
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
	// 바탕
	//------------------------------------------------------
	HBRUSH hBrush = CreateSolidBrush(_BackColor);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, 0, 0, _MemDCRect.right, _MemDCRect.bottom); // NULLPEN으로 그리던지 배경과 같은 색상의 펜으로 그리던지 테두리가 그려지지 않는다
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);
}

void CMonitorGraphUnit::Paint_Title(void)
{
	//------------------------------------------------------
	// 타이틀
	//------------------------------------------------------
	HBRUSH hBrush = CreateSolidBrush(_BackColor * 2);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 7/5);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, 0, 0, _MemDCRect.right, dfTITLE_HEIGHT); // NULLPEN으로 그리던지 배경과 같은 색상의 펜으로 그리던지 테두리가 그려지지 않는다
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);


	SetTextAlign(_hMemDC, TA_LEFT);
	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	
	TextOut(_hMemDC, 5, 5, _szTitle, lstrlen(_szTitle));
	SetBkMode(_hMemDC, TRANSPARENT);
	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
}

void CMonitorGraphUnit::Paint_Grid(void)
{
	//------------------------------------------------------
	// 그리드 라인
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
	// 그리드 수치
	//------------------------------------------------------
	HFONT hFont = CreateFont(dfFONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
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
	// 그리드 라인
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
	// 그리드 수치
	//------------------------------------------------------
	HFONT hFont = CreateFont(dfFONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
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
	// 인덱스 바탕
	//------------------------------------------------------
	HBRUSH hBrush = CreateSolidBrush(_BackColor * 7 /5);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 7 / 5);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, _MemDCRect.right - 155, dfTITLE_HEIGHT, _MemDCRect.right, _MemDCRect.bottom); // NULLPEN으로 그리던지 배경과 같은 색상의 펜으로 그리던지 테두리가 그려지지 않는다
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);

	//------------------------------------------------------
	// 인덱스 
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
		HFONT hFont = CreateFont(dfFONT_INDEX_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
		HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
		SetTextColor(_hMemDC,RGB);
		TextOut(_hMemDC, _MemDCRect.right - 140, dfTITLE_HEIGHT + 10 + dfFONT_INDEX_SIZE * Cnt, L"━", 1);
		TextOut(_hMemDC, _MemDCRect.right - 120, dfTITLE_HEIGHT + 10 + dfFONT_INDEX_SIZE * Cnt, (LPCWSTR)_Data.szName[Cnt], lstrlen((LPCWSTR)_Data.szName[Cnt]));
		SelectObject(_hMemDC, hOldFont);
		DeleteObject(hFont);
	}
}

void CMonitorGraphUnit::Paint_Index_Pie(void)
{
	//------------------------------------------------------
	// 인덱스 
	//------------------------------------------------------
	for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
	{
		if (_Data.iType[Cnt] == NULL)
			break;

		COLORREF RGB;
		HFONT hFont = CreateFont(dfFONT_INDEX_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
		HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
		SetTextColor(_hMemDC, 0);
		TextOut(_hMemDC, _MemDCRect.right - 139, dfTITLE_HEIGHT + 11 + dfFONT_INDEX_SIZE * Cnt, L"■", 1);
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
		TextOut(_hMemDC, _MemDCRect.right - 140, dfTITLE_HEIGHT + 10 + dfFONT_INDEX_SIZE * Cnt, L"■", 1);
		TextOut(_hMemDC, _MemDCRect.right - 100, dfTITLE_HEIGHT + 10 + dfFONT_INDEX_SIZE * Cnt, (LPCWSTR)_Data.szName[Cnt], lstrlen((LPCWSTR)_Data.szName[Cnt]));
		SelectObject(_hMemDC, hOldFont);
		DeleteObject(hFont);
	}
}

void CMonitorGraphUnit::Paint_Index_BarColumnVert(void)
{
	//------------------------------------------------------
	// 인덱스 바탕
	//------------------------------------------------------
	HBRUSH hBrush = CreateSolidBrush(_BackColor * 7 / 5);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 7 / 5);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, 0, _MemDCRect.bottom - 20, _MemDCRect.right, _MemDCRect.bottom); // NULLPEN으로 그리던지 배경과 같은 색상의 펜으로 그리던지 테두리가 그려지지 않는다
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);

	//------------------------------------------------------
	// 인덱스 
	//------------------------------------------------------
	int iGraphWidth = (_MemDCRect.right - (dfBAR_REMAIN * 2) - (dfBAR_GAP * (_iColumnCnt - 1))) / _iColumnCnt;
	int iGraphEndX = dfBAR_REMAIN + iGraphWidth;
	int iGraphStartX = dfBAR_REMAIN;
	COLORREF RGB;

	SetTextAlign(_hMemDC, TA_CENTER);
	HFONT hFont = CreateFont(dfFONT_INDEX_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
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
	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
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

	ControlDataMax();	// 상황에 따라 iDataMax값 조절

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

	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
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
			Rectangle(_hMemDC, iGraphStartX, (int)((_MemDCRect.bottom - 20) - iValue * height), iGraphEndX, _MemDCRect.bottom - 20);	// 20 하단 인덱스 공간

			if (iValue * height <= dfFONT_TITLE_SIZE)
				TextOut(_hMemDC, iGraphStartX + iGraphWidth / 2, (_MemDCRect.bottom - 20) - dfFONT_TITLE_SIZE, szValue, lstrlen(szValue));
			else
				TextOut(_hMemDC, iGraphStartX + iGraphWidth / 2, (int)((_MemDCRect.bottom - 20) - iValue * height), szValue, lstrlen(szValue));
		}
		else
		{
			height = (double)(_iDataMax / (_MemDCRect.bottom - 50.0));
			Rectangle(_hMemDC, iGraphStartX, (int)((_MemDCRect.bottom - 20) - iValue / height), iGraphEndX, _MemDCRect.bottom - 20);	// 20 하단 인덱스 공간

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

	ControlDataMax();	// 상황에 따라 iDataMax값 조절

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
	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
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
	if (_iDataMax < _MemDCRect.bottom)	// MAX가 창 높이보다 작을 경우 비율
	{
		height = (double)((_MemDCRect.bottom - 30.0) / _iDataMax);
		while (_Data.Queue[0]->MyPeek(&originy))
		{
			MoveToEx(_hMemDC, destx * _MemDCRect.right / dfDATA_CNT, (int)(_MemDCRect.bottom - desty * height), NULL);	//_GraphRect 이용하도록 변경
			LineTo(_hMemDC, originx * _MemDCRect.right / dfDATA_CNT, (int)(_MemDCRect.bottom - originy * height));
			destx = originx;
			desty = originy;
			originx++;
		}
	}
	else								// MAX가 창 높이보다 클 경우 비율 조절
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

	ControlDataMax();	// 상황에 따라 iDataMax값 조절

	Paint_Grid();
}

void CMonitorGraphUnit::Paint_LineMulti(void)
{
	COLORREF RGB;
	HPEN hOldPen;

	double height;
	double width = _MemDCRect.right - 150;	// 색인 영역(150)을 제외한 너비
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
		if (_iDataMax < _MemDCRect.bottom)	// MAX가 창 높이보다 작을 경우 비율
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
		else								// MAX가 창 높이보다 클 경우 비율 조절
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


	ControlDataMax();	// 상황에 따라 iDataMax값 조절

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

	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	WCHAR szValue[32];

	Paint_Background();

	////////////////////////////////////////////////////////////
	//
	//  윈도우 API에서 원형 그래프는 Pie라는 GDI 함수를 사용하여 손쉽게 그릴 수 있다.
	//
	//  * Pie 함수
	//	
	//	BOOL Pie(
	//		_In_ HDC hdc,			// 출력 대상
	//		_In_ int nLeftRect,		// 전체 원 영역 좌측좌표
	//		_In_ int nTopRect,		// 전체 원 영역 상단좌표
	//		_In_ int nRightRect,	// 전체 원 영역 우측좌표
	//		_In_ int nBottomRect,	// 전체 원 영역 하단좌표
	//		_In_ int nXRadial1,		// 원 그래프 시작 X (호의 시작점)
	//		_In_ int nYRadial1,		// 원 그래프 시작 Y
	//		_In_ int nXRadial2,		// 원 그래프 끝 X	(호의 끝점)
	//		_In_ int nYRadial2		// 원 그래프 끝 Y
	//	);
	//
	//  nXRadial, nYRadial는 해당 좌표로 직접 원을 그린다기보다는 방향만 설정해주는 역할
	//
	//    여러 개의 데이터를 제대로 된 원형 그래프로 그리려면 다음과 같은 과정이 필요하다.
	//	 #우측 중앙 직선을 시작점으로 지정
	//	 1. 그리고자 하는 원 그래프의 지름 결정
	//	 2. 원형 그래프는 비율로서 데이터를 표현하므로 각 데이터의 비율(백분율)을 구한다
	//	 3. 각 비율에 대한 라디안 값을 구한다 (100% = 360도 = 2파이 = 6.2831)
	//	 4. sin, cos를 활용하여 라디안에 대한 좌표를 구한다. (방향만 잡으면 되므로 길이는 크게 상관없음)
	//	 5. 4번의 결과는 0, 0 기준의 좌표이므로 구해진 좌표를 그래프 출력위치로 보정
	//	 6. 브러시를 생성하여 색상 지정 후 구해진 좌표로 Pie 함수 호출
	//
	////////////////////////////////////////////////////////////

	int iValue;
	int iTotalValue = 0;						// 총 데이터 수치 합 
	//int iDiameter = _MemDCRect.bottom - 30;			// 원 지름
	//if (_MemDCRect.right < _MemDCRect.bottom - 30)
	//	iDiameter = _MemDCRect.right;

	int iGraphDrawX = 0 ;		// 그래프를 그릴 HDC 좌표
	int iGraphDrawY = 30;

	//int iX = iDiameter;							// 시작점 X, Y
	//int iY = iDiameter / 2;						// 중앙 우측직선 좌표
	int iX = _MemDCRect.right;
	int iY = _MemDCRect.bottom - 30;

	int iNX = iX;								// 파이 끝 좌표 & 다음 시작 좌표
	int iNY = iY;

	double dfAccuRadian = 0;					// 누적 합산 라디안 (파이 그리기용)

	
	//---------------------------------------------------
	// 총합에 대한 각 데이터의 비율로 라디안을 알아낸다.
	// 360도 -> 6.2831 라디안.
	// 전체 : 6.2831 = 각 데이터 : N Radian
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
		// 해당 값의 라디안 비율 구하기 (누적치)
		// 라디안은 누적치로 구하며, 파이 그리기 좌표는 이전좌표를 시작으로 그린다.
		//---------------------------------------------------
		// iTotalData : 6.2831 = iData : dfRadian)
		dfAccuRadian += iValue * 6.2831 / iTotalValue;

		// 수학 그래프에서 1.0의 좌표 기준으로 라디안에 대한 X 축 값, 그리고 * 100으로 확대
		/*iX = iDiameter / 2 + (int)(cos(dfAccuRadian) * 100);
		iY = iDiameter / 2 + (int)(sin(dfAccuRadian) * 100);*/
		iX = _MemDCRect.right / 2 + (int)(cos(dfAccuRadian) * 100);
		iY = (_MemDCRect.bottom - 30) / 2 + (int)(sin(dfAccuRadian) * 100);

		// iDiameter / 2 + 부분은 0,0 기준으로 계산 후, 센터 중심점 이동

		//----------------------------------------------------
		// 파이 원 출력, 항목별로 컬러를 다르게 하려면 브러시를 바꾸어 준다.
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
