/// Aimbot.hpp

#pragma once

namespace Features::Combat
{
	void Aimbot( SDK::player_ptr_t pLocalPlayer, CUserCmd* pCmd );
	void GetRangeReference( QAngle& angReference );
}
