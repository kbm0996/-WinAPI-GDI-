#ifndef __EFFECT_OBJ__
#define __EFFECT_OBJ__
#include "CBaseObject.h"

class CEffectObject : public CBaseObject
{
public:
	CEffectObject(int iX, int iY, __int64 Type = eTYPE_EFFECT, __int64 ID = 0);
	~CEffectObject();

	void Action();
	bool Draw(BYTE *bypDest, int iDestWidth, int iDestHeight, int iDestPitch);

private:
	BOOL m_bEffectStart;
	DWORD m_dwAttackID;
};

#endif