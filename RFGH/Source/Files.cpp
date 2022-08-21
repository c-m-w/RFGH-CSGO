/// Files.cpp

#include "RFGH.hpp"

namespace Files
{
	constexpr auto DIRECTORY = R"(C:\RFGH\)";

	bool Setup( )
	{
		if ( std::filesystem::exists( DIRECTORY )
			|| std::filesystem::create_directory( DIRECTORY ) )
		{
			Log( PREFIX_SUCCESS, LOCATION_FILES, "Setup successful, directory exists." );
			return true;
		}

		Log( PREFIX_ERROR, LOCATION_FILES, "Setup unsuccessful, error creating directory." );
		return false;
	}

	bool Read( const std::string& strFilename, std::string& strOut )
	{
		const auto pFile = fopen( ( DIRECTORY + strFilename ).c_str( ), "rb" );
		if ( pFile == nullptr )
		{
			Log( PREFIX_WARNING, LOCATION_FILES, "Error opening file " + strFilename + "." );
			return false;
		}

		fseek( pFile, 0, SEEK_END );
		const auto sSize = ftell( pFile );
		if ( sSize == 0 )
		{
			Log( PREFIX_WARNING, LOCATION_FILES, "File " + strFilename + " has an invalid size." );
			return false;
		}

		rewind( pFile );
		strOut.resize( sSize );
		fread( &strOut[ 0 ], sizeof( char ), sSize, pFile );
		fclose( pFile );

		if( strOut.empty( ) )
		{
			Log( PREFIX_WARNING, LOCATION_FILES, "Error reading file " + strFilename + " into buffer." );
			return false;
		}

		Log( PREFIX_SUCCESS, LOCATION_FILES, "Read file " + strFilename + " into buffer." );
		return true;
	}

	bool Write( const std::string& strFilename, const std::string& strIn )
	{
		const auto pFile = fopen( ( DIRECTORY + strFilename ).c_str( ), "wb" );
		const auto sWritten = fwrite( &strIn[ 0 ], sizeof( char ), strIn.length( ), pFile );
		fclose( pFile );
		if( sWritten > 0u )
		{
			Log( PREFIX_SUCCESS, LOCATION_FILES, "Wrote %u bytes into file %s successfully.", sWritten, strFilename.c_str( ) );
			return true;
		}

		Log( PREFIX_SUCCESS, LOCATION_FILES, "Could not write to file " + strFilename + "." );
		return false;
	}

	bool Remove( const std::string& strFilename )
	{
		if( NULL == std::remove( ( DIRECTORY + strFilename ).c_str( ) ) )
		{
			Log( PREFIX_SUCCESS, LOCATION_FILES, "Removed file " + strFilename + " successfully." );
			return true;
		}
		return false;
	}

	std::vector< std::string > GetFiles( const std::string& strExtension )
	{
		std::vector< std::string > vecFiles { };

		for ( auto& file : std::filesystem::directory_iterator( DIRECTORY ) )
		{
			if ( !file.is_regular_file( ) )
				continue;

			const auto wstrFile = file.path( ).wstring( ).substr( strlen( DIRECTORY ) );
			std::string strFile;
			strFile.resize( wstrFile.length( ) );
			wcstombs( &strFile[ 0 ], &wstrFile[ 0 ], wstrFile.length( ) );
			if ( strFile.find( strExtension ) != std::string::npos )
				vecFiles.emplace_back( strFile );
		}

		Log( PREFIX_SUCCESS, LOCATION_FILES, "Found %u files in directory.", vecFiles.size( ) );
		return vecFiles;
	}
}
