#ifndef __MONITOR_UNIT__
#define __MONITOR_UNIT__
#include "CQueue.h"
#include "170529_Win32_TimeGraph.h"

//////////////////////////////////////////////////////////////////////
// 모니터링 툴 제작시 사용되는 그래프 유닛 윈도우 클래스
//
// 
//////////////////////////////////////////////////////////////////////
//------------------------------------------------------
// 경고시 발생 윈도우 메시지
//------------------------------------------------------
#define UM_ALERT					WM_USER + 1

#define dfALERT_CANCEL_TIMER_ID		200
#define dfALERT_CANCEL_TIME			500

//------------------------------------------------------
// 최대 생성가능 윈도우
//------------------------------------------------------
#define dfMAXCHILD			50

//------------------------------------------------------
// 폰트사이즈, 타이틀 / 그래프지표
//------------------------------------------------------
#define dfFONT_TITLE_SIZE	20
#define dfFONT_POINT_SIZE	12
#define dfFONT_INDEX_SIZE	18

//------------------------------------------------------
// 타이틀 표시영역 높이
//------------------------------------------------------
#define dfTITLE_HEIGHT		30

//------------------------------------------------------
// 세로축 지표 개수
//------------------------------------------------------
#define dfGRID_VERT_NUM		4

//------------------------------------------------------
// 컬럼 최대 개수
//------------------------------------------------------
#define dfMAXCOLUMN	30


class CMonitorGraphUnit
{
public:

	enum TYPE
	{
		BAR_SINGLE_VERT,		// 바 하나 세로
		BAR_SINGLE_HORZ,		// 바 하나 가로
		BAR_COLUMN_VERT,		// 바 여러개 세로
		BAR_COLUMN_HORZ,		// 바 여러개 가로
		LINE_SINGLE,			// 라인 그래프 하나
		LINE_MULTI,				// 라인 그래프 여러개
		PIE						// 원 파이 나누기
	};

	typedef struct ST_HWNDtoTHIS
	{
		HWND				hWnd[dfMAXCHILD];
		CMonitorGraphUnit	*pThis[dfMAXCHILD];
	}stHWNDtoTHIS;

	typedef struct ST_TYPEtoID
	{
		ULONG64				u64ServerID[dfMAXCOLUMN];	// 서버 ID
		WCHAR				szName[dfMAXCOLUMN][32];	// 서버 이름
		int					iType[dfMAXCOLUMN];			// 데이터 종류
		CQueue<int>			*Queue[dfMAXCOLUMN];		// 데이터 값
	}stTYPEtoID;


public:

	CMonitorGraphUnit(HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight, int iColumnCnt);
	~CMonitorGraphUnit();

	/////////////////////////////////////////////////////////
	// 그래프 정보 입력
	//
	// 윈도우 이름, 최대치, 경보수치 (0이면 없음)
	/////////////////////////////////////////////////////////
	void	SetInformation(WCHAR *szTitle, int iDataMax, int iDataAlert);
	

	/////////////////////////////////////////////////////////
	// 칼럼 정보 입력
	//
	// 서버 ID, 데이터 종류, 서버 이름
	/////////////////////////////////////////////////////////
	void	SetDataColumnInfo(ULONG64 u64ServerID, int iType, WCHAR *szName); 


	/////////////////////////////////////////////////////////
	// 그래프 라인 선 지정
	//
	// 수시로 호출 가능,
	/////////////////////////////////////////////////////////
	void	SetLineColor(int iWeight, COLORREF BackColor);


	/////////////////////////////////////////////////////////
	// 윈도우 프로시저
	/////////////////////////////////////////////////////////
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	/////////////////////////////////////////////////////////
	// 데이터 넣기.
	/////////////////////////////////////////////////////////
	//void	InsertData(int iData);
	void	InsertData(ULONG64 u64ServerID, int iType,  int iData);


protected:

	/////////////////////////////////////////////////////////////
	// 윈도우 생성 및 파괴, 내부 호출용
	/////////////////////////////////////////////////////////////
	BOOL	CreateGraphWindow(void);
	void	DestroyGraphWindow(void);

	/////////////////////////////////////////////////////////////
	// 윈도우 핸들, this 포인터 매칭 테이블 관리.
	//
	// 윈도우 ExtraData 사용으로 없어짐
	/////////////////////////////////////////////////////////////
	BOOL						PutThis(void);
	static BOOL					RemoveThis(HWND hWnd);
	static CMonitorGraphUnit	*GetThis(HWND hWnd);

public:
	/////////////////////////////////////////////////////////////
	// 경고수치 알람발생.
	/////////////////////////////////////////////////////////////
	BOOL	Alert(void);
protected:

	/////////////////////////////////////////////////////////////
	// 타입별 Paint 함수
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
	// 메모리DC 화면 지우기
	////////////////////////////////////////////////////////////
	void	ClearMemDC(void);

	/////////////////////////////////////////////////////////////
	// 메모리DC 화면으로 전환
	////////////////////////////////////////////////////////////
	void	FlipMemDC(HDC hDC);

private:

	//------------------------------------------------------
	// 윈도우 핸들, 인스턴스 핸들
	//------------------------------------------------------
	HWND		_hWndParent;
	HWND		_hWnd;
	HINSTANCE	_hInstance;
	WCHAR		_szWindowClass[30];

	//------------------------------------------------------
	// 윈도우 위치,크기,색상 
	//------------------------------------------------------
	COLORREF	_BackColor;
	int			_iWindowPosX;
	int			_iWindowPosY;
	int			_iWindowWidth;
	int			_iWindowHeight;

	TYPE		_enGraphType;

	//------------------------------------------------------
	// GDI 리소스들
	//------------------------------------------------------
	HDC			_hMemDC;		// 더블 버퍼링용 메모리 DC
	HBITMAP		_hMemBitmap;
	HBITMAP		_hMemBitmapOld;

	HBRUSH		_hBackBrush;	// 화면 지우기용 브러쉬

	HFONT		_hFontTitle;	// 타이틀 폰트
	HFONT		_hFontGrid;		// 그래프 지표 폰트

	HPEN		_hGridPen;		// 그리드 그리기 펜
	HPEN		_hLinePen;		// 그래프 그리기 펜 *hLinePenArray
								// *PieBrushArray


	RECT		_MemDCRect;		// 메모리DC 사이즈




	//------------------------------------------------------
	// 정보창 이름
	//------------------------------------------------------
	WCHAR			_szTitle[32];

	//------------------------------------------------------
	// 데이터, 최고치, 경보 수치
	//------------------------------------------------------
	stTYPEtoID		_Data;

	int				_iColumnCnt;			// 창에 표시할 그래프 갯수
	int				_iDataMax;
	int				_iDataAlert;

	//------------------------------------------------------
	// 경고모드 - 
	//
	// 데이터 경고수치 도달시 경고음과 함께 자신을 제외한 모든 윈도우에 UM_ALERT 메시지를 보낸다.
	//
	// _bAlertMode = TRUE 가 된 후 타이머를 사용하여 일정시간 후 해제.
	//------------------------------------------------------
	BOOL			_bAlertMode;




	
	//------------------------------------------------------
	// 윈도우 프로시저가 static 인 관계로 모든 차일드 윈도우가
	// 같은 static 프로시저를 사용한다. this 를 얻을 방법이 없으므로,
	// stHWNDtoTHIS 구조체의 배열을 통해 HWND <-> this 매칭 테이블을 사용한다.
	//
	// 윈도우 생성시 정보 셋팅.
	//
	// WindowLongPtr 사용으로 삭제함
	//------------------------------------------------------
	static stHWNDtoTHIS	_HWND_This;	

};






#endif