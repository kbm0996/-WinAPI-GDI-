#include "stdafx.h"
#include "Windows.h"
#include "ScreenDib.h"
#include "DefineEnum.h"

CScreenDib g_cScreenDib(dfSCREEN_WIDTH, dfSCREEN_HEIGHT, dfSCREEN_BIT);

CScreenDib::CScreenDib(int iWidth, int iHeight, int iColorBit)
{
	memset(&m_stDibInfo, 0, sizeof(BITMAPINFO));
	m_bypBuffer = NULL;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iPitch = 0;
	m_iBufferSize = 0;
	m_iColorBit = 0;
	CreateDibBuffer(iWidth, iHeight, iColorBit);
}

CScreenDib::~CScreenDib()
{
	ReleaseDibBuffer();
}

void CScreenDib::CreateDibBuffer(int iWidth, int iHeight, int iColorBit)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iColorBit = iColorBit;
	m_iPitch = ((m_iWidth * (m_iColorBit / 8)) + 3) & ~3;
	m_iBufferSize = m_iPitch * m_iHeight;

	m_stDibInfo.biSize = sizeof(BITMAPINFOHEADER);
	m_stDibInfo.biWidth = m_iWidth;
	m_stDibInfo.biHeight = -m_iHeight;
	m_stDibInfo.biPlanes = 1;
	m_stDibInfo.biBitCount = m_iColorBit;
	m_stDibInfo.biCompression = 0;
	m_stDibInfo.biSizeImage = m_stDibInfo.biSizeImage;
	m_stDibInfo.biXPelsPerMeter = 0;
	m_stDibInfo.biYPelsPerMeter = 0;
	m_stDibInfo.biClrUsed = 0;
	m_stDibInfo.biClrImportant = 0;
	
	m_bypBuffer = new BYTE[m_iBufferSize];
	memset(m_bypBuffer, 0xff, m_iBufferSize);
}

void CScreenDib::ReleaseDibBuffer()
{
	m_iWidth = 0;
	m_iHeight = 0;
	m_iPitch = 0;
	m_iBufferSize = 0;
	
	memset(&m_stDibInfo, 0x00, sizeof(BITMAPINFO));

	if (m_bypBuffer != NULL)
		delete[] m_bypBuffer;

	m_bypBuffer = NULL;
}

void CScreenDib::DrawBuffer(HWND hWnd, int iX, int iY)
{
	if (m_bypBuffer == NULL) return;

	RECT crt;
	HDC hdc;

	GetClientRect(hWnd, &crt);
	hdc = GetDC(hWnd);

	//StretchDIBits(hdc, iX, iY, m_iPitch, m_iHeight, 0, 0, m_iPitch, m_iHeight, m_bypBuffer, (LPBITMAPINFO)(&m_stDibInfo), DIB_RGB_COLORS, SRCCOPY);
	SetDIBitsToDevice(hdc, iX, iY, m_iWidth, m_iHeight, 0, 0, 0, m_iHeight, m_bypBuffer, (LPBITMAPINFO)(&m_stDibInfo), DIB_RGB_COLORS);
	ReleaseDC(hWnd, hdc);
}

BYTE * CScreenDib::GetDibBuffer()
{
	return m_bypBuffer;
}

int CScreenDib::GetWidth()
{
	return m_iWidth;
}

int CScreenDib::GetHeight()
{
	return m_iHeight;
}

int CScreenDib::GetPitch(void)
{
	return m_iPitch;
}
