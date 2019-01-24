#include "stdafx.h"
#include "CPlayer.h"
#include "Windows.h"

CPlayer* g_pPlayer;

CPlayer::CPlayer(int iX, int iY, BOOL isPlayer, __int64 Type, __int64 ID, __int64 HP)
{
	m_CurX = iX;
	m_CurY = iY;
	m_bPlayerCharacter = isPlayer;
	m_iObjectType = Type;
	m_iObjectID = ID;
	m_iHP = HP;

	m_dwActionInput = dfACTION_STAND;
	m_iDirCur = dfDIR_RIGHT;
	m_iDirOld = m_iDirCur;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Action()	// 1 Action을 받아 멤버를 설정하는 부분,  2 애니메이션을 설정하는 부분, 3 좌표 설정 등 액션을 취하는 부분으로 나눠야함.
{
	NextFrame(); // 프레임 넘기기

	if (m_dwActionOld == dfACTION_ATTACK1 || m_dwActionOld == dfACTION_ATTACK2 || m_dwActionOld == dfACTION_ATTACK3)
	{
		if (IsEndFrame())
		{
			SetActionStand();
			// 공격이 끝나면 액션을 바꿔서 연속으로 공격할 때 재전송이 가능하도록 한다.
			m_dwActionOld = dfACTION_STAND;
		}
	}
	else
	{
		////////////////////////////////////////////////////////////////////////////////////
		// 1. Action을 받아 멤버를 설정하는 부분
		//
		////////////////////////////////////////////////////////////////////////////////////
		if (m_CurY != dfRANGE_MOVE_TOP || m_CurY != dfRANGE_MOVE_BOTTOM || m_CurX != dfRANGE_MOVE_LEFT || m_CurX != dfRANGE_MOVE_RIGHT)
		{
			m_iDirOld = m_iDirCur;
			switch (m_dwActionInput)
			{
			case dfACTION_MOVE_LU:
				m_iDirCur = dfDIR_LEFT;
				break;
			case dfACTION_MOVE_RU:
				m_iDirCur = dfDIR_RIGHT;
				break;
			case dfACTION_MOVE_LD:
				m_iDirCur = dfDIR_LEFT;
				break;
			case dfACTION_MOVE_RD:
				m_iDirCur = dfDIR_RIGHT;
				break;
			case dfACTION_MOVE_LL:
				m_iDirCur = dfDIR_LEFT;
				break;
			case dfACTION_MOVE_RR:
				m_iDirCur = dfDIR_RIGHT;
				break;
			}
		}

		////////////////////////////////////////////////////////////////////////////////////
		// 2 애니메이션을 설정하는 부분
		//
		////////////////////////////////////////////////////////////////////////////////////
		m_dwActionCur = m_dwActionInput;
		switch (m_dwActionCur)
		{
		case dfACTION_ATTACK1:
			SetActionAttack1();
			break;
		case dfACTION_ATTACK2:
			SetActionAttack2();
			break;
		case dfACTION_ATTACK3:
			SetActionAttack3();
			break;
		case dfACTION_STAND:
			if (m_dwActionOld != dfACTION_STAND)
				SetActionStand();
			break;
		default:
			if (m_dwActionOld == dfACTION_STAND || m_iDirCur != m_iDirOld)
				SetActionMove();
			break;
		}
		m_dwActionOld = m_dwActionCur;


		////////////////////////////////////////////////////////////////////////////////////
		// 3. 좌표 이동
		//
		////////////////////////////////////////////////////////////////////////////////////
		switch (m_dwActionOld)
		{
		case dfACTION_MOVE_LU:
			if (m_CurY != dfRANGE_MOVE_TOP && m_CurX != dfRANGE_MOVE_LEFT)
			{
				m_CurX -= dfSPEED_PLAYER_X;
				m_CurY -= dfSPEED_PLAYER_Y;
			}
			break;

		case dfACTION_MOVE_RU:
			if (m_CurY != dfRANGE_MOVE_TOP && m_CurX != dfRANGE_MOVE_RIGHT)
			{
				m_CurX += dfSPEED_PLAYER_X;
				m_CurY -= dfSPEED_PLAYER_Y;
			}
			break;

		case dfACTION_MOVE_LD:
			if (m_CurY != dfRANGE_MOVE_BOTTOM && m_CurX != dfRANGE_MOVE_LEFT)
			{
				m_CurX -= dfSPEED_PLAYER_X;
				m_CurY += dfSPEED_PLAYER_Y;
			}
			break;

		case dfACTION_MOVE_RD:
			if (m_CurY != dfRANGE_MOVE_BOTTOM && m_CurX != dfRANGE_MOVE_RIGHT)
			{
				m_CurX += dfSPEED_PLAYER_X;
				m_CurY += dfSPEED_PLAYER_Y;
			}
			break;

		case dfACTION_MOVE_UU:
			m_CurY -= dfSPEED_PLAYER_Y;
			break;

		case dfACTION_MOVE_DD:
			m_CurY += dfSPEED_PLAYER_Y;
			break;

		case dfACTION_MOVE_LL:
			m_CurX -= dfSPEED_PLAYER_X;
			break;

		case dfACTION_MOVE_RR:
			m_CurX += dfSPEED_PLAYER_X;
			break;
		}

		if (m_CurX < dfRANGE_MOVE_LEFT)		m_CurX = dfRANGE_MOVE_LEFT;
		if (m_CurY < dfRANGE_MOVE_TOP)		m_CurY = dfRANGE_MOVE_TOP;
		if (m_CurX > dfRANGE_MOVE_RIGHT)	m_CurX = dfRANGE_MOVE_RIGHT;
		if (m_CurY > dfRANGE_MOVE_BOTTOM)	m_CurY = dfRANGE_MOVE_BOTTOM;
	}
}

bool CPlayer::Draw(BYTE *bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	g_cSprite.DrawSprite50(eSHADOW, GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch); 

	if(!m_bPlayerCharacter)
		g_cSprite.DrawSpriteRed(GetSprite(), m_CurX, m_CurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	else
		g_cSprite.DrawSprite(GetSprite(), m_CurX, m_CurY, bypDest, iDestWidth, iDestHeight, iDestPitch);

	g_cSprite.DrawSprite(eGUAGE_HP, GetCurX() - 35, GetCurY() + 9, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());
	return true;
}

void CPlayer::SetHP(int iHP)
{
	m_iHP = iHP;
}

__int64 CPlayer::GetHP()
{
	return m_iHP;
}

void CPlayer::SetDirection(int iDir)
{
	m_iDirCur = iDir;
}

int CPlayer::GetDirection()
{
	return m_iDirCur;
}

void CPlayer::SetActionAttack1()
{
	if (m_iDirCur == dfDIR_LEFT)		SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L_MAX, 4);
	else if (m_iDirCur == dfDIR_RIGHT)	SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R_MAX, 4);
}

void CPlayer::SetActionAttack2()
{
	if (m_iDirCur == dfDIR_LEFT)		SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L_MAX, 4);
	else if (m_iDirCur == dfDIR_RIGHT)	SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R_MAX, 4);
}

void CPlayer::SetActionAttack3()
{
	if (m_iDirCur == dfDIR_LEFT)		SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L_MAX, 6);
	else if (m_iDirCur == dfDIR_RIGHT)	SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R_MAX, 6);
}

void CPlayer::SetActionMove()
{
	if (m_dwActionCur == dfACTION_MOVE_DD || m_dwActionCur == dfACTION_MOVE_UU)
	{
		if (m_iDirCur == dfDIR_LEFT)			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, 12);
		else if (m_iDirCur == dfDIR_RIGHT)		SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, 12);
	}
	else if (m_dwActionCur == dfACTION_MOVE_LD || m_dwActionCur == dfACTION_MOVE_LU || m_dwActionCur == dfACTION_MOVE_LL)
	{
		SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, 12);
	}
	else if (m_dwActionCur == dfACTION_MOVE_RD || m_dwActionCur == dfACTION_MOVE_RU || m_dwActionCur == dfACTION_MOVE_RR)
	{
		SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, 12);
	}
}

void CPlayer::SetActionStand()
{
	if(m_iDirCur == dfDIR_LEFT)		SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, 5);
	else if (m_iDirCur == dfDIR_RIGHT)	SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, 5);
}
