/// Hooks.cpp

#include "RFGH.hpp"

namespace Hooks
{
	CHook* hkSurface = nullptr;
	CHook* hkClientMode = nullptr;

	std::size_t CHook::GetTableLength( void* pTable )
	{
		auto sReturn = std::size_t( 0u );
		MEMORY_BASIC_INFORMATION mbiTable { };

		while ( NULL != VirtualQuery( reinterpret_cast< void** >( pTable )[ sReturn ], &mbiTable, sizeof mbiTable ) &&
				mbiTable.BaseAddress != nullptr &&
				mbiTable.Type != NULL &&
				mbiTable.Protect & PAGE_EXECUTE_READ &&
				!( mbiTable.Protect & PAGE_GUARD ) )
			sReturn++;

		Log( PREFIX_SUCCESS, LOCATION_HOOKS, "Found %u functions in virtual table %08X.", sReturn, pTable );
		return sReturn;
	}

	CHook::CHook( void* pInterface ): pOrigin( pInterface )
	{
		pOldTable = *reinterpret_cast< void** >( pInterface );
		sTable = GetTableLength( pOldTable );

		pNewTable = new void*[ sTable ];
		memcpy( pNewTable, pOldTable, sTable * sizeof( void* ) );
		*reinterpret_cast< void** >( pInterface ) = pNewTable;
		Log( PREFIX_SUCCESS, LOCATION_HOOKS, "Replacing table %08X with table %08X in interface %08X.", pOldTable, pNewTable, pInterface );
	}

	CHook::~CHook( )
	{
		*reinterpret_cast< void** >( pOrigin ) = pOldTable;
		delete[ ] pNewTable;
	}

	bool CHook::HookIndex( std::size_t sIndex, void* pFunction )
	{
		if ( sIndex < 0 || sIndex > sTable )
		{
			Log( PREFIX_SUCCESS, LOCATION_HOOKS, "Index %u is out of range for table %08X (max %u).", sIndex, pNewTable, sTable );
			return false;
		}

		Log( PREFIX_SUCCESS, LOCATION_HOOKS, "Replacing function %08X with function %08X at index %u in table %08X.", pNewTable[ sIndex ], pFunction, sIndex, pNewTable );
		pNewTable[ sIndex ] = pFunction;
		return true;
	}

	void* CHook::GetOriginalFunction( std::size_t sIndex )
	{
		return reinterpret_cast< void** >( pOldTable )[ sIndex ];
	}

	void __stdcall PaintTraverse( vgui::VPANEL panel, bool forceRepaint, bool allowForce );
	bool __stdcall CreateMove( float flInputSampleFrametime, CUserCmd* pCmd );
	void __stdcall DoPostSpaceScreenEffects( int iUnknown );

	bool Setup( )
	{
		hkSurface = new CHook( SDK::pPanel );
		hkClientMode = new CHook( SDK::pClientMode );

		if ( hkSurface->HookIndex( SDK::Indices::PAINT_TRAVERSE, PaintTraverse )
			&& hkClientMode->HookIndex( SDK::Indices::CREATE_MOVE, CreateMove )
			&& hkClientMode->HookIndex( SDK::Indices::DO_POST_SPACE_SCREEN_EFFECTS, DoPostSpaceScreenEffects ) )
		{
			Log( PREFIX_SUCCESS, LOCATION_HOOKS, "Set hooks." );
			return true;
		}

		Log( PREFIX_ERROR, LOCATION_HOOKS, "Could not set hooks." );
		return false;
	}

	void Close( )
	{
		delete hkSurface;
		delete hkClientMode;
	}

	void __stdcall PaintTraverse( vgui::VPANEL panel, bool forceRepaint, bool allowForce )
	{
		static auto fnOriginal = reinterpret_cast< void( __thiscall* )( IPanel* ,vgui::VPANEL, bool, bool ) >( hkSurface->GetOriginalFunction( SDK::Indices::PAINT_TRAVERSE ) );
		static auto vpPanelID = vgui::VPANEL( 0u );

		fnOriginal( SDK::pPanel, panel, forceRepaint, allowForce );

		if ( !vpPanelID && !strcmp( SDK::pPanel->GetName( panel ), "FocusOverlayPanel" ) )
			vpPanelID = panel;

		if ( vpPanelID != panel )
			return;

		Menu::Draw( );
		Features::Visuals::Draw( );
		Features::Visuals::VisualizeAimbotRange( );
	}

	bool __stdcall CreateMove( float flInputSampleFrametime, CUserCmd* pCmd )
	{
		static auto fnOriginal = reinterpret_cast< bool( __thiscall* )( void*, float, void* ) >( hkClientMode->GetOriginalFunction( SDK::Indices::CREATE_MOVE ) );
		const auto bReturn = fnOriginal( SDK::pClientMode, flInputSampleFrametime, pCmd );
		const auto pLocalPlayer = SDK::player_ptr_t( SDK::pEntityList->GetClientEntity( SDK::pEngineClient->GetLocalPlayer( ) ) );

		if ( pCmd == nullptr
			 || pCmd->command_number == 0
			 || nullptr == pLocalPlayer
			 || !SDK::pEngineClient->IsInGame( ) )
			return bReturn;

		Features::Miscellaneous::AutoJump( pLocalPlayer, pCmd );
		Features::Combat::Aimbot( pLocalPlayer, pCmd );
		Features::Combat::Triggerbot( pLocalPlayer, pCmd );

		return false;
	}

	void __stdcall DoPostSpaceScreenEffects( int iUnknown )
	{
		static auto fnOriginal = reinterpret_cast< void( __thiscall* )( void*, int ) >( hkClientMode->GetOriginalFunction( SDK::Indices::DO_POST_SPACE_SCREEN_EFFECTS ) );
		Features::Visuals::GlowObjects( );
		return fnOriginal( SDK::pClientMode, iUnknown );
	}
}
