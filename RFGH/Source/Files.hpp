/// Files.hpp

#pragma once

namespace Files
{
	bool Setup( );
	bool Read( const std::string& strFilename, std::string& strOut );
	bool Write( const std::string& strFilename, const std::string& strIn );
	bool Remove( const std::string& strFilename );
	std::vector< std::string > GetFiles( const std::string& strExtension );
}
