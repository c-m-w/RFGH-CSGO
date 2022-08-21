#pragma once

#include "Vector.hpp"
#include "QAngle.hpp"

class CUserCmd
{
public:
	virtual ~CUserCmd( )
	{ };
	int       command_number;     // For matching server and client commands for debugging
	int       tick_count;         // The tick the client created this command
	QAngle    viewangles;         // Player instantaneous view angles.
	QAngle    aimdirection;       // 
	float     forwardmove;        // Forward velocity
	float     sidemove;           // Sideways velocity
	float     upmove;             // Upward velocity
	int       buttons;            // Attack button states
	byte      impulse;            // Impulse command issued.
	int       weaponselect;       // Current weapon id
	int       weaponsubtype;      // 
	int       random_seed;        // For shared random functions
	short     mousedx;            // Mouse accum in x from create move
	short     mousedy;            // Mouse accum in y from create move
	bool      hasbeenpredicted;   // Client only, tracks whether we've predicted this command at least once
	char      pad_0x4C[ 0x18 ];
};

class CVerifiedUserCmd
{
public:

	CUserCmd m_cmd;
	CRC32_t  m_crc;
};
