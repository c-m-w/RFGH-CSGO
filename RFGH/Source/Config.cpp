/// Config.cpp

#include "RFGH.hpp"

namespace Config
{
	constexpr auto GENERAL_CONFIG = "config.json",
		DEFAULT_DEFAULT_CONFIG = "default",
		EXTENSION = ".rfgh";

	nlohmann::json jsGeneral { };

	void SaveGeneralConfig( )
	{
		const auto strDump = jsGeneral.dump( 4 );
		if ( Files::Write( GENERAL_CONFIG, strDump ) )
			Log( PREFIX_SUCCESS, LOCATION_CONFIG, "Saved general config (%s).", GENERAL_CONFIG );
		else
			Log( PREFIX_WARNING, LOCATION_CONFIG, "Could not save general config (%s).", GENERAL_CONFIG );
	}

	int& general_t::GetWindowX( )
	{
		try
		{
			return jsGeneral[ "Window Location" ][ "X" ].get_ref< int& >( );
		}
		catch( ... )
		{
			jsGeneral[ "Window Location" ][ "X" ] = 0;
			SaveGeneralConfig( );
			return iWindowX;
		}
	}

	int& general_t::GetWindowY( )
	{
		try
		{
			return jsGeneral[ "Window Location" ][ "Y" ].get_ref< int& >( );
		}
		catch( ... )
		{
			jsGeneral[ "Window Location" ][ "Y" ] = 0;
			SaveGeneralConfig( );
			return iWindowY;
		}
	}

	int& general_t::GetInputMode( )
	{
		try
		{
			return jsGeneral[ "Input Mode" ].get_ref< int& >( );
		}
		catch( ... )
		{
			jsGeneral[ "Input Mode" ] = INPUT_BLOCK_NONE;
			SaveGeneralConfig( );
			return iInputMode;
		}
	}

	DWORD& general_t::GetMenuKey( )
	{
		try
		{
			return jsGeneral[ "Menu Key" ].get_ref< DWORD& >( );
		}
		catch ( ... )
		{
			jsGeneral[ "Menu Key" ] = DEFAULT_MENU_KEY;
			SaveGeneralConfig( );
			return dwMenuKey;
		}
	}

	void general_t::SetWindowX( int iNewX )
	{
		jsGeneral[ "Window Location" ][ "X" ] = iNewX;
		SaveGeneralConfig( );
	}

	void general_t::SetWindowY( int iNewY )
	{
		jsGeneral[ "Window Location" ][ "Y" ] = iNewY;
		SaveGeneralConfig( );
	}

	void general_t::SetInputMode( int iNewInputMode )
	{
		jsGeneral[ "Input Mode" ] = iNewInputMode;
		SaveGeneralConfig( );
	}

	void general_t::SetMenuKey( DWORD dwNewKey )
	{
		jsGeneral[ "Menu Key" ] = dwNewKey;
		SaveGeneralConfig( );
	}

	bool Setup( )
	{
		std::string strContents { };

		if( Files::Read( GENERAL_CONFIG, strContents ) )
		{
			Log( PREFIX_SUCCESS, LOCATION_CONFIG, "Read general config (%s).", GENERAL_CONFIG );
			
			try
			{
				jsGeneral = nlohmann::json::parse( strContents );
			}
			catch( nlohmann::json::parse_error& )
			{
				Log( PREFIX_ERROR, LOCATION_CONFIG, "Error parsing general config." );
				return false;
			}

			Log( PREFIX_SUCCESS, LOCATION_CONFIG, "Parsed general config." );

			if ( !Load( jsGeneral[ "Default Config" ].get< std::string >( ) )
				 && !Create( jsGeneral[ "Default Config" ].get< std::string >( ) ) )
			{
				Log( PREFIX_ERROR, LOCATION_CONFIG, "Could not load or create default config (" + jsGeneral[ "Default Config" ].get< std::string >( ) + ")." );
				return false;
			}
		}
		else
		{
			jsGeneral[ "Default Config" ] = std::string( DEFAULT_DEFAULT_CONFIG );
			jsGeneral[ "Window Location" ][ "X" ] = 0;
			jsGeneral[ "Window Location" ][ "Y" ] = 0;
			jsGeneral[ "Input Mode" ] = general_t::INPUT_BLOCK_ALL;
			jsGeneral[ "Menu Key" ] = general_t::DEFAULT_MENU_KEY;

			const auto strDump = jsGeneral.dump( 4 );
			if ( !Files::Write( GENERAL_CONFIG, strDump ) )
			{
				Log( PREFIX_ERROR, LOCATION_CONFIG, "Error creating general config." );
				return false;
			}

			Log( PREFIX_SUCCESS, LOCATION_CONFIG, "Created general config." );
			return Setup( );
		}

		return true;
	}

	bool Load( const std::string& strConfig )
	{
		std::string strContents { };
		if( !Files::Read( strConfig + EXTENSION, strContents ) )
			return false;

		nlohmann::json jsConfig { };

		try
		{
			jsConfig = nlohmann::json::parse( strContents );
		}
		catch( nlohmann::json::parse_error& )
		{
			Log( PREFIX_WARNING, LOCATION_CONFIG, "Could not parse config " + strConfig + "." );
			return false;
		}

		Log( PREFIX_SUCCESS, LOCATION_CONFIG, "Parsed config " + strConfig + "." );
		if ( jsConfig[ "Size" ].get< std::size_t >( ) != sizeof( config_t ) )
		{
			Log( PREFIX_WARNING, LOCATION_CONFIG, "Config " + strConfig + " out of date, removing." );
			return Remove( strConfig );
		}

		for ( auto u = 0u; u < jsConfig[ "Size" ].get< std::size_t >( ); u++ )
			*reinterpret_cast< unsigned char* >( uintptr_t( &_Settings ) + u ) = jsConfig[ "Bytes" ][ u ].get< unsigned char >( );

		return true;
	}
	
	bool Save( const std::string& strConfig )
	{
		nlohmann::json jsConfig { };
		jsConfig[ "Size" ] = sizeof( config_t );

		for ( auto u = 0u; u < sizeof( config_t ); u++ )
			jsConfig[ "Bytes" ][ u ] = *reinterpret_cast< unsigned char* >( uintptr_t( &_Settings ) + u );

		const auto strContents = jsConfig.dump( 4 );
		return Files::Write( strConfig + EXTENSION, strContents );
	}

	bool Remove( const std::string& strConfig )
	{
		auto bReturn = Files::Remove( strConfig + EXTENSION );
		if ( bReturn && 
			 strConfig == jsGeneral[ "Default Config" ].get< std::string >( ) )
			bReturn &= Create( jsGeneral[ "Default Config" ].get< std::string >( ) );

		return bReturn;
	}

	bool Create( const std::string& strConfig )
	{
		const auto vecConfigs = Files::GetFiles( EXTENSION );

		for( auto& config : vecConfigs )
			if( config.substr( 0, config.length( ) - strlen( EXTENSION ) ) == strConfig )
			{
				Log( PREFIX_WARNING, LOCATION_CONFIG, "Could not remove config " + strConfig + "; config already exists." );
				return false;
			}

		return Save( strConfig );
	}

	std::vector< std::wstring > GetConfigs( )
	{
		const auto vecFiles = Files::GetFiles( EXTENSION );
		std::vector< std::wstring > vecNewFiles { };

		if( vecFiles.empty( ) )
		{
			Create( jsGeneral[ "Default Config" ].get< std::string >( ) );
			return GetConfigs( );
		}
		
		for ( auto& file : vecFiles )
		{
			const auto strTrimmed = file.substr( 0, file.length( ) - strlen( EXTENSION ) );

			vecNewFiles.emplace_back( ).resize( strTrimmed.length( ) );
			mbstowcs( &vecNewFiles.back( )[ 0 ], &strTrimmed[ 0 ], strTrimmed.length( ) );
		}

		return vecNewFiles;
	}
}
