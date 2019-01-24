#include "stdafx.h"
#include "170605_OpenBitmap.h"
#include <cstdio>

#define MAX_LOADSTRING 100

// ���� ����:
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

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	hInst = hInstance;

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
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
            // �޴� ������ ���� �м��մϴ�.
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
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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
		MessageBox(NULL, L"���� ����� ���� �� �����ϴ�.", NULL, MB_OK);
		fclose(pFile);
		return;
	}
	Read = fread(&stInfoHeader, sizeof(stInfoHeader), 1, pFile);
	if (Read != 1)
	{
		MessageBox(NULL, L"���� ����� ���� �� �����ϴ�.", NULL, MB_OK);
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
		pBuffer += iPitch;	// pitch��ŭ ������
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
	delete[] pImageT;	// ������ �̵����ѳ��� �����ϸ� ���� �߻�
	return;
}


//@. �������� Char a = 11001100 (������)
// a >> 2 ������ �ϸ� 00110011�� �ƴ϶� �ְ��Ʈ�� ������ �ǹ��ϴ� 1�̹Ƿ� 11110011�� �ȴ�.
// ������ �߸� ����ɱ�� ��Ʈ ���� �ÿ��� unsigned�� ���� ���̴�.


// pitch ���� ���� ������ ������?


void Bitmap()
{
	size_t Read;
	FILE *pFile;
	_wfopen_s(&pFile, L"test.bmp", L"rb");
	if (pFile == NULL)
	{
		MessageBox(NULL, L"������ ã�� �� �����ϴ�.", NULL, MB_OK);
		fclose(pFile);
		return;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	BMP ������ �ֻ���� ���������� ���ϴ�. �̴� DIB �� ������ �ƴ� BMP ���� ��ü�� ������ DIB ���������� �ʿ����� �ʴ� �����Դϴ�.
	//
	//typedef struct tagBITMAPFILEHEADER {		// 14 = 2+4+2+2+4 (byte)
	//	WORD    bfType;							// "BM"�̶�� ���ڰ� ������
	//	DWORD   bfSize;							// ��Ʈ�� ������ ��ü ũ��
	//	WORD    bfReserved1;					// ���ຯ��(0���� ������)
	//	WORD    bfReserved2;					// ���ຯ��(0���� ������)
	//	DWORD   bfOffBits;						// ���Ͽ��� ��Ʈ�� �����Ͱ� �ִ� ��ġ = �̹����� ���۵Ǵ� Offset ��ġ
	//} BITMAPFILEHEADER;
	//
	//  BMP ������ �����Ͽ� ���� ���� BITMAPFILEHEADER�� �а� bfType ���� ���Ͽ� BM(0x4d42, ��Ʋ-�����) ���� �´��� Ȯ���մϴ�. ���� �տ� BM�� ���ٸ� BMP ������ �ƴ� ������ �Ǵ��� �� �ֽ��ϴ�.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	BITMAPFILEHEADER stFileHeader;
	Read = fread(&stFileHeader, sizeof(stFileHeader), 1, pFile);
	if (Read != 1)
	{
		MessageBox(NULL, L"���� ����� ���� �� �����ϴ�.", NULL, MB_OK);
		fclose(pFile);
		return;
	}

	if (stFileHeader.bfType != 0x4d42)
	{
		MessageBox(NULL, L"BMP ������ �ƴմϴ�.", NULL, MB_OK);
		fclose(pFile);
		return;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	�̰� ������ DIB��� ���� �����մϴ�.
	//
	//	typedef struct tagBITMAPINFOHEADER {	// 40 = 4+4+4+2+2+4+4+4+4+4+4 (byte)
	//		DWORD biSize;						// �ش� ����ü�� ũ��
	//		LONG biWidth;						// �ȼ� �ʺ� (����Ʈ�� �ƴ�)
	//		LONG biHeight;						// �ȼ� ����
	//		WORD biPlanes;						// Plane��(1�� ������)
	//		WORD biBitCount;					// �� �ȼ��� ��Ʈ�� ( 8 : 256, 16 : 65536, 24, 32 )
	//		DWORD biCompression;				// ���� ���� �÷���, ���� ��� BI_RGB(0) ��Ʈ���� ������ ���� �ʴ� ���� �⺻
	//		DWORD biSizeImage;					// �̹��� ũ��, �������� ��� ������ ����
	//		LONG biXPelsPerMeter;				// ���̽� ���ʹ� �ȼ� ��  (����)
	//		LONG biYPelsPerMeter;				// ����̽� ���ʹ� �ȼ� ��  (����)
	//		DWORD biClrUsed;					// �׸����� ���� ��� ���� �� 0 �̸� ��Į��
	//		DWORD biClrImportant;				// �ַ� ���� ���� �� 0 �̸� ��Į��
	//	} BITMAPINFOHEADER;
	//
	//  BITMAPINFOHEADER ������ �о BMP ������ ������ Ȯ�� �� �� �ֽ��ϴ�. �� �����ͺ��� DIB �� �ڷ� �Դϴ�. 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAPINFOHEADER stInfoHeader;
	Read = fread(&stInfoHeader, sizeof(stInfoHeader), 1, pFile);
	if (Read != 1)
	{
		MessageBox(NULL, L"���� ����� ���� �� �����ϴ�.", NULL, MB_OK);
		fclose(pFile);
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////## �ȷ�Ʈ
	////	�ȷ�Ʈ�� �ε����� ���� Į���� �����ϱ� ���� ����ü�̴�.�� ����ü�� ���, �ȷ�Ʈ ����ŭ �迭�� �Ҵ�, �����Ѵ�. 256 Į������ ��� �ȷ�Ʈ�迭 ũ��� 256�� �ǰ�, 16��Ʈ Į�󿵻��� ��� 2 ^ 16 ���� �ȷ�Ʈ�迭 ũ�⸦ ������ �ȴ�.
	////		typedef struct tagRGBQUAD { // 4
	////		BYTE rgbBlue;  // B���� (�Ķ�)
	////		BYTE rgbGreen; // G���� (�ʷ�)
	////		BYTE rgbRed;   // R���� (����)
	////		BYTE rgbReserved;
	////	} RGBQUAD;
	////	typedef struct tagBITMAPINFO {
	////		BITMAPINFOHEADER bmiHeader;
	////		RGBQUAD bmiColors[1];
	////	} BITMAPINFO;
	////
	////	8bit �÷��� ��� �ȷ�Ʈ �����͸� ����մϴ�.  RGBQUAD ���� �緹Ʈ �������̸�, 256 Į���� RGBQUAD �����Ͱ� 256�� �����մϴ�. /������ 8bit ������ ���� ������� �����Ƿ� ���� �մϴ�.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//RGBQUAD stBGRQuad[256];
	//Read = fread(stBGRQuad, sizeof(stBGRQuad), 1, pFile);
	//if (Read != 1)
	//{
	//	MessageBox(NULL, L"���� ���̺��� ���� �� �����ϴ�.", NULL, MB_OK);
	//	fclose(pFile);
	//	return;
	//}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���� �̹����� ũ�� ��ŭ �ȼ� �����Ͱ� ��� �ֽ��ϴ�.DIB �� Ư¡�� ������ �����ϴ�.
	//	�̹����� ���Ʒ� �������� ����ִ�.
	//	�ٴ����� 4Byte ������ �Ǿ��ִ�.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LONG WidthByte = stInfoHeader.biWidth * (stInfoHeader.biBitCount / 8);	// �� �ٴ� ���� �ȼ� ����, stInfoHeader.biBitCount / 8 

	int Pitch = (WidthByte + 3) & ~3;	// 4byte ����, 3(������ 011) �÷��� 4byte ������ ���� ���� �÷��� 3(������ 011)�� ����
	int ImageSize = Pitch * stInfoHeader.biHeight;

	BYTE *pImage = new BYTE[ImageSize];

	fseek(pFile, stFileHeader.bfOffBits, SEEK_SET);
	Read = fread(pImage, ImageSize, 1, pFile);
	if (Read != 1)
	{
		MessageBox(NULL, L"�̹����� ���� �� �����ϴ�.", NULL, MB_OK);
		fclose(pFile);
		return;
	}

	fclose(pFile);

	// �̹��� ����
	BYTE *pTemp = pImage;
	BYTE *pImageOrigin = pImage;

	int iCntHeight, iCntWidth;



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  �̹����� �������� ������ �������� �̹����� DIB�� ���� �����̹Ƿ�, �������� ä�� �о DIB ����� ���ָ� �׸��� ��µ˴ϴ�. 
	// DIB �� DC �� ����ϴ� �Լ��� StretchDIBits �Լ� �Ǵ� SetDIBitsToDevice �� ����ϰ� �˴ϴ�.
	//
	//int StretchDIBits(
	//	_In_  HDC hdc,						// ������ DC
	//	_In_  int XDest,					// ��� ��� X ��ġ
	//	_In_  int YDest,					// ��� ��� Y ��ġ
	//	_In_  int nDestWidth,				// ������ �ʺ�
	//	_In_  int nDestHeight,				// ������ ����
	//	_In_  int XSrc,						// ��� �ҽ� X ��ġ
	//	_In_  int YSrc,						// ��� �ҽ� Y ��ġ
	//	_In_  int nSrcWidth,				// ��� �ҽ� �ʺ�
	//	_In_  int nSrcHeight,				// ��� �ҽ� ����
	//	_In_  const VOID *lpBits,			// �̹����� ��ġ�� ������
	//	_In_  const BITMAPINFO *lpBitsInfo,	// BITMAPINFO ����ü�� ��ġ / BITMAPINFO �� BITMAPINFOHEADER + RGBQUAD �� ������ ����ü
	//	_In_  UINT iUsage,					// 8byte ��忡���� ���� �ȷ�Ʈ�� �� DIB_RGB_COLORS	
	//	_In_  DWORD dwRop					// ��¸�� SRCCOPY
	//);
	//
	//  SetDIBitsToDevice �Լ��� ���� ����ϸ� �������� Ȯ�� Ȥ�� ��� ����� �����ϴ��� ���� �����Դϴ�.
	// �ܼ� 1:1 ��¸��� �ϰ��� �Ѵٸ� SetDIBitsToDevice�� ����ϴ� ���� �� ������ ���ڰ� �ܼ��մϴ�. 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	StretchDIBits(hdc, 0, 0, Pitch, stInfoHeader.biHeight, 0, 0, Pitch, stInfoHeader.biHeight, pImage, (LPBITMAPINFO)(&stInfoHeader), DIB_RGB_COLORS, SRCCOPY);

	delete[] pImage;
	return;
}
