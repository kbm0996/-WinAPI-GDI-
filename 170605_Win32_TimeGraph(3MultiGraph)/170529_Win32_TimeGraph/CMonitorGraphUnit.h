#ifndef __MONITOR_UNIT__
#define __MONITOR_UNIT__
#include "CQueue.h"
#include "170529_Win32_TimeGraph.h"

//////////////////////////////////////////////////////////////////////
// ����͸� �� ���۽� ���Ǵ� �׷��� ���� ������ Ŭ����
//
// 
//////////////////////////////////////////////////////////////////////
//------------------------------------------------------
// ���� �߻� ������ �޽���
//------------------------------------------------------
#define UM_ALERT					WM_USER + 1

#define dfALERT_CANCEL_TIMER_ID		200
#define dfALERT_CANCEL_TIME			500

//------------------------------------------------------
// �ִ� �������� ������
//------------------------------------------------------
#define dfMAXCHILD			50

//------------------------------------------------------
// ��Ʈ������, Ÿ��Ʋ / �׷�����ǥ
//------------------------------------------------------
#define dfFONT_TITLE_SIZE	20
#define dfFONT_POINT_SIZE	12
#define dfFONT_INDEX_SIZE	18

//------------------------------------------------------
// Ÿ��Ʋ ǥ�ÿ��� ����
//------------------------------------------------------
#define dfTITLE_HEIGHT		30

//------------------------------------------------------
// ������ ��ǥ ����
//------------------------------------------------------
#define dfGRID_VERT_NUM		4

//------------------------------------------------------
// �÷� �ִ� ����
//------------------------------------------------------
#define dfMAXCOLUMN	30


class CMonitorGraphUnit
{
public:

	enum TYPE
	{
		BAR_SINGLE_VERT,		// �� �ϳ� ����
		BAR_SINGLE_HORZ,		// �� �ϳ� ����
		BAR_COLUMN_VERT,		// �� ������ ����
		BAR_COLUMN_HORZ,		// �� ������ ����
		LINE_SINGLE,			// ���� �׷��� �ϳ�
		LINE_MULTI,				// ���� �׷��� ������
		PIE						// �� ���� ������
	};

	typedef struct ST_HWNDtoTHIS
	{
		HWND				hWnd[dfMAXCHILD];
		CMonitorGraphUnit	*pThis[dfMAXCHILD];
	}stHWNDtoTHIS;

	typedef struct ST_TYPEtoID
	{
		ULONG64				u64ServerID[dfMAXCOLUMN];	// ���� ID
		WCHAR				szName[dfMAXCOLUMN][32];	// ���� �̸�
		int					iType[dfMAXCOLUMN];			// ������ ����
		CQueue<int>			*Queue[dfMAXCOLUMN];		// ������ ��
	}stTYPEtoID;


public:

	CMonitorGraphUnit(HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight, int iColumnCnt);
	~CMonitorGraphUnit();

	/////////////////////////////////////////////////////////
	// �׷��� ���� �Է�
	//
	// ������ �̸�, �ִ�ġ, �溸��ġ (0�̸� ����)
	/////////////////////////////////////////////////////////
	void	SetInformation(WCHAR *szTitle, int iDataMax, int iDataAlert);
	

	/////////////////////////////////////////////////////////
	// Į�� ���� �Է�
	//
	// ���� ID, ������ ����, ���� �̸�
	/////////////////////////////////////////////////////////
	void	SetDataColumnInfo(ULONG64 u64ServerID, int iType, WCHAR *szName); 


	/////////////////////////////////////////////////////////
	// �׷��� ���� �� ����
	//
	// ���÷� ȣ�� ����,
	/////////////////////////////////////////////////////////
	void	SetLineColor(int iWeight, COLORREF BackColor);


	/////////////////////////////////////////////////////////
	// ������ ���ν���
	/////////////////////////////////////////////////////////
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	/////////////////////////////////////////////////////////
	// ������ �ֱ�.
	/////////////////////////////////////////////////////////
	//void	InsertData(int iData);
	void	InsertData(ULONG64 u64ServerID, int iType,  int iData);


protected:

	/////////////////////////////////////////////////////////////
	// ������ ���� �� �ı�, ���� ȣ���
	/////////////////////////////////////////////////////////////
	BOOL	CreateGraphWindow(void);
	void	DestroyGraphWindow(void);

	/////////////////////////////////////////////////////////////
	// ������ �ڵ�, this ������ ��Ī ���̺� ����.
	//
	// ������ ExtraData ������� ������
	/////////////////////////////////////////////////////////////
	BOOL						PutThis(void);
	static BOOL					RemoveThis(HWND hWnd);
	static CMonitorGraphUnit	*GetThis(HWND hWnd);

public:
	/////////////////////////////////////////////////////////////
	// ����ġ �˶��߻�.
	/////////////////////////////////////////////////////////////
	BOOL	Alert(void);
protected:

	/////////////////////////////////////////////////////////////
	// Ÿ�Ժ� Paint �Լ�
	/////////////////////////////////////////////////////////////
	void	Paint_Title(void);
	void	Paint_Grid(void);
	void	Paint_Index(void);	// Only LineMulti


	void	Paint_BarSingleVert(void);
	void	Paint_BarSingleHorz(void);
	void	Paint_BarColumnVert(void);
	void	Paint_BarColumnHorz(void);
	void	Paint_LineSingle(void);
	void	Paint_LineMulti(void);
	void	Paint_Pie(void);

	////////////////////////////////////////////////////////////
	// �޸�DC ȭ�� �����
	////////////////////////////////////////////////////////////
	void	ClearMemDC(void);

	/////////////////////////////////////////////////////////////
	// �޸�DC ȭ������ ��ȯ
	////////////////////////////////////////////////////////////
	void	FlipMemDC(HDC hDC);

private:

	//------------------------------------------------------
	// ������ �ڵ�, �ν��Ͻ� �ڵ�
	//------------------------------------------------------
	HWND		_hWndParent;
	HWND		_hWnd;
	HINSTANCE	_hInstance;
	WCHAR		_szWindowClass[30];

	//------------------------------------------------------
	// ������ ��ġ,ũ��,���� 
	//------------------------------------------------------
	COLORREF	_BackColor;
	int			_iWindowPosX;
	int			_iWindowPosY;
	int			_iWindowWidth;
	int			_iWindowHeight;

	TYPE		_enGraphType;

	//------------------------------------------------------
	// GDI ���ҽ���
	//------------------------------------------------------
	HDC			_hMemDC;		// ���� ���۸��� �޸� DC
	HBITMAP		_hMemBitmap;
	HBITMAP		_hMemBitmapOld;

	HBRUSH		_hBackBrush;	// ȭ�� ������ �귯��

	HFONT		_hFontTitle;	// Ÿ��Ʋ ��Ʈ
	HFONT		_hFontGrid;		// �׷��� ��ǥ ��Ʈ

	HPEN		_hGridPen;		// �׸��� �׸��� ��
	HPEN		_hLinePen;		// �׷��� �׸��� �� *hLinePenArray
								// *PieBrushArray


	RECT		_MemDCRect;		// �޸�DC ������




	//------------------------------------------------------
	// ����â �̸�
	//------------------------------------------------------
	WCHAR			_szTitle[32];

	//------------------------------------------------------
	// ������, �ְ�ġ, �溸 ��ġ
	//------------------------------------------------------
	stTYPEtoID		_Data;

	int				_iColumnCnt;			// â�� ǥ���� �׷��� ����
	int				_iDataMax;
	int				_iDataAlert;

	//------------------------------------------------------
	// ����� - 
	//
	// ������ ����ġ ���޽� ������� �Բ� �ڽ��� ������ ��� �����쿡 UM_ALERT �޽����� ������.
	//
	// _bAlertMode = TRUE �� �� �� Ÿ�̸Ӹ� ����Ͽ� �����ð� �� ����.
	//------------------------------------------------------
	BOOL			_bAlertMode;




	
	//------------------------------------------------------
	// ������ ���ν����� static �� ����� ��� ���ϵ� �����찡
	// ���� static ���ν����� ����Ѵ�. this �� ���� ����� �����Ƿ�,
	// stHWNDtoTHIS ����ü�� �迭�� ���� HWND <-> this ��Ī ���̺��� ����Ѵ�.
	//
	// ������ ������ ���� ����.
	//
	// WindowLongPtr ������� ������
	//------------------------------------------------------
	static stHWNDtoTHIS	_HWND_This;	

};






#endif