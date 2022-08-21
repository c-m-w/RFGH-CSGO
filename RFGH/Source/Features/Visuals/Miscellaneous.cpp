/// Miscellaneous.cpp

#include "../../RFGH.hpp"

namespace Features::Visuals
{
	void VisualizeAimbotRange( )
	{
		if ( !Config::_Settings._MiscellaneousVisuals.bVisualizeAimbotRange
			 || !SDK::pEngineClient->IsInGame( ) )
			return;

		const auto pLocalPlayer = SDK::player_ptr_t( SDK::pEntityList->GetClientEntity( SDK::pEngineClient->GetLocalPlayer( ) ) );
		if ( nullptr == pLocalPlayer
			 || !pLocalPlayer->IsAlive( ) )
			return;

		const auto hWeapon = pLocalPlayer->m_hActiveWeapon( );
		if ( hWeapon == nullptr )
			return;

		const auto pData = hWeapon->GetWeaponData( );
		const auto pConfig = &Config::_Settings._Aimbot._Weapons[ pData->weapon_type ];
		const auto vecEyePosition = pLocalPlayer->GetEyePosition( );
		auto angReference = SDK::pClientState->viewangles;
		Combat::GetRangeReference( angReference );
		Vector vecReal, vecFake;

		auto angMax = angReference;
		angMax.yaw += pConfig->flMaxCrosshairDistance;

		const auto vecForwardReal = SDK::Utilities::TransformAngle( angReference ) * 8192.f + vecEyePosition,
			vecForwardFake = SDK::Utilities::TransformAngle( angMax ) * 8192.f + vecEyePosition;
		SDK::Utilities::WorldToScreen( vecForwardReal, vecReal );
		SDK::Utilities::WorldToScreen( vecForwardFake, vecFake );
		const auto vecDifference = vecReal - vecFake;
		const auto flRadius = vecDifference.Length2D( );

		SDK::pSurface->DrawSetColor( Config::_Settings._MiscellaneousVisuals.clrAimbotRange );
		SDK::pSurface->DrawOutlinedCircle( int( vecReal.x ), int( vecReal.y ), int( flRadius ), int( flRadius * D3DX_PI / 2.f ) );
	}
}
