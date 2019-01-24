#include "stdafx.h"
#include "170607_Winapi_2D_Game.h"

HWND g_hWnd;
HINSTANCE g_hInst;         
WCHAR szTitle[100];                  
WCHAR szWindowClass[100];     
HIMC g_hOldIMC;

ObjectList<CBaseObject*> _lst;
int g_iState = 1;
bool g_bActiveApp;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void InitialGame();

void Update(void);
void Update_Title(void);
void Update_Game(void);
void Action();
void Draw();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	InitialGame();

	LPCWSTR szClassName = L"winmain";     // 윈도우 클래스 이름
	LPCWSTR szTitle = L"WINAPI 2D GAME";    // 타이틀 바에 출력될 문자열
	MSG msg;
	WNDCLASSEX wcex;
	g_hInst = hInstance; 
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szClassName;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassExW(&wcex);
	g_hWnd = CreateWindowW(szClassName, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, dfSCREEN_WIDTH, dfSCREEN_HEIGHT, nullptr, nullptr, hInstance, nullptr);
	if (!g_hWnd)
	{
		DWORD err = GetLastError();
		return FALSE;
	}
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	SetFocus(g_hWnd);

	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = 640;
	WindowRect.bottom = 480;
	AdjustWindowRectEx(&WindowRect, GetWindowStyle(g_hWnd), GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));

	int iX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (640 / 2); 
	int iY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (480 / 2);
	
	MoveWindow(g_hWnd, iX, iY, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, TRUE);

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	
		{
			if (msg.message == WM_QUIT)	
				break;					
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
		}
	}
    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
    switch (message)
	{
	case WM_CREATE:
		timeBeginPeriod(1);
		g_hOldIMC = ImmAssociateContext(hWnd, NULL);
		break;
	case WM_ACTIVATEAPP:
		g_bActiveApp = (BOOL)wParam;
		break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
		timeEndPeriod(1);
		ImmAssociateContext(hWnd, g_hOldIMC);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void Update(void)
{
	switch (g_iState)
	{
	case TITLE:
		Update_Title();
		break;
	case GAME:
		Update_Game();
		break;
	}
}

void Update_Title(void)
{

}

void Update_Game (void)
{
	if (g_bActiveApp)	KeyProcess();
	
	Action();
	if (!Frame->FrameSkip(g_hWnd))
		Draw();

	g_cScreenDib.DrawBuffer(g_hWnd); 
}

void Action()
{
	//ObjectList<CBaseObject*> _lst;
	ObjectList<CBaseObject*>::iterator Object_iter;
	ObjectList<CBaseObject*>::iterator Object_Inneriter;
	ObjectList<CBaseObject*>::Node* pNode;
	CBaseObject* pTempData;

	for (Object_iter = _lst.begin(); Object_iter != _lst.end(); ++Object_iter)
	{
		(*Object_iter)->Action();
	}

	for (Object_iter = _lst.begin(); Object_iter != _lst.end(); ++Object_iter)
	{
		for (Object_Inneriter = _lst.begin(); Object_Inneriter != Object_iter; ++Object_Inneriter)
		{
			if ((*Object_iter)->GetObjectType() != eTYPE_EFFECT)					// 이펙트는 뒤로 밀고
			{
				if ((*Object_iter)->GetCurY() < (*Object_Inneriter)->GetCurY())		// Y좌표를 기준으로 정렬
				{
					pTempData = *Object_iter;
					*Object_iter = *Object_Inneriter;
					*Object_Inneriter = pTempData;
				}
			}
		}
	}

	for (Object_iter = _lst.begin(); Object_iter != _lst.end(); ++Object_iter)		// 이펙트 지우기
	{
		if ((*Object_iter)->GetObjectType() == eTYPE_EFFECT)
		{
			if ((*Object_iter)->IsEndFrame())
			{
				pNode = Object_iter.getNode();
				delete pNode->_Data;
				_lst.erase(Object_iter);
				break;
			}
		}
	}
}

void Draw()
{
	ObjectList<CBaseObject*>::iterator Object_iter;

	BYTE *bypDest = g_cScreenDib.GetDibBuffer();
	int iDestWidth = g_cScreenDib.GetWidth();
	int iDestHeight = g_cScreenDib.GetHeight();
	int iDestPitch = g_cScreenDib.GetPitch();

	g_cSprite.DrawImage(eMAP, 0, 0, bypDest, iDestWidth, iDestHeight, iDestPitch);

	for (Object_iter = _lst.begin(); Object_iter != _lst.end(); ++Object_iter)
	{
		(*Object_iter)->Draw(bypDest, iDestWidth, iDestHeight, iDestPitch);
	}
}

void InitialGame()
{
	g_cSprite.LoadDibSprite(eMAP, L"Data\\_Map.bmp", 0, 0);

	g_cSprite.LoadDibSprite(ePLAYER_STAND_L01, L"Data\\Stand_L_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L02, L"Data\\Stand_L_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L03, L"Data\\Stand_L_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L04, L"Data\\Stand_L_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L05, L"Data\\Stand_L_01.bmp", 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_STAND_R01, L"Data\\Stand_R_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R02, L"Data\\Stand_R_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R03, L"Data\\Stand_R_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R04, L"Data\\Stand_R_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R05, L"Data\\Stand_R_01.bmp", 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L01, L"Data\\Move_L_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L02, L"Data\\Move_L_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L03, L"Data\\Move_L_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L04, L"Data\\Move_L_04.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L05, L"Data\\Move_L_05.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L06, L"Data\\Move_L_06.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L07, L"Data\\Move_L_07.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L08, L"Data\\Move_L_08.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L09, L"Data\\Move_L_09.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L10, L"Data\\Move_L_10.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L11, L"Data\\Move_L_11.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L12, L"Data\\Move_L_12.bmp", 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R01, L"Data\\Move_R_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R02, L"Data\\Move_R_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R03, L"Data\\Move_R_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R04, L"Data\\Move_R_04.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R05, L"Data\\Move_R_05.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R06, L"Data\\Move_R_06.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R07, L"Data\\Move_R_07.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R08, L"Data\\Move_R_08.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R09, L"Data\\Move_R_09.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R10, L"Data\\Move_R_10.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R11, L"Data\\Move_R_11.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R12, L"Data\\Move_R_12.bmp", 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L01, L"Data\\Attack1_L_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L02, L"Data\\Attack1_L_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L03, L"Data\\Attack1_L_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L04, L"Data\\Attack1_L_04.bmp", 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R01, L"Data\\Attack1_R_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R02, L"Data\\Attack1_R_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R03, L"Data\\Attack1_R_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R04, L"Data\\Attack1_R_04.bmp", 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L01, L"Data\\Attack2_L_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L02, L"Data\\Attack2_L_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L03, L"Data\\Attack2_L_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L04, L"Data\\Attack2_L_04.bmp", 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R01, L"Data\\Attack2_R_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R02, L"Data\\Attack2_R_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R03, L"Data\\Attack2_R_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R04, L"Data\\Attack2_R_04.bmp", 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L01, L"Data\\Attack3_L_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L02, L"Data\\Attack3_L_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L03, L"Data\\Attack3_L_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L04, L"Data\\Attack3_L_04.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L05, L"Data\\Attack3_L_05.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L06, L"Data\\Attack3_L_06.bmp", 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R01, L"Data\\Attack3_R_01.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R02, L"Data\\Attack3_R_02.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R03, L"Data\\Attack3_R_03.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R04, L"Data\\Attack3_R_04.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R05, L"Data\\Attack3_R_05.bmp", 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R06, L"Data\\Attack3_R_06.bmp", 71, 90);
	
	g_cSprite.LoadDibSprite(eEFFECT_SPARK_01, L"Data\\xSpark_1.bmp", 70, 70);
	g_cSprite.LoadDibSprite(eEFFECT_SPARK_02, L"Data\\xSpark_2.bmp", 70, 70);
	g_cSprite.LoadDibSprite(eEFFECT_SPARK_03, L"Data\\xSpark_3.bmp", 70, 70);
	g_cSprite.LoadDibSprite(eEFFECT_SPARK_04, L"Data\\xSpark_4.bmp", 70, 70);

	g_cSprite.LoadDibSprite(eGUAGE_HP, L"Data\\HPGuage.bmp", 0, 0);
	g_cSprite.LoadDibSprite(eSHADOW, L"Data\\Shadow.bmp", 32, 4);
	
	Frame = new CFrameSkip();

	g_pPlayer = new CPlayer(100, 100, TRUE);
	_lst.push_front((CBaseObject*)g_pPlayer);


	CBaseObject *pObject;
	pObject = new CPlayer(300, 300);
	_lst.push_front((CBaseObject*)pObject);

	pObject = new CPlayer(400, 200);
	_lst.push_front((CBaseObject*)pObject);

	pObject = new CPlayer(200, 400);
	_lst.push_front((CBaseObject*)pObject);
	
	pObject = new CEffectObject(300, 400);
	_lst.push_front((CBaseObject*)pObject);

	pObject = new CEffectObject(300, 400);
	_lst.push_front((CBaseObject*)pObject);
}