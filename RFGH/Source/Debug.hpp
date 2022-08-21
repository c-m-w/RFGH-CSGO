/// Debug.hpp

#pragma once

namespace Debug
{
	enum
	{
		PREFIX_ERROR,
		PREFIX_WARNING,
		PREFIX_SUCCESS,
		PREFIX_MAX
	};

	enum
	{
		LOCATION_MAIN,
		LOCATION_SDK,
		LOCATION_FEATURES,
		LOCATION_CONFIG,
		LOCATION_DEBUG,
		LOCATION_FILES,
		LOCATION_HOOKS,
		LOCATION_INPUT,
		LOCATION_MENU,
		LOCATION_MAX
	};

	void Setup( );
	void Close( );
	void Log( int iPrefix, int iLocation, std::string strLog );
	void Log( int iPrefix, int iLocation, const char* szFormat, ... );
}

using namespace Debug;
