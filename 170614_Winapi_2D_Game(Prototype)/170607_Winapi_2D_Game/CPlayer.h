#ifndef __PLAYER_OBJ__
#define __PLAYER_OBJ__
#include "CBaseObject.h"

class CPlayer : public CBaseObject
{
public:
	CPlayer(int iX, int iY, BOOL isPlayer = FALSE, __int64 Type = e_OBJECT_TYPE::eTYPE_PLAYER, __int64 ID = 0, __int64 HP = 100);
	~CPlayer();

	void Action();
	bool Draw(BYTE *bypDest, int iDestWidth, int iDestHeight, int iDestPitch);

	void SetHP(int iHP);
	__int64 GetHP();
	
	void SetDirection(int iDir);
	int GetDirection();

	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();
	void SetActionMove();
	void SetActionStand();
	

protected:
	__int64 m_iHP;
	int m_iDirCur;
	int m_iDirOld;

	BOOL m_bPlayerCharacter;	// ���� ������ �������� �޾ƿ� ���� ������ �Ǵ�
	// ��Ʈ��ũ�� �� ������ �����̸� �������ϳ� �������� �� ���� ������ �̹� ó���� ������ �Ǵ��Ͽ� ��Ŷ�� ���µ��� �ൿ�� �ϰ� �ؾ��Ѵ�. 
	DWORD m_dwActionCur;		// �׼��� �����Ҷ� ��Ŷ ����
	DWORD m_dwActionOld;		// �̸� �Ǵ��ϱ� ���� ����

};

extern CPlayer* g_pPlayer;

#endif