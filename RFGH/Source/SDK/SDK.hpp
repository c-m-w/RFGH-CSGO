/// SDK.hpp

#pragma once

#include "Valve/Valve.hpp"
#include "Utilities.hpp"

namespace SDK
{
	inline HMODULE hClient { };
	inline HMODULE hEngine { };
	inline HMODULE hVGUI { };
	inline HMODULE hVGUI2 { };

	inline CreateInterfaceFn fnciClient = nullptr;
	inline CreateInterfaceFn fnciEngine = nullptr;
	inline CreateInterfaceFn fnciVGUI = nullptr;
	inline CreateInterfaceFn fnciVGUI2 = nullptr;

	inline IClientEntityList* pEntityList = nullptr;
	inline ISurface* pSurface = nullptr;
	inline IPanel* pPanel = nullptr;
	inline IVEngineClient* pEngineClient = nullptr;
	inline void* pClientBase = nullptr;
	inline IVModelInfoClient* pModelInfo = nullptr;
	inline IEngineTrace* pEngineTrace = nullptr;
	inline CGlobalVars* pGlobalVars = nullptr;
	inline glow_object_manager_t* pGlowObjectManager = nullptr;
	inline void* pClientMode = nullptr;
	inline CClientState* pClientState = nullptr;

	namespace Offsets
	{
		constexpr std::ptrdiff_t GLOBAL_VARS = 0x58BA00,
			GLOW_OBJECT_MANAGER = 0x520DA28,
			CLIENT_STATE = 0x58BCFC,
			CLASS_ID = 0x14,
			VIEW_OFFSET = 0x108,
			TEAM = 0xF4,
			ORIGIN = 0x138,
			EYE_ANGLES = 0xB32C,
			HEALTH = 0x100,
			LIFE_STATE = 0x25F,
			COORDINATE_FRAME = 0x444,
			NEXT_PRIMARY_ATTACK = 0x3218,
			CLIP1 = 0x3244,
			IN_RELOAD = 0x3285,
			FLAGS = 0x104,
			IS_SCOPED = 0x390A,
			AIM_PUNCH = 0x302C,
			ITEM_DEFINITION_INDEX = 0x1EA,
			ATTRIBUTE_MANAGER = 0x2D80,
			ITEM = 0x40,
			ACTIVE_WEAPON = 0x2EF8;
	}

	namespace Indices
	{
		constexpr std::size_t IS_PLAYER = 153,
			GET_WEAPON_DATA = 448,
			PAINT_TRAVERSE = 41,
			CREATE_MOVE = 24,
			DO_POST_SPACE_SCREEN_EFFECTS = 44;
	}

	class C_BaseEntity: public IClientEntity
	{
	public:

		int GetClassID( );
		bool IsPlayer( );

		Vector& m_vecOrigin( );
		QAngle& m_angEyeAngles( );
		matrix3x4_t& m_rgflCoordinateFrame( );
	};

	class C_EconItemView
	{
	public:

		int& m_iItemDefinitionIndex( );
	};

	class C_AttributeManager
	{
	public:

		C_EconItemView& m_Item( );
	};

	class C_BaseCombatWeapon: public C_BaseEntity
	{
	public:

		CCSWeaponData* GetWeaponData( );
		bool Reloading( );
		bool AvailableAmmo( );
		bool CanFire( );

		int m_iClip1( );
		float m_flNextPrimaryAttack( );
		C_AttributeManager& m_AttributeManager( );
	};

	class C_BasePlayer: public C_BaseEntity
	{
	public:

		bool PlayerOnTeam( C_BasePlayer* pPlayer );
		player_info_t GetInfo( );
		const char* GetWeaponName( );
		Vector GetHitboxPosition( int iHitbox );
		Vector GetEyePosition( );
		void TraceViewRay( const QAngle& angAngle, CGameTrace& gtRay );
		bool CanSeePosition( const Vector& vecPosition, IClientEntity* pEntity = nullptr );
		bool CanSeePlayer( C_BasePlayer* pPlayer );
		bool IsAlive( );

		Vector& m_vecViewOffset( );
		int& m_iTeamNum( );
		int& m_iHealth( );
		ELifeState& m_lifeState( );
		C_BaseCombatWeapon* m_hActiveWeapon( );
		unsigned& m_fFlags( );
		bool& m_bIsScoped( );
		QAngle& m_aimPunchAngle( );
	};

	using player_ptr_t = C_BasePlayer*;

	bool Setup( );
}
