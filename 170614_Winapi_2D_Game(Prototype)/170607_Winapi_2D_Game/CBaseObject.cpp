#include "stdafx.h"
#include "CBaseObject.h"

void CBaseObject::SetPosition(int iX, int iY)
{
	m_CurX = iX;
	m_CurY = iY;
}

void CBaseObject::GetPosition(int * OutX, int * OutY)
{
	OutX = &m_CurX;
	OutY = &m_CurY;
}

int CBaseObject::GetCurX()
{
	return m_CurX;
}

int CBaseObject::GetCurY()
{
	return m_CurY;
}

BOOL CBaseObject::IsEndFrame()
{
	return m_bEndFrame;
}

void CBaseObject::NextFrame()
{
	if (0 > m_iSpriteStart)
		return;

	m_iDelayCount++;
	if (m_iDelayCount >= m_iFrameDelay)
	{
		m_iDelayCount = 0;
		m_iSpriteNow++;
		if (m_iSpriteNow > m_iSpriteEnd)
		{
			m_iSpriteNow = m_iSpriteStart;
			m_bEndFrame = TRUE;
		}
	}
}


void CBaseObject::SetSprite(int iSpriteStart, int iSpriteEnd, int iFrameDelay)
{
	m_iSpriteStart = iSpriteStart;
	m_iSpriteEnd = iSpriteEnd;
	m_iFrameDelay = iFrameDelay;

	m_iSpriteNow = iSpriteStart;
	m_iDelayCount = 0;
	m_bEndFrame = FALSE;
}

int CBaseObject::GetSprite()
{
	return m_iSpriteNow;
}

void CBaseObject::ActionInput(DWORD dwAction)
{
	m_dwActionInput = dwAction;
}

void CBaseObject::SetObjectID(int ObjectID)
{
	m_iObjectID = ObjectID;
}

int CBaseObject::GetObjectID()
{
	return m_iObjectID;
}

void CBaseObject::SetObjectType(int ObjectType)
{
	m_iObjectType = ObjectType;
}

int CBaseObject::GetObjectType()
{
	return m_iObjectType;
}