/// RFGH.cpp

#include "RFGH.hpp"

DWORD WINAPI Start( LPVOID pImage );
bool Setup( );
void __declspec( noreturn ) Close( HMODULE hImage );

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
)
{
	if ( fdwReason == DLL_PROCESS_ATTACH )
	{
		DisableThreadLibraryCalls( hinstDLL );
		CreateThread( nullptr, NULL, Start, hinstDLL, NULL, nullptr );
		return TRUE;
	}

	return FALSE;
}

DWORD WINAPI Start( LPVOID pImage)
{
	if ( ::Setup( ) )
	{
		do
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 1ui64 ) );
		} while ( !bExit.load( ) );
	}

	Close( HMODULE( pImage ) );
}

bool Setup( )
{
	Debug::Setup( );
	if ( Files::Setup( )
		 && Config::Setup( )
		 && Input::Setup( )
		 && SDK::Setup( )
		 && Hooks::Setup( )
		 && Menu::Setup( )
		 && Features::Visuals::Setup( ) )
	{
		Log( PREFIX_SUCCESS, LOCATION_MAIN, "Setup successful." );
		return true;
	}

	Log( PREFIX_ERROR, LOCATION_MAIN, "Setup unsuccessful, exiting." );
	
	return false;
}

void __declspec( noreturn ) Close( HMODULE hImage )
{
	Menu::Close( );
	Hooks::Close( );
	Input::Close( );
	Debug::Close( );
	FreeLibraryAndExitThread( hImage, EXIT_SUCCESS );
}
