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
	void NextFrame();		// �ִϸ��̼� �������� �Ѱ��ִ� ���� Action()���� ȣ��
	
	void SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay);		// �ش� �׼��� �������� ������ STAND�� ���ư��� ������ ��Ŷ ��ſ� ����
	int GetSprite();

protected:
	BOOL m_bEndFrame;		// �ִϸ��̼� �� ���޽� True
	int m_iSpriteStart;		//  Now�� End�� �����ϸ� m_bEndFrame�� True�� �ٲ�� Start�� ���ư�
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