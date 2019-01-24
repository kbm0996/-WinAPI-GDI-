#include "stdafx.h"
#include "CEffectObject.h"



CEffectObject::CEffectObject(int iX, int iY, __int64 Type, __int64 ID)
{
	m_CurX = iX;
	m_CurY = iY;
	m_iObjectType = Type;
	m_iObjectID = ID;
	m_bEffectStart = FALSE;
	SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_MAX, 4);
}

CEffectObject::~CEffectObject()
{
	
}

void CEffectObject::Action()
{
	if (m_bEndFrame)
		return;

	if (m_bEffectStart)
		NextFrame();
}

bool CEffectObject::Draw(BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	g_cSprite.DrawSprite(GetSprite(), m_CurX, m_CurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	return true;
}