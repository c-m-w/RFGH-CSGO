/// Aimbot.cpp

#include "../../RFGH.hpp"

namespace Features::Combat
{
	constexpr auto RECOIL_SCALE = 2.f;

	QAngle angControlledRecoil { };

	struct
	{
		Config::config_t::aimbot_t::weapon_t* pConfig = nullptr;
		int iLastTargetIndex = -1,
			iTargetIndex = -1;
		SDK::C_BasePlayer* pTarget = nullptr;
		int iTargetHitbox = -1;
		Vector vecTarget { };
		QAngle angFinal { },
			angAimVelocity { };
		float flTotalTime = 0.f;

		bool bRestoreAngles = false;
		QAngle angOld { };

		void Reset( );
	} _Context;

	void decltype( _Context )::Reset( )
	{
		*this = decltype( _Context )( );
	}

	bool FindTarget( SDK::player_ptr_t pLocalPlayer, CUserCmd* pCmd );
	bool FindHitbox( SDK::player_ptr_t pLocalPlayer, CUserCmd* pCmd );

	void Aimbot( SDK::player_ptr_t pLocalPlayer, CUserCmd* pCmd )
	{
		if ( !Config::_Settings._Aimbot.bGlobalEnable )
			return _Context.Reset( );

		const auto hWeapon = pLocalPlayer->m_hActiveWeapon( );
		if ( hWeapon == nullptr )
			return _Context.Reset( );

		const auto pData = hWeapon->GetWeaponData( );

		_Context.pConfig = &Config::_Settings._Aimbot._Weapons[ pData->weapon_type ];

		if ( !( pCmd->buttons & IN_ATTACK )
			 && _Context.pConfig->bControlRecoil )
			angControlledRecoil = pLocalPlayer->m_aimPunchAngle( ) * RECOIL_SCALE;

		if ( !_Context.pConfig->bControlRecoil )
			angControlledRecoil = QAngle( );

		if( _Context.bRestoreAngles )
		{
			_Context.bRestoreAngles = false;
			pCmd->viewangles = _Context.angOld;
		}
		else
		{
			if ( !Input::IsKeyPressed( _Context.pConfig->dwKey )
				 || !pLocalPlayer->m_bIsScoped( ) && _Context.pConfig->bOnlyScoped )
				return _Context.Reset( );

			pCmd->viewangles += angControlledRecoil;
			SDK::Utilities::ClampAngle( pCmd->viewangles );
			const auto bFoundTarget = FindTarget( pLocalPlayer, pCmd );
			pCmd->viewangles -= angControlledRecoil;
			SDK::Utilities::ClampAngle( pCmd->viewangles );

			if ( bFoundTarget )
			{
				if ( _Context.pConfig->bSilentAim )
				{
					const auto fnDoSilent = [ & ]( )
					{
						_Context.angOld = pCmd->viewangles;
						pCmd->viewangles = _Context.angFinal - angControlledRecoil;
						SDK::Utilities::ClampAngle( pCmd->viewangles );
						_Context.bRestoreAngles = false;
					};

					if ( hWeapon->CanFire( ) )
					{
						if ( pCmd->buttons & IN_ATTACK )
							fnDoSilent( );
						else
						{
							fnDoSilent( );
							if ( !Triggerbot( pLocalPlayer, pCmd ) )
							{
								_Context.bRestoreAngles = false;
								pCmd->viewangles = _Context.angOld;
								SDK::Utilities::ClampAngle( pCmd->viewangles );
							}
						}
					}
				}
				else
				{
					SDK::pClientState->viewangles -= _Context.angAimVelocity * SDK::pGlobalVars->interval_per_tick;
					SDK::Utilities::ClampAngle( SDK::pClientState->viewangles );
					_Context.flTotalTime += SDK::pGlobalVars->interval_per_tick;

					if ( _Context.flTotalTime >= _Context.pConfig->flAimTime )
						_Context.angAimVelocity = QAngle( );
				}
			}
		}

		if ( _Context.pConfig->bControlRecoil )
		{
			const auto angPunch = pLocalPlayer->m_aimPunchAngle( ) * RECOIL_SCALE;
			const auto angControlled = ( angPunch - angControlledRecoil ) * _Context.pConfig->flRecoilAccuracy;
			SDK::pClientState->viewangles -= angControlled;
			SDK::Utilities::ClampAngle( SDK::pClientState->viewangles );
			angControlledRecoil += angControlled;
		}
	}

	void GetRangeReference( QAngle& angReference )
	{
		angReference += angControlledRecoil;
	}

	bool FindTarget( SDK::player_ptr_t pLocalPlayer, CUserCmd* pCmd )
	{
		const auto iLocalPlayerIndex = pLocalPlayer->EntIndex( );
		const auto vecEyePosition = pLocalPlayer->GetEyePosition( );
		const auto vecOrigin = pLocalPlayer->m_vecOrigin( );
		auto flLowestCrosshairDistance = FLT_MAX,
			flLowestDistance = FLT_MAX;
		auto iLowestHealth = INT_MAX;
		auto bEntityWithinRange = false;

		_Context.iLastTargetIndex = _Context.iTargetIndex;
		_Context.iTargetIndex = -1;

		for( auto i = 0; i < SDK::pGlobalVars->maxClients; i++ )
		{
			if ( i == iLocalPlayerIndex )
				continue;

			const auto pPlayer = SDK::player_ptr_t( SDK::pEntityList->GetClientEntity( i ) );
			if ( pPlayer == nullptr
				 || !pPlayer->IsPlayer( )
				 || !pPlayer->IsAlive( )
				 || !_Context.pConfig->bEnabled[ pLocalPlayer->PlayerOnTeam( pPlayer ) ? Config::config_t::TEAM : Config::config_t::ENEMY ]
				 || !pLocalPlayer->CanSeePlayer( pPlayer ) )
				continue;

			const auto vecReferenceHitbox = pPlayer->GetHitboxPosition( _Context.pConfig->iReferenceHitbox );
			const auto angAim = SDK::Utilities::GetAngle( vecEyePosition, vecReferenceHitbox );
			const auto flCrosshairDistance = SDK::Utilities::GetAngleDifference( angAim, pCmd->viewangles );

			switch( _Context.pConfig->iTargetingMode )
			{
				case Config::config_t::aimbot_t::weapon_t::CROSSHAIR_DISTANCE:
				{
					if( flCrosshairDistance < flLowestCrosshairDistance )
					{
						flLowestCrosshairDistance = flCrosshairDistance;
						_Context.iTargetIndex = i;
					}
				}
				break;

				case Config::config_t::aimbot_t::weapon_t::DISTANCE:
				{
					const auto vecDistance = pPlayer->m_vecOrigin( ) - vecOrigin;
					const auto flDistance = vecDistance.Length( );
					const auto bCurrentInRange = flCrosshairDistance <= _Context.pConfig->flMaxCrosshairDistance;

					if( flDistance < flLowestDistance
						|| !bEntityWithinRange && bCurrentInRange )
					{
						flLowestDistance = flDistance;
						_Context.iTargetIndex = i;
						bEntityWithinRange = bCurrentInRange;
					}
				}
				break;

				case Config::config_t::aimbot_t::weapon_t::HEALTH:
				{
					const auto iHealth = pPlayer->m_iHealth( );
					const auto bCurrentInRange = flCrosshairDistance <= _Context.pConfig->flMaxCrosshairDistance;

					if( iHealth < iLowestHealth
						|| !bEntityWithinRange && bCurrentInRange )
					{
						iLowestHealth = iHealth;
						_Context.iTargetIndex = i;
						bEntityWithinRange = bCurrentInRange;
					}
				}
				break;

				default:
				{
					return false;
				}
			}
		}

		if( _Context.iTargetIndex != -1 )
		{
			_Context.pTarget = decltype( _Context.pTarget )( SDK::pEntityList->GetClientEntity( _Context.iTargetIndex ) );
			return FindHitbox( pLocalPlayer, pCmd );
		}

		return false;
	}

	bool FindHitbox( SDK::player_ptr_t pLocalPlayer, CUserCmd* pCmd )
	{
		const auto vecEyePosition = pLocalPlayer->GetEyePosition( );
		const auto vecPriorityHitbox = _Context.pTarget->GetHitboxPosition( _Context.pConfig->iPriorityHitbox );
		auto flLowestHitboxDistance = FLT_MAX;

		if ( _Context.iLastTargetIndex == _Context.iTargetIndex )
		{
			CGameTrace gtRay;
			if ( _Context.angAimVelocity.IsZero( ) )
			{
				pLocalPlayer->TraceViewRay( pCmd->viewangles, gtRay );
				if ( gtRay.hit_entity != nullptr
					 && gtRay.hit_entity->EntIndex( ) == _Context.iTargetIndex
					 && _Context.pConfig->bHitboxes[ gtRay.hitbox ] )
					return true;
			}
			else
			{
				Ray_t rayTrace;
				CTraceFilter tfFilter;

				rayTrace.Init( vecEyePosition, _Context.vecTarget );
				tfFilter.pSkip = pLocalPlayer;
				SDK::pEngineTrace->TraceRay( rayTrace, MASK_SHOT, &tfFilter, &gtRay );
				if ( gtRay.hit_entity != nullptr
					 && gtRay.hit_entity->EntIndex( ) == _Context.iTargetIndex
					 && _Context.pConfig->bHitboxes[ gtRay.hitbox ] )
					return true;
			}
		}

		_Context.iTargetHitbox = -1;
		_Context.flTotalTime = 0.f;

		if ( SDK::Utilities::GetAngleDifference( SDK::Utilities::GetAngle(
			vecEyePosition, vecPriorityHitbox ),
			pCmd->viewangles ) <= _Context.pConfig->flMaxCrosshairDistance )
		{
			_Context.iTargetHitbox = _Context.pConfig->iPriorityHitbox;
			_Context.vecTarget = vecPriorityHitbox;
		}
		else
			for ( auto i = 0; i < HITBOX_MAX; i++ )
			{
				if ( i == _Context.pConfig->iPriorityHitbox
					 || !_Context.pConfig->bHitboxes[ i ] )
					continue;

				const auto vecHitbox = _Context.pTarget->GetHitboxPosition( i );
				if ( !pLocalPlayer->CanSeePosition( vecHitbox, _Context.pTarget ) )
					continue;

				const auto flHitboxDistance = SDK::Utilities::GetAngleDifference( SDK::Utilities::GetAngle(
					vecEyePosition, vecHitbox ),
					pCmd->viewangles );

				if ( flHitboxDistance < flLowestHitboxDistance
					 && flHitboxDistance <= _Context.pConfig->flMaxCrosshairDistance )
				{
					flLowestHitboxDistance = flHitboxDistance;
					_Context.iTargetHitbox = i;
					_Context.vecTarget = vecHitbox;
				}
			}

		if ( _Context.iTargetHitbox != -1 )
		{
			_Context.angFinal = SDK::Utilities::GetAngle( vecEyePosition, _Context.vecTarget );
			auto angView = pCmd->viewangles;

			while ( angView.yaw < 0.f )
				angView.yaw += 360.f;

			while ( angView.yaw > 360.f )
				angView.yaw -= 360.f;

			auto angDifference = angView - _Context.angFinal;
			SDK::Utilities::ClampAngle( _Context.angFinal );

			while ( angDifference.yaw > 180.f )
				angDifference.yaw -= 360.f;

			while ( angDifference.yaw < -180.f )
				angDifference.yaw += 360.f;

			_Context.angAimVelocity = _Context.pConfig->flAimTime == 0.f ? angDifference / SDK::pGlobalVars->interval_per_tick
				: angDifference / ( roundf( _Context.pConfig->flAimTime / SDK::pGlobalVars->interval_per_tick ) * SDK::pGlobalVars->interval_per_tick );// round to nearest interval_per_tick else
			return true;																																// aim will be slighly off by
		}																																				// aim time % interval per tick * velocity degrees

		return false;
	}
}
