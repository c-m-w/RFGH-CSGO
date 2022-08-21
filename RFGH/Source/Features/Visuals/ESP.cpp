/// ESP.cpp

#include "../../RFGH.hpp"

namespace Features::Visuals
{
	vgui::HFont fntESP = NULL;
	struct
	{
		SDK::player_ptr_t pEntity = nullptr;
		Config::config_t::esp_t::player_t* pConfig = nullptr;
		RECT recBoundingBox { };
		int iVisibilityIndex = Config::config_t::VISIBLE;
	} _Context;

	bool GetBoundingBox( );
	void BoundingBox( );
	void Snaplines( );
	void Information( );

	bool Setup( )
	{
		fntESP = SDK::pSurface->CreateFont_( );
		SDK::pSurface->SetFontGlyphSet( fntESP, "Tahoma", 12, 700, 0, 0, FONTFLAG_DROPSHADOW );
		return fntESP != NULL;
	}

	void Draw( )
	{
		if ( !SDK::pEngineClient->IsInGame( )
			 || !Config::_Settings._ESP.bGlobalEnable )
			return;

		const auto iLocalPlayerIndex = SDK::pEngineClient->GetLocalPlayer( );
		const auto pLocalPlayer = SDK::player_ptr_t( SDK::pEntityList->GetClientEntity( iLocalPlayerIndex ) );
		if ( pLocalPlayer == nullptr )
			return;

		const auto iLocalPlayerTeam = pLocalPlayer->m_iTeamNum( );

		for( auto i = 0; i < SDK::pGlobalVars->maxClients; i++ )
		{
			if ( i == iLocalPlayerIndex )
				continue;

			auto pEntity = SDK::player_ptr_t( SDK::pEntityList->GetClientEntity( i ) );

			if ( pEntity == nullptr
				 || !pEntity->IsAlive( )
				 || !pEntity->IsPlayer( ) )
				continue;

			_Context.pEntity = pEntity;
			_Context.pConfig = &Config::_Settings._ESP._Players[ iLocalPlayerTeam == SDK::player_ptr_t( pEntity )->m_iTeamNum( ) ? Config::config_t::TEAM : Config::config_t::ENEMY ];
			_Context.iVisibilityIndex = pLocalPlayer->CanSeePlayer( pEntity ) ? Config::config_t::VISIBLE : Config::config_t::INVISIBLE;

			if( _Context.pConfig->bEnabled[ _Context.iVisibilityIndex ]
				&& GetBoundingBox( ) )
			{
				BoundingBox( );
				Snaplines( );
				Information( );
			}
		}
	}

	bool GetBoundingBox( )
	{
		constexpr auto TRANSFORMATION_POINTS = 8;
		const auto pCollidable = _Context.pEntity->GetCollideable( );

		if ( pCollidable == nullptr )
			return false;

		const auto vecMinimum = pCollidable->OBBMins( ),
			vecMaximum = pCollidable->OBBMaxs( );
		const auto mtxTransformation = _Context.pEntity->m_rgflCoordinateFrame( );

		Vector vecWorldPoints[ ] = {
			Vector( vecMinimum.x, vecMinimum.y, vecMinimum.z ),
			Vector( vecMinimum.x, vecMaximum.y, vecMinimum.z ),
			Vector( vecMaximum.x, vecMaximum.y, vecMinimum.z ),
			Vector( vecMaximum.x, vecMinimum.y, vecMinimum.z ),
			Vector( vecMaximum.x, vecMaximum.y, vecMaximum.z ),
			Vector( vecMinimum.x, vecMaximum.y, vecMaximum.z ),
			Vector( vecMinimum.x, vecMinimum.y, vecMaximum.z ),
			Vector( vecMaximum.x, vecMinimum.y, vecMaximum.z )
		};

		Vector vecScreenPoints[ TRANSFORMATION_POINTS ] { };
		auto bPointFound = true;

		for ( auto i = 0; i < TRANSFORMATION_POINTS; i++ )
			bPointFound &= SDK::Utilities::WorldToScreen( SDK::Utilities::TransformVector( vecWorldPoints[ i ], mtxTransformation ), vecScreenPoints[ i ] );

		if ( !bPointFound )
			return false;

		auto flLeft = vecScreenPoints[ 0 ].x,
			flTop = vecScreenPoints[ 0 ].y,
			flRight = vecScreenPoints[ 0 ].x,
			flBottom = vecScreenPoints[ 0 ].y;

		for ( auto i = 1; i < TRANSFORMATION_POINTS; i++ )
		{
			if ( flLeft > vecScreenPoints[ i ].x )
				flLeft = vecScreenPoints[ i ].x;

			if ( flTop > vecScreenPoints[ i ].y )
				flTop = vecScreenPoints[ i ].y;

			if ( flRight < vecScreenPoints[ i ].x )
				flRight = vecScreenPoints[ i ].x;

			if ( flBottom < vecScreenPoints[ i ].y )
				flBottom = vecScreenPoints[ i ].y;
		}

		_Context.recBoundingBox = { int( flLeft ), int( flTop ), int( flRight ), int( flBottom ) };
		return true;
	}

	void BoundingBox( )
	{
		if ( !_Context.pConfig->bBoundingBox )
			return;

		SDK::pSurface->DrawSetColor( _Context.pConfig->clrBoundingBox[ _Context.iVisibilityIndex ] );
		SDK::pSurface->DrawOutlinedRect( _Context.recBoundingBox.left, _Context.recBoundingBox.top, _Context.recBoundingBox.right, _Context.recBoundingBox.bottom );
	}

	void Snaplines( )
	{
		if ( !_Context.pConfig->bSnaplines )
			return;

		int iWindowWidth, iWindowHeight;

		SDK::pEngineClient->GetScreenSize( iWindowWidth, iWindowHeight );
		SDK::pSurface->DrawSetColor( _Context.pConfig->clrSnapline[ _Context.iVisibilityIndex ] );
		SDK::pSurface->DrawLine( iWindowWidth / 2, iWindowHeight, ( _Context.recBoundingBox.left + _Context.recBoundingBox.right ) / 2, _Context.recBoundingBox.bottom );
	}

	void Information( )
	{
		constexpr auto fnDrawInformation = [ ]( int iOrientation, int& iUsedSpace, const char* szText, const Color& clrText, const RECT& recBox, int iPadding )
		{
			const auto sText = strlen( szText );
			const auto wszNewText = new wchar_t[ sText + 1 ];
			int iWidth, iHeight;

			memset( wszNewText, 0, sizeof( wchar_t ) * ( sText + 1 ) );
			mbstowcs( wszNewText, szText, sText );
			SDK::pSurface->GetTextSize( fntESP, wszNewText, iWidth, iHeight );
			SDK::pSurface->DrawSetTextColor( _Context.pConfig->clrInformation[ _Context.iVisibilityIndex ] );
			SDK::pSurface->DrawSetTextFont( fntESP );

			switch( iOrientation )
			{
				case Config::config_t::esp_t::player_t::LEFT:
				{
					SDK::pSurface->DrawSetTextPos( recBox.left - iWidth - iPadding, recBox.top + iUsedSpace );
				}
				break;

				case Config::config_t::esp_t::player_t::TOP:
				{
					SDK::pSurface->DrawSetTextPos( ( recBox.left + recBox.right ) / 2 - iWidth / 2, recBox.top - _Context.pConfig->iInformationPadding - iUsedSpace - iHeight );
				}
				break;

				case Config::config_t::esp_t::player_t::RIGHT:
				{
					SDK::pSurface->DrawSetTextPos( recBox.right + _Context.pConfig->iInformationPadding, recBox.top + iUsedSpace );
				}
				break;

				case Config::config_t::esp_t::player_t::BOTTOM:
				{
					SDK::pSurface->DrawSetTextPos( ( recBox.left + recBox.right ) / 2 - iWidth / 2, recBox.bottom + _Context.pConfig->iInformationPadding + iUsedSpace );
				}
				break;

				default:
					break;
			}

			SDK::pSurface->DrawPrintText( wszNewText, sText );
			iUsedSpace += iHeight + _Context.pConfig->iInformationPadding;

			delete[ ] wszNewText;
		};

		auto iUsedSpace = 0;

		if ( _Context.pConfig->bInformation[ Config::config_t::esp_t::player_t::NAME ] )
			fnDrawInformation( _Context.pConfig->iInformationOrientation, iUsedSpace, _Context.pEntity->GetInfo( ).szName, _Context.pConfig->clrInformation[ _Context.iVisibilityIndex ], _Context.recBoundingBox, _Context.pConfig->iInformationPadding );

		if ( _Context.pConfig->bInformation[ Config::config_t::esp_t::player_t::HEALTH ] )
		{
			const auto strHealth = "HP: " + std::to_string( _Context.pEntity->m_iHealth( ) );
			fnDrawInformation( _Context.pConfig->iInformationOrientation, iUsedSpace, strHealth.c_str( ), _Context.pConfig->clrInformation[ _Context.iVisibilityIndex ], _Context.recBoundingBox, _Context.pConfig->iInformationPadding );
		}

		if ( _Context.pConfig->bInformation[ Config::config_t::esp_t::player_t::WEAPON ] )
			fnDrawInformation( _Context.pConfig->iInformationOrientation, iUsedSpace, _Context.pEntity->GetWeaponName( ), _Context.pConfig->clrInformation[ _Context.iVisibilityIndex ], _Context.recBoundingBox, _Context.pConfig->iInformationPadding );
	}
}
