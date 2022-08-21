#pragma once

#include "Vector.hpp"

namespace SDK
{
	class C_BasePlayer;
}

struct glow_definition_t
{
	SDK::C_BasePlayer* m_pEntity;			//0x0000

	union
	{
		Vector m_vecGlowColor;			//0x0004

		struct
		{
			float   m_flRed;			//0x0004
			float   m_flGreen;			//0x0008
			float   m_flBlue;			//0x000C
		};
	};

	float   m_flAlpha;					//0x0010
	uint8_t pad_0014[ 8 ];				//0x0014
	float   m_flFullBloomAmount;		//0x001C
	uint8_t pad_0020[ 4 ];				//0x0020
	bool    m_bRenderWhenOccluded;		//0x0024
	bool    m_bRenderWhenUnoccluded;	//0x0025
	bool    m_bFullBloomRender;			//0x0026
	uint8_t pad_0027[ 5 ];				//0x0027
	int		m_nGlowStyle;				//0x002C
	int		m_nSplitScreenSlot;			//0x0030
	int		m_nNextFreeSlot;			//0x0034

	void Reset( )
	{
		m_vecGlowColor.Zero( );
		m_flAlpha = m_flFullBloomAmount = 0.f;
		m_bRenderWhenOccluded = m_bRenderWhenUnoccluded = m_bFullBloomRender = false;
		m_nGlowStyle = 0;
	}
};

struct glow_object_manager_t
{
	glow_definition_t*	m_pObjects;	// 0x0
	int					m_iMax;		// 0x4
	char				x8[ 4 ];	// 0x8
	int					m_iCount;	// 0xC
};
