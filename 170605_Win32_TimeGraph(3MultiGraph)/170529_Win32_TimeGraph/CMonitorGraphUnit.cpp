#include "stdafx.h"
#include "CMonitorGraphUnit.h"

#define dfCOLUMN_NAME_LEN 32

CMonitorGraphUnit::stHWNDtoTHIS	CMonitorGraphUnit::_HWND_This;
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
extern WNDCLASSEXW wcex;

int g_iWindowClassNumbering;

CMonitorGraphUnit::CMonitorGraphUnit(HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight, int iColumnCnt) 
{
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

	WCHAR szTitle[32];
	int iDataMax;
	int iDataAlert;
	/* 그래프 추가할때 손봐야 할 곳*/
	switch (g_iWindowClassNumbering)
	{
	case 0:
		wcscpy_s(_szWindowClass, L"CPU");
		wcscpy_s(szTitle, L"CPU");
		iDataMax = 100;
		iDataAlert = 50;
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(128);
		}
		break;
	case 1:
		wcscpy_s(_szWindowClass, L"CCU");
		wcscpy_s(szTitle, L"동시접속자");
		iDataMax = 5000;
		iDataAlert = NULL;
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(128);
		}
		break;
	case 2:
		wcscpy_s(_szWindowClass, L"NETWORK");	
		wcscpy_s(szTitle, L"네트워크");
		iDataMax = 500;
		iDataAlert = NULL;
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(128);
		}
		break;
	case 3:
		wcscpy_s(_szWindowClass, L" DB_QUERRY");
		wcscpy_s(szTitle, L"DB 쿼리");
		iDataMax = 800;
		iDataAlert = NULL;
		for (int Cnt = 0; Cnt < _iColumnCnt; ++Cnt)
		{
			_Data.Queue[Cnt] = new CQueue<int>(128);
		}
		break;
	default:
		break;
	}
	g_iWindowClassNumbering++;
	SetInformation(szTitle, iDataMax, iDataAlert);

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

void CMonitorGraphUnit::SetInformation(WCHAR * szTitle, int iDataMax, int iDataAlert)
{
	wcscpy_s(_szTitle , szTitle);
	_iDataMax = iDataMax;
	_iDataAlert = iDataAlert;
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
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//////////////////////////////////////////////////////////////////////////////////////////////
		//	윈도우 확장을 조정할 수 있는 맵핑 모드에는 MM_ISOTROPIC과 MM_ANISOTROPIC 두 가지가 있으며 
		//  나머지 맵핑 모드에서는 윈도우 확장을 변경할 수 없다.
		SetMapMode(hMemDC, MM_ANISOTROPIC);
		SetWindowExtEx(hMemDC, MemDCRect.right, MemDCRect.bottom, NULL);
		GetClientRect(hWnd, &MemDCRect);
		SetViewportExtEx(hMemDC, MemDCRect.right, MemDCRect.bottom, NULL);
		//////////////////////////////////////////////////////////////////////////////////////////////
		
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

BOOL CMonitorGraphUnit::Alert(void)
{
	return 0;
}

void CMonitorGraphUnit::Paint_Title(void)
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

	//------------------------------------------------------
	// 타이틀
	//------------------------------------------------------
	hBrush = CreateSolidBrush(_BackColor * 4/5);
	hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 4/5);
	hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	Rectangle(_hMemDC, 0, 0, _MemDCRect.right, dfTITLE_HEIGHT); // NULLPEN으로 그리던지 배경과 같은 색상의 펜으로 그리던지 테두리가 그려지지 않는다
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);


	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	SetTextColor(_hMemDC, RGB(255, 255, 255));
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
	HPEN hLinePen = CreatePen(PS_SOLID, 1, (COLORREF)(_BackColor * 1.9));
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hLinePen);
	SetBkMode(_hMemDC, TRANSPARENT);
	SetROP2(_hMemDC, R2_XORPEN);
	MoveToEx(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / 4), NULL);
	LineTo(_hMemDC, _MemDCRect.right, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / 4));

	MoveToEx(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 2 / 4), NULL);
	LineTo(_hMemDC, _MemDCRect.right, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 2 / 4));

	MoveToEx(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 3 / 4), NULL);
	LineTo(_hMemDC, _MemDCRect.right, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 3 / 4));
	SetROP2(_hMemDC, R2_COPYPEN);

	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hLinePen);

	//------------------------------------------------------
	// 그리드 수치
	//------------------------------------------------------
	HFONT hFont = CreateFont(dfFONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	WCHAR szLineNumber[10];

	_itow_s((_iDataMax * 1 / 4), szLineNumber, 10);
	SetTextColor(_hMemDC, RGB(255, 255, 255));
	TextOut(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / 4), szLineNumber, lstrlen(szLineNumber));

	_itow_s((_iDataMax * 1 / 2), szLineNumber, 10, 10);
	TextOut(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 1 / 2), szLineNumber, lstrlen(szLineNumber));

	_itow_s((_iDataMax * 3 / 4), szLineNumber, 10, 10);
	TextOut(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0) * 3 / 4), szLineNumber, lstrlen(szLineNumber));

	_itow_s((_iDataMax), szLineNumber, 10, 10);
	TextOut(_hMemDC, 0, (int)(_MemDCRect.bottom - (_MemDCRect.bottom - 30.0)), szLineNumber, lstrlen(szLineNumber));

	SelectObject(_hMemDC, hOldFont);
	DeleteObject(hFont);
}

void CMonitorGraphUnit::Paint_Index(void)
{
	//------------------------------------------------------
	// 인덱스 바탕
	//------------------------------------------------------
	HBRUSH hBrush = CreateSolidBrush(_BackColor * 3/5);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);
	HPEN hLinePen = CreatePen(PS_SOLID, 1, _BackColor * 3/5);
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

void CMonitorGraphUnit::Paint_BarSingleVert(void)
{
	Paint_Title();

	double height;
	HPEN hPen = CreatePen(PS_SOLID, 1, _BackColor);
	HPEN hOldPen = (HPEN)SelectObject(_hMemDC, hPen);
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 150, 0));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(_hMemDC, hBrush);


	HFONT hFont = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("맑은 고딕"));
	HFONT hOldFont = (HFONT)SelectObject(_hMemDC, hFont);
	WCHAR szValue[10];

	int iValue;

	SetTextColor(_hMemDC, 0xffffff);
	SetTextAlign(_hMemDC, TA_CENTER);

	_Data.Queue[0]->MyPeek(&iValue);
	if (_iDataMax < _MemDCRect.bottom)
	{
		height = (double)((_MemDCRect.bottom - 30.0) / _iDataMax);
		Rectangle(_hMemDC, _MemDCRect.right / 5, (int)(_MemDCRect.bottom - iValue * height), _MemDCRect.right * 4 / 5, _MemDCRect.bottom);

		_itow_s(iValue, szValue, sizeof(szValue));
		if (iValue * height <= dfFONT_TITLE_SIZE)
			TextOut(_hMemDC, _MemDCRect.right / 2, _MemDCRect.bottom - dfFONT_TITLE_SIZE, szValue, lstrlen(szValue));
		else
			TextOut(_hMemDC, _MemDCRect.right / 2, (int)(_MemDCRect.bottom - iValue * height), szValue, lstrlen(szValue));

		SelectObject(_hMemDC, hOldFont);
		DeleteObject(hFont);
	}
	else
	{
		height = (double)(_iDataMax / (_MemDCRect.bottom - 30.0));
		Rectangle(_hMemDC, _MemDCRect.right / 5, (int)(_MemDCRect.bottom - iValue / height), _MemDCRect.right * 4 / 5, _MemDCRect.bottom);
	}

	SetTextAlign(_hMemDC, TA_LEFT);

	SelectObject(_hMemDC, hOldPen);
	DeleteObject(hPen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hBrush);

	Paint_Grid();
}

void CMonitorGraphUnit::Paint_BarSingleHorz(void)
{
	Paint_Title();
	Paint_Grid();
}

void CMonitorGraphUnit::Paint_BarColumnVert(void)
{
	Paint_Title();
	Paint_Grid();
}

void CMonitorGraphUnit::Paint_BarColumnHorz(void)
{
	Paint_Title();
	Paint_Grid();
}

void CMonitorGraphUnit::Paint_LineSingle(void)
{
	Paint_Title();
	
	HPEN hOldPen;
	double height;

	int originx;
	int originy;
	int destx;
	int desty;

	_hLinePen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
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
			MoveToEx(_hMemDC, destx * _MemDCRect.right / 128, (int)(_MemDCRect.bottom - desty * height), NULL);	//_GraphRect 이용하도록 변경
			LineTo(_hMemDC, originx * _MemDCRect.right / 128, (int)(_MemDCRect.bottom - originy * height));
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
			MoveToEx(_hMemDC, destx * _MemDCRect.right / 128, (int)(_MemDCRect.bottom - desty / height), NULL);
			LineTo(_hMemDC, originx * _MemDCRect.right / 128, (int)(_MemDCRect.bottom - originy / height));
			destx = originx;
			desty = originy;
			originx++;
		}
	}
	SelectObject(_hMemDC, hOldPen);
	DeleteObject(_hLinePen);

	Paint_Grid();
}

void CMonitorGraphUnit::Paint_LineMulti(void)
{
	Paint_Title();

	COLORREF RGB;
	HPEN hOldPen;

	double height;
	double width = _MemDCRect.right - 150;	// 색인 영역(150)을 제외한 너비
	int originx[dfMAXCOLUMN];
	int originy[dfMAXCOLUMN];
	int destx[dfMAXCOLUMN];
	int desty[dfMAXCOLUMN];

	
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
					MoveToEx(_hMemDC, (int)(destx[Cnt] * width / 128), (int)(_MemDCRect.bottom - desty[Cnt] * height), NULL);
					LineTo(_hMemDC, (int)(originx[Cnt] * width / 128), (int)(_MemDCRect.bottom - originy[Cnt] * height));
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
					MoveToEx(_hMemDC, (int)(destx[Cnt] * width / 128), (int)(_MemDCRect.bottom - desty[Cnt] / height), NULL);
					LineTo(_hMemDC, (int)(originx[Cnt] * width / 128), (int)(_MemDCRect.bottom - originy[Cnt] / height));
					destx[Cnt] = originx[Cnt];
					desty[Cnt] = originy[Cnt];
					originx[Cnt]++;
				}
			}
		}
		SelectObject(_hMemDC, hOldPen);
		DeleteObject(_hLinePen);
	}

	Paint_Grid();
	Paint_Index();
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
