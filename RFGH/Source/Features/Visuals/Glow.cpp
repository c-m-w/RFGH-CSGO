/// Glow.cpp

#include "../../RFGH.hpp"

namespace Features::Visuals
{
	void GlowObjects( )
	{
		if ( !SDK::pEngineClient->IsInGame( ) )
			return;

		const auto pLocalPlayer = SDK::player_ptr_t( SDK::pEntityList->GetClientEntity( SDK::pEngineClient->GetLocalPlayer( ) ) );
		if ( pLocalPlayer == nullptr )
			return;

		const auto iLocalPlayerTeam = pLocalPlayer->m_iTeamNum( );
		for( auto i = 0; i < SDK::pGlowObjectManager->m_iCount; i++ )
		{
			auto& pDefinition = SDK::pGlowObjectManager->m_pObjects[ i ];

			if ( pDefinition.m_pEntity == nullptr
				 || !pDefinition.m_pEntity->IsAlive( )
				 || !pDefinition.m_pEntity->IsPlayer( ) )
				continue;

			auto& _Config = Config::_Settings._Glow._Players[ pDefinition.m_pEntity->m_iTeamNum( ) == iLocalPlayerTeam ? Config::config_t::TEAM : Config::config_t::ENEMY ];
			const auto iVisibilityIndex = pLocalPlayer->CanSeePlayer( pDefinition.m_pEntity ) ? Config::config_t::VISIBLE : Config::config_t::INVISIBLE;

			if( !Config::_Settings._Glow.bGlobalEnable
				|| !_Config.bEnabled[ iVisibilityIndex ] )
			{
				pDefinition.Reset( );
				continue;
			}

			pDefinition.m_flRed = float( _Config.clrGlow[ iVisibilityIndex ].r( ) ) / 255.f;
			pDefinition.m_flGreen = float( _Config.clrGlow[ iVisibilityIndex ].g( ) ) / 255.f;
			pDefinition.m_flBlue = float( _Config.clrGlow[ iVisibilityIndex ].b( ) ) / 255.f;
			pDefinition.m_flAlpha = float( _Config.clrGlow[ iVisibilityIndex ].a( ) ) / 255.f;
			pDefinition.m_nGlowStyle = _Config.iGlowStyle;

			if( _Config.iGlowStyle == 0 )
			{
				pDefinition.m_bFullBloomRender = _Config.bFullBloom;
				pDefinition.m_flFullBloomAmount = _Config.bFullBloom ? _Config.flFullBloomAmount : 1.f;
			}
			else
			{
				pDefinition.m_bFullBloomRender = false;
				pDefinition.m_flFullBloomAmount = 1.f;
			}

			pDefinition.m_bRenderWhenUnoccluded = false;
			pDefinition.m_bRenderWhenOccluded = true;
		}
	}
}
