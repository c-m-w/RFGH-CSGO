/// Debug.cpp

#include "RFGH.hpp"

namespace Debug
{
	constexpr char LOG_FILE[ ] = "debug.log";
	constexpr auto MAX_LOG_LENGTH = std::size_t( 1024 );
	constexpr char PREFIXES[ PREFIX_MAX ][ 32 ]
	{
		"[Error]\t\t",
		"[Warning]\t",
		"[Success]\t"
	};

	constexpr char LOCATIONS[ LOCATION_MAX ][ 32 ]
	{
		"[Main]\t\t",
		"[SDK]\t\t",
		"[Features]\t",
		"[Config]\t",
		"[Debug]\t\t",
		"[Files]\t\t",
		"[Hooks]\t\t",
		"[Input]\t\t",
		"[Menu]\t\t"
	};

	std::stringstream ssBuffer { };

	std::string GetTimestamp( )
	{
		char szBuffer[ 32 ];
		auto tmNow = time_t( Time( ) );
		time( &tmNow );
		const auto tmTime = localtime( &tmNow );
		strftime( szBuffer, 80, "%B %e, 20%g | %H:%M:%S", tmTime );
		return szBuffer;
	}

	void WriteBuffer( )
	{
		auto strBuffer = ssBuffer.str( );
		if ( strBuffer.empty( ) )
			return;

		std::string strLog { };
		if ( Files::Read( LOG_FILE, strLog ) )
			strBuffer = strLog + strBuffer;

		Files::Write( LOG_FILE, strBuffer );
		ssBuffer.str( std::string( ) );
	}

	void Setup( )
	{
		ssBuffer << "- Log Begin [" << GetTimestamp( ) << "] -" << std::endl;
	}

	void Close( )
	{
		ssBuffer << "- Log End [" << GetTimestamp( ) << "] -" << std::endl;
		WriteBuffer( );
	}

	void Log( int iPrefix, int iLocation, std::string strLog )
	{
		ssBuffer << "[" << GetTimestamp( ) << "]\t" << PREFIXES[ iPrefix ] << LOCATIONS[ iLocation ] << strLog << std::endl;
	}

	void Log( int iPrefix, int iLocation, const char* szFormat, ... )
	{
		va_list vaArgs;
		char chBuffer[ MAX_LOG_LENGTH ];

		memset( chBuffer, 0, sizeof( char ) * MAX_LOG_LENGTH );
		va_start( vaArgs, szFormat );
		_vsnprintf_s( chBuffer, MAX_LOG_LENGTH, szFormat, vaArgs );
		va_end( vaArgs );
		Log( iPrefix, iLocation, std::string( chBuffer ) );
	}
}
