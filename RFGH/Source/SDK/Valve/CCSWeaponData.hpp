#pragma once

class CCSWeaponData
{
public:

	char _0x0000[ 20 ];
	int iClip1;			//0x0014
	char _0x0018[ 12 ];
	int max_reserved_ammo;	//0x0024
	char _0x0028[ 96 ];
	char* hud_name;				//0x0088
	char* weapon_name;			//0x008C
	char _0x0090[ 56 ];
	int weapon_type;
	char _0x00CC[ 4 ];
	int price;				//0x00D0
	int reward;				//0x00D4
	char _0x00D8[ 20 ];
	bool full_auto;				//0x00EC
	char _0x00ED[ 3 ];
	int iDamage;				//0x00F0
	float flArmorRatio;			//0x00F4
	int bullets;			//0x00F8
	float flPenetration;			//0x00FC
	char _0x0100[ 8 ];
	float flRange;				//0x0108
	float flRangeModifier;		//0x010C
	char _0x0110[ 16 ];
	bool bHasSilencer;				//0x0120
	char _0x0121[ 15 ];
	float max_speed;			//0x0130
	float max_speed_alt;		//0x0134
	char _0x0138[ 76 ];
	int recoil_seed;		//0x0184
	char _0x0188[ 32 ];
};
