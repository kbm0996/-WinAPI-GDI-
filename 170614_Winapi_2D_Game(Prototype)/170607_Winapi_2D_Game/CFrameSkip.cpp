#include "stdafx.h"
#include "CFrameSkip.h"
#include "Timeapi.h"

CFrameSkip* Frame;

CFrameSkip::CFrameSkip() :m_dwSystemTick(0), m_iMaxFPS(50), m_iOneFrameTick(0), m_iTick(0)
{
}

CFrameSkip::~CFrameSkip()
{
}

BOOL CFrameSkip::FrameSkip(HWND hWnd)
{
	DWORD dwCurTime;
	DWORD dwElapTime;
	WCHAR szFPS[10];
	int iFPS;
	
	dwCurTime = timeGetTime();

	m_iTick++;
	dwElapTime = dwCurTime - m_dwSystemTick;
	iFPS = (int)(m_iTick * 1000.0 / dwElapTime);
	

	/*if (iFPS == 50)
	{
		int test=1;
	}*/
	if (dwElapTime >= 1000)
	{
		_itow_s(iFPS, szFPS, 10);
		SetWindowTextW(hWnd, szFPS);

		m_iTick = 0;
		m_dwSystemTick = dwCurTime;
	}
	else
	{
		if (iFPS < m_iMaxFPS)
		{
			m_iOneFrameTick += 1000 - dwElapTime;
			if (m_iOneFrameTick >= 20 * 1000)
			{
				m_iOneFrameTick -= 20 * 1000;
				return TRUE;
			}
		}
		Sleep(0.02f * 1000);
	}
	return FALSE;
}
