/// Triggerbot.cpp

#include "../../RFGH.hpp"

namespace Features::Combat
{
	bool Triggerbot( SDK::player_ptr_t pLocalPlayer, CUserCmd* pCmd )
	{
		if ( !Config::_Settings._Triggerbot.bGlobalEnable
			 || pCmd->buttons & IN_ATTACK )
			return false;

		const auto hWeapon = pLocalPlayer->m_hActiveWeapon( );
		if ( hWeapon == nullptr
			 || !hWeapon->CanFire( ) )
			return false;

		const auto pData = hWeapon->GetWeaponData( );
		const auto pConfig = &Config::_Settings._Triggerbot._Weapons[ pData->weapon_type ];

		if ( !Input::IsKeyPressed( pConfig->dwKey )
			 || !pLocalPlayer->m_bIsScoped( ) && pConfig->bOnlyScoped )
			return false;

		CGameTrace gtRay { };

		pLocalPlayer->TraceViewRay( pCmd->viewangles, gtRay );

		if ( !gtRay.DidHit( )
			 || !SDK::player_ptr_t( gtRay.hit_entity )->IsPlayer( )
			 || !SDK::player_ptr_t( gtRay.hit_entity )->IsAlive( )
			 || !pConfig->bEnabled[ pLocalPlayer->PlayerOnTeam( SDK::player_ptr_t( gtRay.hit_entity ) ) ? Config::config_t::TEAM : Config::config_t::ENEMY ]
			 || !pConfig->bHitboxes[ gtRay.hitbox ] )
			return false;

		pCmd->buttons |= IN_ATTACK;
		return true;
	}
}
