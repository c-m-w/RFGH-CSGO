/// Config.hpp

#pragma once

namespace Config
{
	inline struct general_t
	{
		constexpr static auto DEFAULT_MENU_KEY = DWORD( VK_INSERT );
		enum
		{
			INPUT_BLOCK_NONE,
			INPUT_BLOCK_PROCESSED,
			INPUT_BLOCK_ALL
		};

		int& GetWindowX( );
		int& GetWindowY( );
		int& GetInputMode( );
		DWORD& GetMenuKey( );
		void SetWindowX( int iNewX );
		void SetWindowY( int iNewY );
		void SetInputMode( int iNewInputMode );
		void SetMenuKey( DWORD dwNewKey );

		__declspec( property( get = GetWindowX, put = SetWindowX ) ) int iWindowX;
		__declspec( property( get = GetWindowY, put = SetWindowY ) ) int iWindowY;
		__declspec( property( get = GetInputMode, put = SetInputMode ) ) int iInputMode;
		__declspec( property( get = GetMenuKey, put = SetMenuKey ) ) DWORD dwMenuKey;
	} _General;

	inline struct config_t
	{
		const static inline auto clrDefault = Color( 255, 255, 255, 255 );
		enum
		{
			TEAM,
			ENEMY,
			VISIBLE = 0,
			INVISIBLE,
			MAX
		};

		struct esp_t
		{
			bool bGlobalEnable = false;
			struct player_t
			{
				enum
				{
					NAME,
					HEALTH,
					WEAPON,
					INFORMATION_MAX
				};

				enum
				{
					LEFT,
					TOP,
					RIGHT,
					BOTTOM
				};

				bool bEnabled[ MAX ] { };
				bool bBoundingBox = false;
				Color clrBoundingBox[ MAX ] { clrDefault, clrDefault };
				bool bSnaplines = false;
				Color clrSnapline[ MAX ] { clrDefault, clrDefault };
				bool bInformation[ INFORMATION_MAX ] { false, false, false };
				int iInformationOrientation = LEFT;
				int iInformationPadding = 0;
				Color clrInformation[ MAX ] { clrDefault, clrDefault };
			} _Players[ MAX ];
		} _ESP;

		struct glow_t
		{
			bool bGlobalEnable = false;
			struct player_t
			{
				bool bEnabled[ MAX ] { };
				int iGlowStyle = 0;
				Color clrGlow[ MAX ] { clrDefault, clrDefault };
				bool bFullBloom = false;
				float flFullBloomAmount = 0.f;
			} _Players[ MAX ];
		} _Glow;

		struct miscellaneous_visuals_t
		{
			bool bVisualizeAimbotRange = false;
			Color clrAimbotRange = clrDefault;
		} _MiscellaneousVisuals;

		struct aimbot_t
		{
			bool bGlobalEnable = false;
			struct weapon_t
			{
				enum
				{
					CROSSHAIR_DISTANCE,
					DISTANCE,
					HEALTH
				};

				bool bEnabled[ MAX ] { };
				float flMaxCrosshairDistance = 0.1f;
				float flAimTime = 1.f;
				bool bSilentAim = false;
				int iTargetingMode = CROSSHAIR_DISTANCE;
				bool bOnlyScoped = false;
				int iPriorityHitbox = HITBOX_HEAD;
				int iReferenceHitbox = HITBOX_CHEST;
				bool bHitboxes[ HITBOX_MAX ] { };
				DWORD dwKey = VK_XBUTTON1;
				bool bControlRecoil = false;
				float flRecoilAccuracy = 0.01f;
			} _Weapons[ 7 /*WEAPONTYPE_MAX*/ ];
		} _Aimbot;

		struct triggerbot_t
		{
			bool bGlobalEnable = false;
			struct weapon_t
			{
				bool bEnabled[ MAX ] { };
				bool bOnlyScoped = false;
				bool bHitboxes[ HITBOX_MAX ] { };
				DWORD dwKey = VK_XBUTTON1;
			} _Weapons[ 7 /*WEAPONTYPE_MAX*/ ];
		} _Triggerbot;

		struct miscellaneous_t
		{
			struct auto_jump_t
			{
				bool bEnabled = false;
				DWORD dwKey = VK_SPACE;
			} _AutoJump;
		} _Miscellaneous;
	} _Settings;

	bool Setup( );
	bool Load( const std::string& strConfig );
	bool Save( const std::string& strConfig );
	bool Remove( const std::string& strConfig );
	bool Create( const std::string& strConfig );
	std::vector< std::wstring > GetConfigs( );
}
