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

	BOOL m_bPlayerCharacter;	// 본인 것인지 서버에서 받아온 남의 것인지 판단
	// 네트워크시 내 동작은 딜레이를 따져야하나 서버에서 온 남의 동작은 이미 처리된 것으로 판단하여 패킷이 오는데로 행동을 하게 해야한다. 
	DWORD m_dwActionCur;		// 액션을 변경할때 패킷 전송
	DWORD m_dwActionOld;		// 이를 판단하기 위한 변수

};

extern CPlayer* g_pPlayer;

#endif