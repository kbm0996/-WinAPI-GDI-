#include "stdafx.h"
#include "KeyProcess.h"
#include "DefineEnum.h"
#include "CBaseObject.h"
#include "CPlayer.h"

bool KeyProcess()	// if문으로 구성할 경우 키를 많이 눌러야하는 로직을 뒤로, if-else문으로 구성할 경우 위로
{
	if (g_pPlayer != NULL)
	{
		DWORD dwAction;

		if (GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			dwAction = dfACTION_MOVE_LU;
		}
		else if (GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			dwAction = dfACTION_MOVE_RU;
		}
		else if (GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			dwAction = dfACTION_MOVE_LD;
		}
		else if (GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			dwAction = dfACTION_MOVE_RD;
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			dwAction = dfACTION_MOVE_UU;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			dwAction = dfACTION_MOVE_DD;
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			dwAction = dfACTION_MOVE_LL;
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			dwAction = dfACTION_MOVE_RR;
		}
		else if (GetAsyncKeyState(0x41) & 0x8000) // A
		{
			dwAction = dfACTION_ATTACK1;
		}
		else if (GetAsyncKeyState(0x53) & 0x8000) // S
		{
			dwAction = dfACTION_ATTACK2;
		}
		else if (GetAsyncKeyState(0x44) & 0x8000) // D
		{
			dwAction = dfACTION_ATTACK3;
		}
		else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			dwAction = dfACTION_PUSH;
		}
		else
			dwAction = dfACTION_STAND;

		g_pPlayer->ActionInput(dwAction);
		return true;
	}
	else
		return false;
}
