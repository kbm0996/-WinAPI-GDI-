#ifndef __FRAMESKIP_H__
#define __FRAMESKIP_H__
#include "stdafx.h"

class CFrameSkip
{
public:
	CFrameSkip();
	~CFrameSkip();
	BOOL FrameSkip(HWND hWnd);

private:
	int m_iMaxFPS;
	DWORD m_dwSystemTick;

	int m_iOneFrameTick;
	int m_iTick;
};

extern CFrameSkip* Frame;

#endif