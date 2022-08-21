/// SDK.cpp

#include "../RFGH.hpp"

namespace SDK
{
	const Utilities::signature_t sigClientMode { { 0xA1, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x80, 0x00, 0x00, 0x00, 0x00, 0x5D }, 1 };
	const Utilities::signature_t sigInput { { 0xB9, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x40, 0x38, 0xFF, 0xD0, 0x84, 0xC0, 0x0F, 0x85 }, 1 };

	int C_BaseEntity::GetClassID( )
	{
		return *reinterpret_cast< int* >( uintptr_t( GetClientClass( ) ) + Offsets::CLASS_ID );
	}

	bool C_BaseEntity::IsPlayer( )
	{
		return reinterpret_cast< bool( __thiscall* )( IClientEntity* ) >( ( *reinterpret_cast< void*** >( this ) )[ Indices::IS_PLAYER ] )( this );
	}

	Vector& C_BaseEntity::m_vecOrigin( )
	{
		return *reinterpret_cast< Vector* >( uintptr_t( this ) + Offsets::ORIGIN );
	}

	QAngle& C_BaseEntity::m_angEyeAngles( )
	{
		return *reinterpret_cast< QAngle* >( uintptr_t( this ) + Offsets::EYE_ANGLES );
	}

	matrix3x4_t& C_BaseEntity::m_rgflCoordinateFrame( )
	{
		return *reinterpret_cast< matrix3x4_t* >( uintptr_t( this ) + Offsets::COORDINATE_FRAME );
	}

	int& C_EconItemView::m_iItemDefinitionIndex( )
	{
		return *reinterpret_cast< int* >( uintptr_t( this ) + Offsets::ITEM_DEFINITION_INDEX );
	}

	C_EconItemView& C_AttributeManager::m_Item( )
	{
		return *reinterpret_cast< C_EconItemView* >( uintptr_t( this ) + Offsets::ITEM );
	}

	CCSWeaponData* C_BaseCombatWeapon::GetWeaponData( )
	{
		return reinterpret_cast< CCSWeaponData*( __thiscall* )( void* ) >( ( *reinterpret_cast< void*** >( this ) )[ Indices::GET_WEAPON_DATA ] )( this );
	}

	bool C_BaseCombatWeapon::Reloading( )
	{
		return *reinterpret_cast< bool* >( uintptr_t( this ) + Offsets::IN_RELOAD );
	}

	bool C_BaseCombatWeapon::AvailableAmmo( )
	{
		return m_iClip1( ) > 0 && !Reloading( );
	}

	bool C_BaseCombatWeapon::CanFire( )
	{
		return AvailableAmmo( ) && m_flNextPrimaryAttack( ) <= pGlobalVars->curtime;
	}

	int C_BaseCombatWeapon::m_iClip1( )
	{
		return *reinterpret_cast< int* >( uintptr_t( this ) + Offsets::CLIP1 );
	}

	float C_BaseCombatWeapon::m_flNextPrimaryAttack( )
	{
		return *reinterpret_cast< float* >( uintptr_t( this ) + Offsets::NEXT_PRIMARY_ATTACK );
	}

	C_AttributeManager& C_BaseCombatWeapon::m_AttributeManager( )
	{
		return *reinterpret_cast< C_AttributeManager* >( uintptr_t( this ) + Offsets::ATTRIBUTE_MANAGER );
	}

	bool C_BasePlayer::PlayerOnTeam( C_BasePlayer* pPlayer )
	{
		return m_iTeamNum( ) == pPlayer->m_iTeamNum( );
	}

	player_info_t C_BasePlayer::GetInfo( )
	{
		player_info_t piReturn { };

		pEngineClient->GetPlayerInfo( EntIndex( ), &piReturn );
		return piReturn;
	}

	const char* C_BasePlayer::GetWeaponName( )
	{
		const auto hActiveWeapon = m_hActiveWeapon( );
		if ( hActiveWeapon == nullptr )
			return "None";

		switch( hActiveWeapon->m_AttributeManager( ).m_Item( ).m_iItemDefinitionIndex( ) ) // probably an extremely shit way of doign this
		{
			case ITEM_NONE:
			{
				return "Unarmed";
			}

			case ITEM_DEAGLE:
			{
				return "Deagle";
			}

			case ITEM_ELITE:
			{
				return "Elites";
			}

			case ITEM_FIVESEVEN:
			{
				return "Five-SeveN";
			}

			case ITEM_GLOCK:
			{
				return "Glock";
			}

			case ITEM_AK47:
			{
				return "AK-47";
			}

			case ITEM_AUG:
			{
				return "AUG";
			}

			case ITEM_AWP:
			{
				return "AWP";
			}

			case ITEM_FAMAS:
			{
				return "FAMAS";
			}

			case ITEM_G3SG1:
			{
				return "G3SG1";
			}

			case ITEM_GALILAR:
			{
				return "Galil";
			}

			case ITEM_M249:
			{
				return "M249";
			}

			case ITEM_M4A1:
			{
				return "M4A4";
			}

			case ITEM_MAC10:
			{
				return "Mac-10";
			}

			case ITEM_P90:
			{
				return "P90";
			}

			case ITEM_MP5SD:
			{
				return "MP5";
			}

			case ITEM_UMP45:
			{
				return "UMP";
			}

			case ITEM_XM1014:
			{
				return "XM1014";
			}

			case ITEM_PPBIZON:
			{
				return "PP-Bizon";
			}

			case ITEM_MAG7:
			{
				return "MAG-7";
			}

			case ITEM_NEGEV:
			{
				return "Negev";
			}

			case ITEM_SAWEDOFF:
			{
				return "Sawed-off";
			}

			case ITEM_TEC9:
			{
				return "Tec-9";
			}

			case ITEM_TASER:
			{
				return "Zeus";
			}

			case ITEM_P2000:
			{
				return "P2000";
			}

			case ITEM_MP7:
			{
				return "MP7";
			}

			case ITEM_MP9:
			{
				return "MP9";
			}

			case ITEM_NOVA:
			{
				return "Nova";
			}

			case ITEM_P250:
			{
				return "P250";
			}

			case ITEM_SCAR20:
			{
				return "SCAR-20";
			}

			case ITEM_SG553:
			{
				return "SG-553";
			}

			case ITEM_SSG08:
			{
				return "SSG-08";
			}

			case ITEM_CTKNIFE1:
			case ITEM_CTKNIFE2:
			case ITEM_TKNIFE:
			{
				return "Knife";
			}

			case ITEM_FLASHBANG:
			{
				return "Flashbang";
			}

			case ITEM_HEGRENADE:
			{
				return "HE Grenade";
			}

			case ITEM_SMOKEGRENADE:
			{
				return "Smoke";
			}

			case ITEM_MOLOTOV:
			{
				return "Molly";
			}

			case ITEM_DECOY:
			{
				return "Decoy";
			}

			case ITEM_INCGRENADE:
			{
				return "Incendiary";
			}

			case ITEM_C4:
			{
				return "Bomb";
			}

			case ITEM_HEALTHSHOT:
			{
				return "Healthshot";
			}

			case ITEM_M4A1S:
			{
				return "M4A1-S";
			}

			case ITEM_USPS:
			{
				return "USP-S";
			}

			case ITEM_CZ75AUTO:
			{
				return "CZ75";
			}

			case ITEM_R8REVOLVER:
			{
				return "R8";
			}

			case ITEM_TACTICALAWARENESSGRENADE:
			{
				return "Tac Grenade";
			}

			case ITEM_FISTS:
			{
				return "Fists";
			}

			case ITEM_BREACHCHARGE:
			{
				return "Breach Charge";
			}

			case ITEM_TABLET:
			{
				return "Tablet";
			}

			case ITEM_MELEE:
			{
				return "Melee";
			}

			case ITEM_AXE:
			{
				return "Axe";
			}

			case ITEM_HAMMER:
			{
				return "Hammer";
			}

			case ITEM_SPANNER:
			{
				return "Spanner";
			}

			case ITEM_KNIFE_GHOST:
			{
				return "Ghost Knife";
			}

			case ITEM_FIREBOMB:
			{
				return "Firebomb";
			}

			case ITEM_DIVERSION:
			{
				return "Diversion";
			}

			case ITEM_FRAG_GRENADE:
			{
				return "Frag Grenade";
			}

			case ITEM_KNIFE_BAYONET:
			{
				return "Bayonet";
			}

			case ITEM_KNIFE_FLIP:
			{
				return "Flip Knife";
			}

			case ITEM_KNIFE_GUT:
			{
				return "Gut Knife";
			}

			case ITEM_KNIFE_KARAMBIT:
			{
				return "Karambit";
			}

			case ITEM_KNIFE_M9_BAYONET:
			{
				return "M9 Bayonet";
			}

			case ITEM_KNIFE_TACTICAL:
			{
				return "Huntsman Knife";
			}

			case ITEM_KNIFE_FALCHION:
			{
				return "Falchion Knife";
			}

			case ITEM_KNIFE_SURVIVAL_BOWIE:
			{
				return "Bowie Knife";
			}

			case ITEM_KNIFE_BUTTERFLY:
			{
				return "Butterfly Knife";
			}

			case ITEM_KNIFE_SHADOW_DAGGERS:
			{
				return "Shadow Daggers";
			}

			case ITEM_KNIFE_URSUS:
			{
				return "Ursus";
			}

			case ITEM_KNIFE_NAVAJA:
			{
				return "Navaja";
			}

			case ITEM_KNIFE_STILLETTO:
			{
				return "Stilletto";
			}

			case ITEM_KNIFE_TALON:
			{
				return "Talon";
			}

			default:
			{
				return "Unknown Weapon";
			}
		}
	}

	Vector C_BasePlayer::GetHitboxPosition( int iHitbox )
	{
		matrix3x4_t mtxBones[ MAXSTUDIOBONES ] { };

		if ( !SetupBones( mtxBones, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.f ) )
			return { };

		const auto pModel = pModelInfo->GetStudiomodel( GetModel( ) );
		if ( pModel == nullptr )
			return { };

		const auto pHitboxSet = pModel->GetHitboxSet( 0 );
		if ( pHitboxSet == nullptr )
			return { };

		const auto pHitbox = pHitboxSet->GetHitbox( iHitbox );
		if ( nullptr == pHitbox )
			return { };

		return 1.f / ( 2.f / ( Utilities::TransformVector( pHitbox->bbmin, mtxBones[ pHitbox->bone ] ) + Utilities::TransformVector( pHitbox->bbmax, mtxBones[ pHitbox->bone ] ) ) );
	}

	Vector C_BasePlayer::GetEyePosition( )
	{
		return m_vecOrigin( ) + m_vecViewOffset( );
	}

	void C_BasePlayer::TraceViewRay( const QAngle& angAngle, CGameTrace& gtRay )
	{
		const auto vecStart = GetEyePosition( ),
				vecEnd = Utilities::TransformAngle( angAngle ) * 8192.f + vecStart;

		Ray_t rayTrace;
		CTraceFilter tfFilter;

		rayTrace.Init( vecStart, vecEnd );
		tfFilter.pSkip = this;
		pEngineTrace->TraceRay( rayTrace, MASK_SHOT, &tfFilter, &gtRay );
	}

	bool C_BasePlayer::CanSeePosition( const Vector& vecPosition, IClientEntity* pEntity /*= nullptr*/ )
	{
		Ray_t rayTrace;
		CTraceFilter tfFilter;
		CGameTrace gtRay;

		rayTrace.Init( GetEyePosition( ), vecPosition );
		tfFilter.pSkip = this;
		pEngineTrace->TraceRay( rayTrace, MASK_SHOT, &tfFilter, &gtRay );

		return gtRay.fraction == 1.f
			|| gtRay.hit_entity != nullptr && gtRay.hit_entity->EntIndex( ) == pEntity->EntIndex( );
	}

	bool C_BasePlayer::CanSeePlayer( C_BasePlayer* pPlayer )
	{
		return CanSeePosition( pPlayer->GetHitboxPosition( HITBOX_HEAD ), pPlayer )
			|| CanSeePosition( pPlayer->GetHitboxPosition( HITBOX_LEFT_FOOT ), pPlayer )
			|| CanSeePosition( pPlayer->GetHitboxPosition( HITBOX_RIGHT_FOOT ), pPlayer );
	}

	bool C_BasePlayer::IsAlive( )
	{
		return m_lifeState( ) == LIFE_ALIVE;
	}

	Vector& C_BasePlayer::m_vecViewOffset( )
	{
		return *reinterpret_cast< Vector* >( uintptr_t( this ) + Offsets::VIEW_OFFSET );
	}

	int& C_BasePlayer::m_iTeamNum( )
	{
		return *reinterpret_cast< int* >( uintptr_t( this ) + Offsets::TEAM );
	}

	int& C_BasePlayer::m_iHealth( )
	{
		return *reinterpret_cast< int* >( uintptr_t( this ) + Offsets::HEALTH );
	}

	ELifeState& C_BasePlayer::m_lifeState( )
	{
		return *reinterpret_cast< ELifeState* >( uintptr_t( this ) + Offsets::LIFE_STATE );
	}

	C_BaseCombatWeapon* C_BasePlayer::m_hActiveWeapon( )
	{
		const auto iIndex = *reinterpret_cast< CBaseHandle* >( uintptr_t( this ) + Offsets::ACTIVE_WEAPON ) & 0xFFF;
		return reinterpret_cast< C_BaseCombatWeapon* >( pEntityList->GetClientEntity( iIndex ) );
	}

	unsigned& C_BasePlayer::m_fFlags( )
	{
		return *reinterpret_cast< unsigned* >( uintptr_t( this ) + Offsets::FLAGS );
	}

	bool& C_BasePlayer::m_bIsScoped( )
	{
		return *reinterpret_cast< bool* >( uintptr_t( this ) + Offsets::IS_SCOPED );
	}

	QAngle& C_BasePlayer::m_aimPunchAngle( )
	{
		return *reinterpret_cast< QAngle* >( uintptr_t( this ) + Offsets::AIM_PUNCH );
	}

	bool Setup( )
	{
		if ( nullptr != ( hClient = GetModuleHandle( L"client_panorama.dll" ) )
			&& nullptr != ( hEngine = GetModuleHandle( L"engine.dll" ) )
			&& nullptr != ( hVGUI = GetModuleHandle( L"vguimatsurface.dll" ) )
			&& nullptr != ( hVGUI2 = GetModuleHandle( L"vgui2.dll" ) )
			&& nullptr != ( fnciClient = reinterpret_cast< CreateInterfaceFn >( GetProcAddress( hClient, "CreateInterface" ) ) )
			&& nullptr != ( fnciEngine = reinterpret_cast< CreateInterfaceFn >( GetProcAddress( hEngine, "CreateInterface" ) ) )
			&& nullptr != ( fnciVGUI = reinterpret_cast< CreateInterfaceFn >( GetProcAddress( hVGUI, "CreateInterface" ) ) )
			&& nullptr != ( fnciVGUI2 = reinterpret_cast< CreateInterfaceFn >( GetProcAddress( hVGUI2, "CreateInterface" ) ) )
			&& nullptr != ( pEntityList = reinterpret_cast< IClientEntityList* >( fnciClient( "VClientEntityList003", nullptr ) ) )
			&& nullptr != ( pEngineClient = reinterpret_cast< IVEngineClient* >( fnciEngine( "VEngineClient014", nullptr ) ) )
			&& nullptr != ( pSurface = reinterpret_cast< ISurface* >( fnciVGUI( "VGUI_Surface031", nullptr ) ) )
			&& nullptr != ( pPanel = reinterpret_cast< IPanel* >( fnciVGUI2( "VGUI_Panel009", nullptr ) ) )
			&& nullptr != ( pClientBase = fnciClient( "VClient018", nullptr ) )
			&& nullptr != ( pModelInfo = reinterpret_cast< IVModelInfoClient* >( fnciEngine( "VModelInfoClient004", nullptr ) ) )
			&& nullptr != ( pEngineTrace = reinterpret_cast< IEngineTrace* >( fnciEngine( "EngineTraceClient004", nullptr ) ) )
			&& nullptr != ( pGlobalVars = reinterpret_cast< decltype( pGlobalVars ) >( uintptr_t( hEngine ) + Offsets::GLOBAL_VARS ) )
			&& nullptr != ( pGlowObjectManager = reinterpret_cast< decltype( pGlowObjectManager ) >( uintptr_t( hClient ) + Offsets::GLOW_OBJECT_MANAGER ) )
			&& nullptr != ( pClientMode = *reinterpret_cast< void** >( FindPattern( hClient, sigClientMode ) ) )
			&& nullptr != ( pClientState = *reinterpret_cast< decltype( pClientState )* >( uintptr_t( hEngine ) + Offsets::CLIENT_STATE ) ) )
		{
			Log( PREFIX_SUCCESS, LOCATION_SDK, "Found interfaces." );
			return true;
		}

		Log( PREFIX_ERROR, LOCATION_SDK, "Failed to find interfaces." );
		return false;
	}
}
