/// Input.cpp

#include "RFGH.hpp"

namespace Input
{
	WNDPROC pOldWindowProc = nullptr;
	HWND hwWindow = nullptr;
	std::vector< std::function< bool( bool ) > > vecCallbacks[ UCHAR_MAX ] { };
	std::vector< std::function< bool( DWORD ) > > vecKeyTypedCallbacks { };
	std::vector< std::function< bool( DWORD, bool ) > > vecGlobalCallbacks { };
	bool bKeyStates[ UCHAR_MAX ] { };

	bool HandleMessage( bool bPressed, UINT uMessage, WPARAM wParam );

	LRESULT CALLBACK WindowProc(
		_In_ HWND   hwnd,
		_In_ UINT   uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam
	)
	{
		auto bPressed = false;

		switch( uMsg )
		{
			case WM_LBUTTONDOWN:
			{
				wParam = VK_LBUTTON;
				bPressed = true;
			}
			break;

			case WM_RBUTTONDOWN:
			{
				wParam = VK_RBUTTON;
				bPressed = true;
			}
			break;

			case WM_XBUTTONDOWN:
			{
				wParam = HIWORD( wParam ) == 0x1 ? VK_XBUTTON1 : VK_XBUTTON2;
				bPressed = true;
			}
			break;

			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			case WM_CHAR:
			{
				bPressed = true;
			}
			break;

			case WM_LBUTTONUP:
			{
				wParam = VK_LBUTTON;
			}
			break;

			case WM_RBUTTONUP:
			{
				wParam = VK_RBUTTON;
			}
			break;

			case WM_XBUTTONUP:
			{
				wParam = HIWORD( wParam ) == 0x1 ? VK_XBUTTON1 : VK_XBUTTON2;
			}
			break;

			case WM_SYSKEYUP:
			case WM_KEYUP:
				break;

			default:
			{
				return CallWindowProc( pOldWindowProc, hwnd, uMsg, wParam, lParam );
			}
		}

		return HandleMessage( bPressed, uMsg, wParam ) ? TRUE : CallWindowProc( pOldWindowProc, hwnd, uMsg, wParam, lParam );
	}

	BOOL CALLBACK EnumWindowsProc(
		_In_ HWND   hwnd,
		_In_ LPARAM lParam
	)
	{
		DWORD dwProcessID { };
		GetWindowThreadProcessId( hwnd, &dwProcessID );

		if ( dwProcessID == GetCurrentProcessId( ) )
			return ( hwWindow = hwnd ) == nullptr;

		return true;
	}

	bool Setup( )
	{
		memset( bKeyStates, false, sizeof( bool ) * UCHAR_MAX );
		EnumWindows( EnumWindowsProc, 0 );
		if ( nullptr != hwWindow )
		{
			Log( PREFIX_SUCCESS, LOCATION_INPUT, "Found target window with handle %08X.", hwWindow );
			if( nullptr != ( pOldWindowProc = WNDPROC( SetWindowLongPtr( hwWindow, GWLP_WNDPROC, LONG( WindowProc ) ) ) ) )
			{
				Log( PREFIX_SUCCESS, LOCATION_INPUT, "Successfully replaced window input processor %08X with %08X", pOldWindowProc, WindowProc );
				return true;
			}
			Log( PREFIX_ERROR, LOCATION_INPUT, "Failed to set the window input processor." );
			return false;
		}

		Log( PREFIX_ERROR, LOCATION_INPUT, "Could not find the target window." );
		return false;
	}

	void Close( )
	{
		if( pOldWindowProc != nullptr )
			SetWindowLongPtr( hwWindow, GWLP_WNDPROC, LONG( pOldWindowProc ) );
	}

	void AddKeyCallback( std::function< bool( bool ) > fnCallback, DWORD dwKey )
	{
		vecCallbacks[ dwKey ].emplace_back( fnCallback );
	}

	void AddKeyTypedCallback( std::function< bool( DWORD ) > fnCallback )
	{
		vecKeyTypedCallbacks.emplace_back( fnCallback );
	}

	void AddGlobalKeyCallback( std::function< bool( DWORD, bool ) > fnCallback )
	{
		vecGlobalCallbacks.emplace_back( fnCallback );
	}

	bool IsKeyPressed( DWORD dwKey )
	{
		return bKeyStates[ dwKey ];
	}

	bool HandleMessage( bool bPressed, UINT uMessage, WPARAM wParam )
	{
		auto bReturn = false;
		bKeyStates[ wParam ] = bPressed;

		if ( uMessage == WM_CHAR )
			for ( auto& fnCallback : vecKeyTypedCallbacks )
				bReturn |= fnCallback( wParam );
		else
		{
			for ( auto& fnCallback : vecCallbacks[ wParam ] )
				bReturn |= fnCallback( bPressed );

			for ( auto& fnCallback : vecGlobalCallbacks )
				bReturn |= fnCallback( wParam, bPressed );
		}

		return bReturn;
	}
}
