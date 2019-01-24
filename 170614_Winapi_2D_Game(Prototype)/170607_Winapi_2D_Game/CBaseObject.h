#ifndef __BASE_OBJ__
#define __BASE_OBJ__
#include "ScreenDib.h"
#include "SpriteDib.h"
#include "LinkedList.h"
#include "DefineEnum.h"
#include "stdafx.h"

class CBaseObject
{
public:
	CBaseObject() {}
	virtual ~CBaseObject() {}	

	virtual void Action() = 0;	
	virtual bool Draw(BYTE *bypDest, int iDestWidth, int iDestHeight, int iDestPitch) = 0;
	void ActionInput(DWORD dwAction);

	void SetObjectID(int ObjectID);
	int GetObjectID();

	void SetObjectType(int ObjectType);
	int GetObjectType();

	void SetPosition(int iX, int iY);
	void GetPosition(int *OutX, int *OutY);
	int GetCurX();
	int GetCurY();

	BOOL IsEndFrame();	
	void NextFrame();		// 애니메이션 프레임을 넘겨주는 역할 Action()에서 호출
	
	void SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay);		// 해당 액션이 끝났을때 강제로 STAND로 돌아가게 만들어야 패킷 통신에 적합
	int GetSprite();

protected:
	BOOL m_bEndFrame;		// 애니메이션 끝 도달시 True
	int m_iSpriteStart;		//  Now가 End에 도달하면 m_bEndFrame이 True로 바뀌고 Start로 돌아감
	int m_iSpriteNow;
	int m_iSpriteEnd;
	int m_iDelayCount;
	int m_iFrameDelay;

	DWORD m_dwActionInput;

	__int64 m_iObjectType;
	__int64 m_iObjectID;
	int m_CurX;
	int m_CurY;
	bool m_bFlag;
};

#endif