/// Menu.inl

#pragma once

namespace Menu
{
	inline CSlider< int >::CSlider( int * _pValue, int _iMin, int _iMax, const wchar_t * wszSubject, callback_t _cbOnValueChanged /*= nullptr*/ ): IInteractable( TEXT_HEIGHT * 2.f + PADDING ), pValue( _pValue ), 
		iMin( _iMin ), iMax( _iMax ), txtSubject( wszSubject, fntDefault ), pEditWindow( new CContainer( L"Edit Slider Value", false ) ), pInput( new CInputBox( L"Value", 8, CInputBox::FILTER_INTEGER, [ & ]( )
	{
		*pValue = std::clamp( pInput->GetValue< int >( ), iMin, iMax );
		PreviousWindow( );
	} ) ), cbOnValueChanged( _cbOnValueChanged )
	{
		assert( iMin < iMax
				&& iMin >= INT_MIN
				&& iMax <= INT_MAX );

		const auto wstrMin = std::to_wstring( iMin ),
			wstrMax = std::to_wstring( iMax );
		const auto wszMin = new wchar_t[ wstrMin.length( ) + 1 ],
			wszMax = new wchar_t[ wstrMax.length( ) + 1 ];

		wcscpy( wszMin, &wstrMin[ 0 ] );
		wcscpy( wszMax, &wstrMax[ 0 ] );
		txtMin = text_t( wszMin, fntDefault );
		txtMax = text_t( wszMax, fntDefault );

		pEditWindow->Add( pInput );
	}

	inline CSlider< int >::~CSlider( )
	{
		delete[ ] txtMin.wszText;
		delete[ ] txtMax.wszText;
		delete pEditWindow;
	}

	inline void CSlider< int >::Draw( )
	{
		const auto wstrText = std::to_wstring( *pValue );
		const auto txtValue = text_t( wstrText.c_str( ), fntDefault );
		const auto clrCurrent = bSelected ? clrSelected : clrUnselected;
		auto recAbsolute = GetLocation( );

		txtSubject.Draw( text_t::CENTER, text_t::LEFT, clrCurrent, recAbsolute );
		txtMax.Draw( text_t::BOTTOM, text_t::RIGHT, clrCurrent, recAbsolute );
		recAbsolute.flWidth -= PADDING * 2.f + BAR_WIDTH + txtMax.flWidth;
		txtMin.Draw( text_t::BOTTOM, text_t::RIGHT, clrCurrent, recAbsolute );

		const auto flSliderBarTop = recAbsolute.y + recAbsolute.flHeight - txtMin.flHeight / 2.f - BAR_HEIGHT / 2.f;
		const auto flSliderBarLeft = recAbsolute.x + recAbsolute.flWidth + PADDING;
		const auto flValue = roundf( BAR_WIDTH * ( float( *pValue - iMin ) / float( iMax - iMin ) ) );
		SDK::pSurface->DrawSetColor( clrSliderBar );
		SDK::pSurface->DrawFilledRect( int( flSliderBarLeft ), int( flSliderBarTop ),
									   int( flSliderBarLeft + BAR_WIDTH ), int( flSliderBarTop + BAR_HEIGHT ) );

		const Vector2D vecIndicatorPoints[ ]
		{
			Vector2D( flSliderBarLeft + flValue - INDICATOR_SIZE / 2.f, flSliderBarTop - PADDING - INDICATOR_SIZE / 2.f ),
			Vector2D( flSliderBarLeft + flValue, flSliderBarTop - PADDING ),
			Vector2D( flSliderBarLeft + flValue + INDICATOR_SIZE / 2.f, flSliderBarTop - PADDING - INDICATOR_SIZE / 2.f )
		};
		SDK::pSurface->DrawSetColor( clrSliderIndicator );
		SDK::pSurface->DrawLine( int( vecIndicatorPoints[ 0 ].x ), int( vecIndicatorPoints[ 0 ].y ), int( vecIndicatorPoints[ 1 ].x ), int( vecIndicatorPoints[ 1 ].y ) );
		SDK::pSurface->DrawLine( int( vecIndicatorPoints[ 1 ].x ), int( vecIndicatorPoints[ 1 ].y ), int( vecIndicatorPoints[ 2 ].x ), int( vecIndicatorPoints[ 2 ].y ) );
		SDK::pSurface->DrawLine( int( vecIndicatorPoints[ 0 ].x ), int( vecIndicatorPoints[ 0 ].y ), int( vecIndicatorPoints[ 2 ].x ), int( vecIndicatorPoints[ 2 ].y ) );

		txtValue.Draw( vecIndicatorPoints[ 1 ].x - txtValue.flWidth / 2.f + 1.f, vecIndicatorPoints[ 0 ].y - PADDING / 2.f - txtValue.flHeight, bSelected ? clrSliderValueSelected : clrUnselected );
	}

	inline bool CSlider< int >::Event( DWORD dwKey )
	{
		const auto iValue = int( Input::IsKeyPressed( VK_CONTROL ) ? ( iMax - iMin ) / 5 : 1 );

		if ( dwKey == VK_LEFT )
		{
			*pValue = LoopValue( iMin, iMax, *pValue - iValue );

			if( cbOnValueChanged != nullptr )
				cbOnValueChanged( );

			return true;
		}

		if ( dwKey == VK_RIGHT )
		{
			*pValue = LoopValue( iMin, iMax, *pValue + iValue );

			if ( cbOnValueChanged != nullptr )
				cbOnValueChanged( );

			return true;
		}

		if( dwKey == VK_RETURN )
		{
			pEditWindow->ResetSelection( );
			pInput->SetValue( *pValue );
			SetForegroundWindow( pEditWindow );
			return true;
		}

		return false;
	}

	inline CSlider< unsigned char >::CSlider( unsigned char * _pValue, unsigned char _bMin, unsigned char _bMax, const wchar_t * wszSubject, callback_t _cbOnValueChanged /*= nullptr*/ ): IInteractable( TEXT_HEIGHT * 2.f + PADDING ), pValue( _pValue ),
		bMin( _bMin ), bMax( _bMax ), txtSubject( wszSubject, fntDefault ), pEditWindow( new CContainer( L"Edit Slider Value", false ) ), pInput( new CInputBox( L"Value", 3, CInputBox::FILTER_INTEGER_UNSIGNED, [ & ]( )
	{
		*pValue = std::clamp( pInput->GetValue< unsigned char >( ), bMin, bMax );
		PreviousWindow( );
	} ) ), cbOnValueChanged( _cbOnValueChanged )
	{
		assert( bMin < bMax
				&& bMin >= 0
				&& bMax <= UCHAR_MAX );

		const auto wstrMin = std::to_wstring( bMin ),
			wstrMax = std::to_wstring( bMax );
		const auto wszMin = new wchar_t[ wstrMin.length( ) + 1 ],
			wszMax = new wchar_t[ wstrMax.length( ) + 1 ];

		wcscpy( wszMin, &wstrMin[ 0 ] );
		wcscpy( wszMax, &wstrMax[ 0 ] );
		txtMin = text_t( wszMin, fntDefault );
		txtMax = text_t( wszMax, fntDefault );

		pEditWindow->Add( pInput );
	}

	inline CSlider< unsigned char >::~CSlider( )
	{
		delete[ ] txtMin.wszText;
		delete[ ] txtMax.wszText;
		delete pEditWindow;
	}

	inline void CSlider< unsigned char >::Draw( )
	{
		const auto wstrText = std::to_wstring( *pValue );
		const auto txtValue = text_t( wstrText.c_str( ), fntDefault );
		const auto clrCurrent = bSelected ? clrSelected : clrUnselected;
		auto recAbsolute = GetLocation( );

		txtSubject.Draw( text_t::CENTER, text_t::LEFT, clrCurrent, recAbsolute );
		txtMax.Draw( text_t::BOTTOM, text_t::RIGHT, clrCurrent, recAbsolute );
		recAbsolute.flWidth -= PADDING * 2.f + BAR_WIDTH + txtMax.flWidth;
		txtMin.Draw( text_t::BOTTOM, text_t::RIGHT, clrCurrent, recAbsolute );

		const auto flSliderBarTop = recAbsolute.y + recAbsolute.flHeight - txtMin.flHeight / 2.f - BAR_HEIGHT / 2.f;
		const auto flSliderBarLeft = recAbsolute.x + recAbsolute.flWidth + PADDING;
		const auto flValue = floorf( BAR_WIDTH * ( float( *pValue - bMin ) / float( bMax - bMin ) ) );
		SDK::pSurface->DrawSetColor( clrSliderBar );
		SDK::pSurface->DrawFilledRect( int( flSliderBarLeft ), int( flSliderBarTop ),
									   int( flSliderBarLeft + BAR_WIDTH ), int( flSliderBarTop + BAR_HEIGHT ) );

		const Vector2D vecIndicatorPoints[ ]
		{
			Vector2D( flSliderBarLeft + flValue - INDICATOR_SIZE / 2.f, flSliderBarTop - PADDING - INDICATOR_SIZE / 2.f ),
			Vector2D( flSliderBarLeft + flValue, flSliderBarTop - PADDING ),
			Vector2D( flSliderBarLeft + flValue + INDICATOR_SIZE / 2.f, flSliderBarTop - PADDING - INDICATOR_SIZE / 2.f )
		};
		SDK::pSurface->DrawSetColor( clrSliderIndicator );
		SDK::pSurface->DrawLine( int( vecIndicatorPoints[ 0 ].x ), int( vecIndicatorPoints[ 0 ].y ), int( vecIndicatorPoints[ 1 ].x ), int( vecIndicatorPoints[ 1 ].y ) );
		SDK::pSurface->DrawLine( int( vecIndicatorPoints[ 1 ].x ), int( vecIndicatorPoints[ 1 ].y ), int( vecIndicatorPoints[ 2 ].x ), int( vecIndicatorPoints[ 2 ].y ) );
		SDK::pSurface->DrawLine( int( vecIndicatorPoints[ 0 ].x ), int( vecIndicatorPoints[ 0 ].y ), int( vecIndicatorPoints[ 2 ].x ), int( vecIndicatorPoints[ 2 ].y ) );

		txtValue.Draw( vecIndicatorPoints[ 1 ].x - txtValue.flWidth / 2.f + 1.f, vecIndicatorPoints[ 0 ].y - PADDING / 2.f - txtValue.flHeight, bSelected ? clrSliderValueSelected : clrUnselected );
	}

	inline bool CSlider< unsigned char >::Event( DWORD dwKey )
	{
		const auto iValue = int( Input::IsKeyPressed( VK_CONTROL ) ? ( bMax - bMin ) / 5 : 1 );

		if ( dwKey == VK_LEFT )
		{
			*pValue = unsigned char( LoopValue( int( bMin ), int( bMax ), int( *pValue ) - iValue ) );

			if ( cbOnValueChanged != nullptr )
				cbOnValueChanged( );

			return true;
		}

		if ( dwKey == VK_RIGHT )
		{
			*pValue = unsigned char( LoopValue( int( bMin ), int( bMax ), int( *pValue ) + iValue ) );

			if ( cbOnValueChanged != nullptr )
				cbOnValueChanged( );

			return true;
		}

		if ( dwKey == VK_RETURN )
		{
			pEditWindow->ResetSelection( );
			pInput->SetValue( *pValue );
			SetForegroundWindow( pEditWindow );
			return true;
		}

		return false;
	}

	inline CSlider< float >::CSlider( float* _pValue, float _flMin, float _flMax, const wchar_t* wszSubject, std::size_t _sPrecision, callback_t _cbOnValueChanged /*= nullptr*/ ): IInteractable( TEXT_HEIGHT * 2.f + PADDING ), pValue( _pValue ), flMin( _flMin ), flMax( _flMax ),
		sPrecision( _sPrecision ), txtSubject( wszSubject, fntDefault ), pEditWindow( new CContainer( L"Edit Slider Value", false ) ), pInput( new CInputBox( L"Value", 8, CInputBox::FILTER_FLOAT, [ & ]( )
	{
		*pValue = std::clamp( pInput->GetValue< float >( ), flMin, flMax );
		PreviousWindow( );
	} ) ), cbOnValueChanged( _cbOnValueChanged )
	{
		assert( flMin < flMax
				&& flMin >= -FLT_MAX
				&& flMax <= FLT_MAX );

		auto wstrMin = std::to_wstring( flMin ),
			wstrMax = std::to_wstring( flMax );

		wstrMin = wstrMin.substr( 0, wstrMin.length( ) - FLOAT_PRECISION + sPrecision );
		wstrMax = wstrMax.substr( 0, wstrMax.length( ) - FLOAT_PRECISION + sPrecision );

		const auto wszMin = new wchar_t[ wstrMin.length( ) + 1 ],
			wszMax = new wchar_t[ wstrMax.length( ) + 1 ];

		wcscpy( wszMin, &wstrMin[ 0 ] );
		wcscpy( wszMax, &wstrMax[ 0 ] );
		txtMin = text_t( wszMin, fntDefault );
		txtMax = text_t( wszMax, fntDefault );

		flStep = powf( 10.f, -float( sPrecision ) );
		pEditWindow->Add( pInput );
	}

	inline CSlider< float >::~CSlider( )
	{
		delete[ ] txtMin.wszText;
		delete[ ] txtMax.wszText;
		delete pEditWindow;
	}

	inline void CSlider< float >::Draw( )
	{
		const auto dMultiplier = pow( 10, double( sPrecision ) ),
			dRounded = round( *pValue * dMultiplier ) / dMultiplier;
		const auto wstrValue = std::to_wstring( dRounded );
		const auto wstrText = wstrValue.substr( 0, wstrValue.length( ) - FLOAT_PRECISION + sPrecision );
		const auto txtValue = text_t( wstrText.c_str( ), fntDefault );
		const auto clrCurrent = bSelected ? clrSelected : clrUnselected;
		auto recAbsolute = GetLocation( );

		txtSubject.Draw( text_t::CENTER, text_t::LEFT, clrCurrent, recAbsolute );
		txtMax.Draw( text_t::BOTTOM, text_t::RIGHT, clrCurrent, recAbsolute );
		recAbsolute.flWidth -= PADDING * 2.f + BAR_WIDTH + txtMax.flWidth;
		txtMin.Draw( text_t::BOTTOM, text_t::RIGHT, clrCurrent, recAbsolute );

		const auto flSliderBarTop = recAbsolute.y + recAbsolute.flHeight - txtMin.flHeight / 2.f - BAR_HEIGHT / 2.f;
		const auto flSliderBarLeft = recAbsolute.x + recAbsolute.flWidth + PADDING;
		const auto flValue = roundf( BAR_WIDTH * ( float( *pValue - flMin ) / float( flMax - flMin ) ) );
		SDK::pSurface->DrawSetColor( clrSliderBar );
		SDK::pSurface->DrawFilledRect( int( flSliderBarLeft ), int( flSliderBarTop ),
									   int( flSliderBarLeft + BAR_WIDTH ), int( flSliderBarTop + BAR_HEIGHT ) );

		const Vector2D vecIndicatorPoints[ ]
		{
			Vector2D( flSliderBarLeft + flValue - INDICATOR_SIZE / 2.f, flSliderBarTop - PADDING - INDICATOR_SIZE / 2.f ),
			Vector2D( flSliderBarLeft + flValue, flSliderBarTop - PADDING ),
			Vector2D( flSliderBarLeft + flValue + INDICATOR_SIZE / 2.f, flSliderBarTop - PADDING - INDICATOR_SIZE / 2.f )
		};
		SDK::pSurface->DrawSetColor( clrSliderIndicator );
		SDK::pSurface->DrawLine( int( vecIndicatorPoints[ 0 ].x ), int( vecIndicatorPoints[ 0 ].y ), int( vecIndicatorPoints[ 1 ].x ), int( vecIndicatorPoints[ 1 ].y ) );
		SDK::pSurface->DrawLine( int( vecIndicatorPoints[ 1 ].x ), int( vecIndicatorPoints[ 1 ].y ), int( vecIndicatorPoints[ 2 ].x ), int( vecIndicatorPoints[ 2 ].y ) );
		SDK::pSurface->DrawLine( int( vecIndicatorPoints[ 0 ].x ), int( vecIndicatorPoints[ 0 ].y ), int( vecIndicatorPoints[ 2 ].x ), int( vecIndicatorPoints[ 2 ].y ) );

		txtValue.Draw( vecIndicatorPoints[ 1 ].x - txtValue.flWidth / 2.f + 1.f, vecIndicatorPoints[ 0 ].y - PADDING / 2.f - txtValue.flHeight, bSelected ? clrSliderValueSelected : clrUnselected );
	}

	inline bool CSlider< float >::Event( DWORD dwKey )
	{
		const auto flValue = Input::IsKeyPressed( VK_CONTROL ) ? ( flMax - flMin ) / 5.f : flStep;

		if ( dwKey == VK_LEFT )
		{
			*pValue = LoopValue( flMin, flMax, *pValue - flValue, sPrecision );

			if ( cbOnValueChanged != nullptr )
				cbOnValueChanged( );

			return true;
		}

		if ( dwKey == VK_RIGHT )
		{
			*pValue = LoopValue( flMin, flMax, *pValue + flValue, sPrecision );

			if ( cbOnValueChanged != nullptr )
				cbOnValueChanged( );

			return true;
		}

		if ( dwKey == VK_RETURN )
		{
			pEditWindow->ResetSelection( );
			pInput->SetValue( *pValue );
			SetForegroundWindow( pEditWindow );
			return true;
		}

		return false;
	}

	template< > inline int CInputBox::GetValue< int >( )
	{
		return std::stoi( strBuffer );
	}

	template< > inline unsigned char CInputBox::GetValue< unsigned char >( )
	{
		return unsigned char( std::stoul( strBuffer ) );
	}

	template< > inline float CInputBox::GetValue< float >( )
	{
		return std::stof( strBuffer );
	}

	template< > inline std::string CInputBox::GetValue< std::string >( )
	{
		return strBuffer;
	}

	template< typename _t > _t LoopValue( _t _Min, _t _Max, _t _Current )
	{
		if ( _Max == _Min )
			_Current = _Max;
		else
		{
			while ( _Current > _Max )
				_Current -= _Max - _Min + _t( 1 );
			while ( _Current < _Min )
				_Current += _Max - _Min + _t( 1 );
		}

		return _Current;
	}

	inline float LoopValue( float flMin, float flMax, float flCurrent, std::size_t sPrecision )
	{
		const auto dMultiplier = pow( 10.0, double( sPrecision ) ), // fucking gay floating point rounding error shit aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
			dMin		= double( __int64( round( double( flMin ) * dMultiplier ) ) ),
			dMax		= double( __int64( round( double( flMax ) * dMultiplier ) ) ),
			dCurrent	= double( __int64( round( double( flCurrent ) * dMultiplier ) ) );
		const auto dFinal = double( LoopValue( __int64( dMin ), __int64( dMax ), __int64( dCurrent ) ) ) / dMultiplier;

		return float( dFinal );
	}
}
