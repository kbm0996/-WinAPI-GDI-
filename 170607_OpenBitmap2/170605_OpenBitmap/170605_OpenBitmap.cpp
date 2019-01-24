#include "stdafx.h"
#include "170605_OpenBitmap.h"
#include <cstdio>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];       
WNDCLASSEXW wcex;
HWND hWnd;
HDC hdc;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void				Bitmap();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	hInst = hInstance;

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY170605_OPENBITMAP, szWindowClass, MAX_LOADSTRING);

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY170605_OPENBITMAP));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY170605_OPENBITMAP);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);


	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


    return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
		    hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
			Bitmap();

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void BitmapDraw(BYTE *pBuffer, int iWidth, int iHeight, int iPitch, int iX, int iY)
{
	size_t Read;
	FILE *pFile;
	BITMAPFILEHEADER stFileHeader;
	BITMAPINFOHEADER stInfoHeader;

	BYTE *pImage;
	LONG WidthByte;
	int Pitch;
	int ImageSize;
	
	_wfopen_s(&pFile, L"test.bmp", L"rb");

	Read = fread(&stFileHeader, sizeof(stFileHeader), 1, pFile);
	if (Read != 1)
	{
		MessageBox(NULL, L"파일 헤더를 읽을 수 없습니다.", NULL, MB_OK);
		fclose(pFile);
		return;
	}
	Read = fread(&stInfoHeader, sizeof(stInfoHeader), 1, pFile);
	if (Read != 1)
	{
		MessageBox(NULL, L"정보 헤더를 읽을 수 없습니다.", NULL, MB_OK);
		fclose(pFile);
		return;
	}

	WidthByte = stInfoHeader.biWidth * (stInfoHeader.biBitCount / 8);	
	Pitch = (WidthByte + 3) & ~3;	
	ImageSize = Pitch * stInfoHeader.biHeight;
	pImage = new BYTE[ImageSize];
	fseek(pFile, stFileHeader.bfOffBits, SEEK_SET);
	fread(pImage, ImageSize, 1, pFile);

	fclose(pFile);

	pBuffer = pBuffer + (iX * 4) + (iPitch * iY);
	BYTE* pImageT = pImage;
	DWORD* dwpBuffer;//
	DWORD* dwpImage;//
	for (int iCnt = 0; iCnt < stInfoHeader.biHeight; ++iCnt)
	{
		memcpy(pBuffer, pImage, WidthByte);
		pBuffer += iPitch;	// pitch만큼 내려감
		pImage += Pitch;

	}

	for (int iCnt = 0; iCnt < stInfoHeader.biHeight; ++iCnt)
	{
		for (int iCntW = 0; iCntW < stInfoHeader.biHeight; ++iCntW)
		{
			if (*dwpImage != 0xffffffff)
			{
				*dwpBuffer = *dwpImage;
			}
			dwpBuffer++;
			dwpImage++;
		}
		dwpBuffer = (DWORD*)pBuffer;
		dwpImage = (DWORD*)pImage;
	}
	delete[] pImageT;	// 포인터 이동시켜놓고 삭제하면 에러 발생
	return;
}


//@. 면접문제 Char a = 11001100 (이진수)
// a >> 2 연산을 하면 00110011이 아니라 최고비트가 음수를 의미하는 1이므로 11110011이 된다.
// 때문에 잘못 변경될까봐 비트 연산 시에는 unsigned를 쓰는 것이다.


// pitch 값을 쓰는 이유가 뭐였지?


void Bitmap()
{
	size_t Read;
	FILE *pFile;
	_wfopen_s(&pFile, L"test.bmp", L"rb");
	if (pFile == NULL)
	{
		MessageBox(NULL, L"파일을 찾을 수 없습니다.", NULL, MB_OK);
		fclose(pFile);
		return;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	BMP 파일의 최상단은 파일정보가 들어갑니다. 이는 DIB 의 정보가 아닌 BMP 파일 자체의 정보로 DIB 구조에서는 필요하지 않는 정보입니다.
	//
	//typedef struct tagBITMAPFILEHEADER {		// 14 = 2+4+2+2+4 (byte)
	//	WORD    bfType;							// "BM"이라는 글자가 설정됨
	//	DWORD   bfSize;							// 비트맵 파일의 전체 크기
	//	WORD    bfReserved1;					// 예약변수(0으로 설정함)
	//	WORD    bfReserved2;					// 예약변수(0으로 설정함)
	//	DWORD   bfOffBits;						// 파일에서 비트맵 데이터가 있는 위치 = 이미지가 시작되는 Offset 위치
	//} BITMAPFILEHEADER;
	//
	//  BMP 파일을 오픈하여 가장 먼저 BITMAPFILEHEADER를 읽고 bfType 값을 비교하여 BM(0x4d42, 리틀-엔디언) 값이 맞는지 확인합니다. 가장 앞에 BM이 없다면 BMP 파일이 아닌 것으로 판단할 수 있습니다.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	BITMAPFILEHEADER stFileHeader;
	Read = fread(&stFileHeader, sizeof(stFileHeader), 1, pFile);
	if (Read != 1)
	{
		MessageBox(NULL, L"파일 헤더를 읽을 수 없습니다.", NULL, MB_OK);
		fclose(pFile);
		return;
	}

	if (stFileHeader.bfType != 0x4d42)
	{
		MessageBox(NULL, L"BMP 파일이 아닙니다.", NULL, MB_OK);
		fclose(pFile);
		return;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	이게 있으면 DIB라고 봐도 무관합니다.
	//
	//	typedef struct tagBITMAPINFOHEADER {	// 40 = 4+4+4+2+2+4+4+4+4+4+4 (byte)
	//		DWORD biSize;						// 해당 구조체의 크기
	//		LONG biWidth;						// 픽셀 너비 (바이트가 아님)
	//		LONG biHeight;						// 픽셀 높이
	//		WORD biPlanes;						// Plane수(1로 설정함)
	//		WORD biBitCount;					// 한 픽셀당 비트수 ( 8 : 256, 16 : 65536, 24, 32 )
	//		DWORD biCompression;				// 압축 유무 플래그, 압축 방식 BI_RGB(0) 비트맵은 압축을 하지 않는 것이 기본
	//		DWORD biSizeImage;					// 이미지 크기, 무압축의 경우 사용되지 않음
	//		LONG biXPelsPerMeter;				// 바이스 미터당 픽셀 수  (무시)
	//		LONG biYPelsPerMeter;				// 디바이스 미터당 픽셀 수  (무시)
	//		DWORD biClrUsed;					// 그림에서 실제 사용 색상 수 0 이면 올칼라
	//		DWORD biClrImportant;				// 주로 사용된 색상 수 0 이면 올칼라
	//	} BITMAPINFOHEADER;
	//
	//  BITMAPINFOHEADER 정보를 읽어서 BMP 파일의 정보를 확인 할 수 있습니다. 이 데이터부터 DIB 의 자료 입니다. 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAPINFOHEADER stInfoHeader;
	Read = fread(&stInfoHeader, sizeof(stInfoHeader), 1, pFile);
	if (Read != 1)
	{
		MessageBox(NULL, L"정보 헤더를 읽을 수 없습니다.", NULL, MB_OK);
		fclose(pFile);
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////## 팔레트
	////	팔레트는 인덱스에 의한 칼라값을 저장하기 위한 구조체이다.이 구조체를 사용, 팔레트 수만큼 배열을 할당, 저장한다. 256 칼라모드의 경우 팔레트배열 크기는 256이 되고, 16비트 칼라영상의 경우 2 ^ 16 개의 팔레트배열 크기를 가지게 된다.
	////		typedef struct tagRGBQUAD { // 4
	////		BYTE rgbBlue;  // B성분 (파랑)
	////		BYTE rgbGreen; // G성분 (초록)
	////		BYTE rgbRed;   // R성분 (빨강)
	////		BYTE rgbReserved;
	////	} RGBQUAD;
	////	typedef struct tagBITMAPINFO {
	////		BITMAPINFOHEADER bmiHeader;
	////		RGBQUAD bmiColors[1];
	////	} BITMAPINFO;
	////
	////	8bit 컬러의 경우 팔레트 데이터를 사용합니다.  RGBQUAD 값이 펠레트 데이터이며, 256 칼라라면 RGBQUAD 데이터가 256개 존재합니다. /하지만 8bit 색상은 거의 사용하지 않으므로 생략 합니다.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//RGBQUAD stBGRQuad[256];
	//Read = fread(stBGRQuad, sizeof(stBGRQuad), 1, pFile);
	//if (Read != 1)
	//{
	//	MessageBox(NULL, L"색상 테이블을 읽을 수 없습니다.", NULL, MB_OK);
	//	fclose(pFile);
	//	return;
	//}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 이제 이미지의 크기 만큼 픽셀 데이터가 들어 있습니다.DIB 의 특징은 다음과 같습니다.
	//	이미지가 위아래 뒤집혀서 들어있다.
	//	줄단위로 4Byte 정렬이 되어있다.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LONG WidthByte = stInfoHeader.biWidth * (stInfoHeader.biBitCount / 8);	// 한 줄당 실제 픽셀 갯수, stInfoHeader.biBitCount / 8 

	int Pitch = (WidthByte + 3) & ~3;	// 4byte 정렬, 3(이진수 011) 올려서 4byte 정렬을 해준 다음 올려준 3(이진수 011)를 날림
	int ImageSize = Pitch * stInfoHeader.biHeight;

	BYTE *pImage = new BYTE[ImageSize];

	fseek(pFile, stFileHeader.bfOffBits, SEEK_SET);
	Read = fread(pImage, ImageSize, 1, pFile);
	if (Read != 1)
	{
		MessageBox(NULL, L"이미지를 읽을 수 없습니다.", NULL, MB_OK);
		fclose(pFile);
		return;
	}

	fclose(pFile);

	// 이미지 변조
	BYTE *pTemp = pImage;
	BYTE *pImageOrigin = pImage;

	int iCntHeight, iCntWidth;



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  이미지가 뒤집혀져 있지만 뒤집혀진 이미지가 DIB의 정상 구조이므로, 뒤집혀진 채로 읽어서 DIB 출력을 해주면 그림이 출력됩니다. 
	// DIB 를 DC 에 출력하는 함수는 StretchDIBits 함수 또는 SetDIBitsToDevice 를 사용하게 됩니다.
	//
	//int StretchDIBits(
	//	_In_  HDC hdc,						// 목적지 DC
	//	_In_  int XDest,					// 출력 대상 X 위치
	//	_In_  int YDest,					// 출력 대상 Y 위치
	//	_In_  int nDestWidth,				// 목적지 너비
	//	_In_  int nDestHeight,				// 목적지 높이
	//	_In_  int XSrc,						// 출력 소스 X 위치
	//	_In_  int YSrc,						// 출력 소스 Y 위치
	//	_In_  int nSrcWidth,				// 출력 소스 너비
	//	_In_  int nSrcHeight,				// 출력 소스 높이
	//	_In_  const VOID *lpBits,			// 이미지가 위치한 포인터
	//	_In_  const BITMAPINFO *lpBitsInfo,	// BITMAPINFO 구조체의 위치 / BITMAPINFO 는 BITMAPINFOHEADER + RGBQUAD 가 합쳐진 구조체
	//	_In_  UINT iUsage,					// 8byte 모드에서는 색상 팔레트가 들어감 DIB_RGB_COLORS	
	//	_In_  DWORD dwRop					// 출력모드 SRCCOPY
	//);
	//
	//  SetDIBitsToDevice 함수는 위와 비슷하며 차이점은 확대 혹은 축소 기능을 지원하는지 여부 차이입니다.
	// 단순 1:1 출력만을 하고자 한다면 SetDIBitsToDevice을 사용하는 것이 더 빠르고 인자가 단순합니다. 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	StretchDIBits(hdc, 0, 0, Pitch, stInfoHeader.biHeight, 0, 0, Pitch, stInfoHeader.biHeight, pImage, (LPBITMAPINFO)(&stInfoHeader), DIB_RGB_COLORS, SRCCOPY);

	delete[] pImage;
	return;
}
