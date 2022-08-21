/// Auto Jump.cpp

#include "../../RFGH.hpp"

namespace Features::Miscellaneous
{
	void AutoJump( SDK::player_ptr_t pLocalPlayer, CUserCmd* pCmd )
	{
		static auto bJumpedLastTick = false;

		if ( bJumpedLastTick
			 || !( pLocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
		{
			pCmd->buttons &= ~IN_JUMP;
			bJumpedLastTick = false;
		}
		else if ( Config::_Settings._Miscellaneous._AutoJump.bEnabled
				  && Input::IsKeyPressed( Config::_Settings._Miscellaneous._AutoJump.dwKey )
				  && pLocalPlayer->m_fFlags( ) & FL_ONGROUND )
		{
			pCmd->buttons |= IN_JUMP;
			bJumpedLastTick = true;
		}
	}
}
