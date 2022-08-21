/// Input.hpp

#pragma once

namespace Input
{
	bool Setup( );
	void Close( );
	void AddKeyCallback( std::function< bool( bool ) > fnCallback, DWORD dwKey );
	void AddKeyTypedCallback( std::function< bool( DWORD ) > fnCallback );
	void AddGlobalKeyCallback( std::function< bool( DWORD, bool ) > fnCallback );
	bool IsKeyPressed( DWORD dwKey );
}
