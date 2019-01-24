#include "Windows.h"
#include "stdafx.h"
#include "SpriteDib.h"
#include "ScreenDib.h"
#include "DefineEnum.h"

CSpriteDib g_cSprite(eSPRITE_MAX, 0x00ffffff);

CSpriteDib::CSpriteDib(int iMaxSprite, DWORD dwColorKey)
{
	m_iMaxSprite = iMaxSprite;
	m_dwColorKey = dwColorKey;

	m_stpSprite = NULL;

	m_stpSprite = new st_SPRITE[m_iMaxSprite];
	memset(m_stpSprite, 0, sizeof(st_SPRITE) * m_iMaxSprite);
}

CSpriteDib::~CSpriteDib()
{
	for (int iCnt = 0; iCnt > m_iMaxSprite; ++iCnt)
	{
		ReleaseSprite(iCnt);
	}
}

BOOL CSpriteDib::LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY)
{
	HANDLE		hFile;
	DWORD		dwRead;
	int			iPitch;
	int			iImageSize;
	int			iCnt;

	BITMAPFILEHEADER stFileHeader;
	BITMAPINFOHEADER stInfoHeader;

	hFile = CreateFile(szFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return FALSE;
	}

	ReleaseSprite(iSpriteIndex);
	ReadFile(hFile, &stFileHeader, sizeof(BITMAPFILEHEADER), &dwRead, NULL);
	if (stFileHeader.bfType == 0x4d42)
	{
		ReadFile(hFile, &stInfoHeader, sizeof(BITMAPINFOHEADER), &dwRead, NULL);
		if (stInfoHeader.biBitCount == 32)
		{
			iPitch = (stInfoHeader.biWidth * 4) + 3 & ~3;

			m_stpSprite[iSpriteIndex].iWidth = stInfoHeader.biWidth;
			m_stpSprite[iSpriteIndex].iHeight = stInfoHeader.biHeight;
			m_stpSprite[iSpriteIndex].iPitch = iPitch;

			iImageSize = iPitch * stInfoHeader.biHeight;
			m_stpSprite[iSpriteIndex].bypImage = new BYTE[iImageSize];

			BYTE *bypTempBuffer = new BYTE[iImageSize];
			BYTE *bypSpriteTemp = m_stpSprite[iSpriteIndex].bypImage;
			BYTE *bypTurnTemp;

			ReadFile(hFile, bypTempBuffer, iImageSize, &dwRead, NULL);

			bypTurnTemp = bypTempBuffer + iPitch * (stInfoHeader.biHeight - 1);

			for (iCnt = 0; iCnt < stInfoHeader.biHeight; iCnt++)
			{
				memcpy_s(bypSpriteTemp, iImageSize, bypTurnTemp, iPitch);
				bypSpriteTemp += iPitch;
				bypTurnTemp -= iPitch;
			}
			delete[] bypTempBuffer;

			m_stpSprite[iSpriteIndex].iCenterPointX = iCenterPointX;
			m_stpSprite[iSpriteIndex].iCenterPointY = iCenterPointY;
			CloseHandle(hFile);
			return TRUE;
		}
	}
	CloseHandle(hFile);
	return FALSE;
}

void CSpriteDib::ReleaseSprite(int iSpriteIndex)
{
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	if (m_stpSprite[iSpriteIndex].bypImage != NULL)
	{
		delete[] m_stpSprite[iSpriteIndex].bypImage;
		memset(&m_stpSprite[iSpriteIndex], 0, sizeof(st_SPRITE));
	}
}

void CSpriteDib::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	if (iSpriteIndex >= m_iMaxSprite)	return;

	if (m_stpSprite[iSpriteIndex].bypImage == NULL)	return;
	
	st_SPRITE *stpSprite = &m_stpSprite[iSpriteIndex];

	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCntX, iCntY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;
	DWORD *dwpDest = (DWORD *)bypDest;
	DWORD *dwpSprite = (DWORD *)(stpSprite->bypImage);

	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD *)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));
		iDrawY = 0;
	}

	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);
		iDrawX = 0;
	}

	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);
	}

	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
		return;

	dwpDest = (DWORD *)(((BYTE *)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	BYTE *bypDestOrigin = (BYTE *)dwpDest;
	BYTE *bypSpriteOrigin = (BYTE *)dwpSprite;
	
	for (iCntY = 0; iSpriteHeight > iCntY; iCntY++)
	{
		for (iCntX = 0; iSpriteWidth > iCntX; iCntX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff))
			{
				*dwpDest = *dwpSprite;
			}
		
			dwpDest++;
			dwpSprite++;
		}

	
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD *)bypDestOrigin;
		dwpSprite = (DWORD *)bypSpriteOrigin;
	}
}

void CSpriteDib::DrawSpriteRed(int iSpriteIndex, int iDrawX, int iDrawY, BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	if (iSpriteIndex >= m_iMaxSprite)	return;

	if (m_stpSprite[iSpriteIndex].bypImage == NULL)	return;

	st_SPRITE *stpSprite = &m_stpSprite[iSpriteIndex];

	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCntX, iCntY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;
	DWORD *dwpDest = (DWORD *)bypDest;
	DWORD *dwpSprite = (DWORD *)(stpSprite->bypImage);

	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD *)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));
		iDrawY = 0;
	}

	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);
		iDrawX = 0;
	}

	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);
	}

	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
		return;

	dwpDest = (DWORD *)(((BYTE *)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	BYTE *bypDestOrigin = (BYTE *)dwpDest;
	BYTE *bypSpriteOrigin = (BYTE *)dwpSprite;

	for (iCntY = 0; iSpriteHeight > iCntY; iCntY++)
	{
		for (iCntX = 0; iSpriteWidth > iCntX; iCntX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff))
			{
				*dwpDest = (*dwpSprite & 0x00ff0000);
			}

			dwpDest++;
			dwpSprite++;
		}


		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD *)bypDestOrigin;
		dwpSprite = (DWORD *)bypSpriteOrigin;
	}
}

void CSpriteDib::DrawSprite50(int iSpriteIndex, int iDrawX, int iDrawY, BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	if (iSpriteIndex >= m_iMaxSprite)	return;

	if (m_stpSprite[iSpriteIndex].bypImage == NULL)	return;

	st_SPRITE *stpSprite = &m_stpSprite[iSpriteIndex];

	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCntX, iCntY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;
	DWORD *dwpDest = (DWORD *)bypDest;
	DWORD *dwpSprite = (DWORD *)(stpSprite->bypImage);
	DWORD *dwpScreen = (DWORD *)g_cScreenDib.GetDibBuffer(); 

	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD *)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));
		iDrawY = 0;
	}

	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);
		iDrawX = 0;
	}

	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);
	}

	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
		return;

	dwpDest = (DWORD *)(((BYTE *)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));
	dwpScreen = (DWORD *)(((BYTE *)(dwpScreen + iDrawX) + (iDrawY * iDestPitch))); 

	BYTE *bypDestOrigin = (BYTE *)dwpDest;
	BYTE *bypSpriteOrigin = (BYTE *)dwpSprite;
	BYTE *bypScreenOrigin = (BYTE *)dwpScreen; 

	for (iCntY = 0; iSpriteHeight > iCntY; iCntY++)
	{
		for (iCntX = 0; iSpriteWidth > iCntX; iCntX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff))
			{
				//*dwpDest = (*dwpDest >> 1) + *dwpSprite >> 1 & 0x7f7f7f7f;	// 이미지 rgb 추출하지 않고 Alpha 표현
				*dwpDest = ((*dwpSprite & 0x007f7f7f) + (*dwpScreen & 0x007f7f7f));
			}

			dwpDest++;
			dwpSprite++;
			dwpScreen++;
		}

		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;
		bypScreenOrigin = bypScreenOrigin + g_cScreenDib.GetPitch();

		dwpDest = (DWORD *)bypDestOrigin;
		dwpSprite = (DWORD *)bypSpriteOrigin;
		dwpScreen = (DWORD *)bypScreenOrigin; 
	}
}

void CSpriteDib::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	if (iSpriteIndex >= m_iMaxSprite)	return;

	if (m_stpSprite[iSpriteIndex].bypImage == NULL)	return;

	st_SPRITE *stpSprite = &m_stpSprite[iSpriteIndex];

	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCntY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD *dwpDest = (DWORD *)bypDest;
	DWORD *dwpSprite = (DWORD *)(stpSprite->bypImage);

	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD *)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));
		iDrawY = 0;
	}

	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);
		iDrawX = 0;
	}

	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);
	}

	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
		return;

	dwpDest = (DWORD *)(((BYTE *)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	for (iCntY = 0; iSpriteHeight  > iCntY; iCntY++)
	{
		memcpy(dwpDest, dwpSprite, iSpriteWidth * 4);
		dwpDest = (DWORD *)((BYTE *)dwpDest + iDestPitch);
		dwpSprite = (DWORD *)((BYTE *)dwpSprite + stpSprite->iPitch);
	}
}
