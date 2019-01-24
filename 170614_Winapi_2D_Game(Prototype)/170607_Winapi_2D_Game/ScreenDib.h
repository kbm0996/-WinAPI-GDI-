#ifndef __SCREEN_DIB__
#define __SCREEN_DIB__

#include "stdafx.h"

class CScreenDib
{
public:
	CScreenDib(int iWidth, int iHeight, int iColorBit);
	virtual ~CScreenDib();

protected:
	void CreateDibBuffer(int iWidth, int iHeight, int iColorBit);
	void ReleaseDibBuffer();

public:
	void DrawBuffer(HWND hWnd, int iX = 0, int iY = 0);

	BYTE *GetDibBuffer();
	int GetWidth();
	int GetHeight();
	int GetPitch(void);

protected:
	BITMAPINFOHEADER	m_stDibInfo;
	BYTE				*m_bypBuffer;

	int					m_iWidth;
	int					m_iHeight;
	int					m_iPitch;
	int					m_iColorBit;
	int					m_iBufferSize;
};

extern CScreenDib g_cScreenDib;

#endif