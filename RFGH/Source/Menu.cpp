/// Menu.cpp

#include "RFGH.hpp"

namespace Menu
{
	std::stack< CContainer* > stkContainers;
	CContainer* pMainWindow = nullptr;
	CContainer* pESPWindow = nullptr;
	CContainer* pGlowWindow = nullptr;
	CContainer* pVisualsWindow = nullptr;
	CContainer* pAimbotWindow = nullptr;
	CContainer* pAimbotSubwindows[ WEAPONTYPE_MAX ] { };
	CContainer* pTriggerbotWindow = nullptr;
	CContainer* pTriggerSubwindows[ WEAPONTYPE_MAX ] { };
	CContainer* pMiscellaneousWindow = nullptr;
	CContainer* pConfigWindow = nullptr;
	CContainer* pExitWindow = nullptr;
	CMultiSelect* pConfigs = nullptr;
	int iSelectedConfig = 0;
	std::string strNewConfig { };

	void SetForegroundWindow( CContainer* pNewWindow );
	void PreviousWindow( );

	rectangle_t::rectangle_t( ): x( 0.f ), y( 0.f ), flWidth( 0.f ), flHeight( 0.f )
	{ }

	rectangle_t::rectangle_t( float _x, float _y, float _flWidth, float _flHeight ): x( _x ), y( _y ), flWidth( _flWidth ), flHeight( _flHeight )
	{ }

	text_t::text_t( ): wszText( nullptr ), fntText( NULL ), flWidth( 0.f ), flHeight( 0.f ), sText( 0u )
	{ }

	text_t::text_t( const wchar_t* _wszText, vgui::HFont _fntText ) : wszText( _wszText ), fntText( _fntText )
	{
		int iWidth, iHeight;

		SDK::pSurface->GetTextSize( fntText, wszText, iWidth, iHeight );
		flWidth = float( iWidth );
		flHeight = float( iHeight );
		sText = wcslen( wszText );
	}

	void text_t::Draw( float x, float y, const Color& clrText ) const
	{
		SDK::pSurface->DrawSetTextFont( fntText );
		SDK::pSurface->DrawSetTextColor( clrText );
		SDK::pSurface->DrawSetTextPos( int( x ), int( y ) );
		SDK::pSurface->DrawPrintText( wszText, sText );
	}

	void text_t::Draw( unsigned uVerticalAlignment, unsigned uHorizontalAlignment, const Color& clrText, const rectangle_t& recUsableSpace ) const
	{
		auto x = 0.f, y = 0.f;

		switch( uVerticalAlignment )
		{
			case TOP:
			{
				y = recUsableSpace.y;
			}
			break;

			case CENTER:
			{
				y = recUsableSpace.y + recUsableSpace.flHeight / 2.f - flHeight / 2.f;
			}
			break;

			case BOTTOM:
			{
				y = recUsableSpace.y + recUsableSpace.flHeight - flHeight;
			}
			break;

			default:
			{
				return;
			}
		}

		switch ( uHorizontalAlignment )
		{
			case LEFT:
			{
				x = recUsableSpace.x;
			}
			break;

			case CENTER:
			{
				x = recUsableSpace.x + recUsableSpace.flWidth / 2.f - flWidth / 2.f;
			}
			break;

			case RIGHT:
			{
				x = recUsableSpace.x + recUsableSpace.flWidth - flWidth;
			}
			break;

			default:
			{
				return;
			}
		}

		Draw( x, y, clrText );
	}

	IDrawable::IDrawable( float flHeight ): pParent( nullptr ), recLocation( rectangle_t( 0.f, 0.f, 0.f, flHeight ) )
	{ }

	IDrawable::IDrawable( const rectangle_t& _recLocation ): pParent( nullptr ), recLocation( _recLocation )
	{ }

	rectangle_t IDrawable::GetLocation( )
	{
		auto recReturn = recLocation;

		if( pParent ) // only single level
		{
			recReturn.x += pParent->recLocation.x;
			recReturn.y += pParent->recLocation.y;
		}

		return recReturn;
	}

	void IDrawable::Move( float flNewX, float flNewY )
	{
		recLocation.x = flNewX;
		recLocation.y = flNewY;
	}

	CHeaderLabel::CHeaderLabel( const wchar_t* wszHeader ): IDrawable( HEADER_TEXT_HEIGHT ), txtHeader( wszHeader, fntHeader )
	{ }

	void CHeaderLabel::Draw( )
	{
		const auto recAbsolute = GetLocation( );

		SDK::pSurface->DrawSetColor( clrLightBackground );
		SDK::pSurface->DrawFilledRect( int( recAbsolute.x ), int( recAbsolute.y ), int( recAbsolute.x + recAbsolute.flWidth ), int( recAbsolute.y + recAbsolute.flHeight ) );

		txtHeader.Draw( text_t::CENTER, text_t::CENTER, clrSelected, recAbsolute );
	}

	CSeparator::CSeparator( float flSpacing, float _flRatio ): IDrawable( flSpacing ), flRatio( _flRatio )
	{ }

	void CSeparator::Draw( )
	{
		const auto recAbsolute = GetLocation( );
		const auto flWidth = recAbsolute.flWidth * flRatio;

		SDK::pSurface->DrawSetColor( clrDarkOutline );
		SDK::pSurface->DrawFilledRect( int( recAbsolute.x + recAbsolute.flWidth / 2.f - flWidth / 2.f ), int( recAbsolute.y + recAbsolute.flHeight / 2.f - LINE_WIDTH / 2.f ),
									   int( recAbsolute.x + recAbsolute.flWidth / 2.f + flWidth / 2.f ), int( recAbsolute.y + recAbsolute.flHeight / 2.f + LINE_WIDTH / 2.f) );
	}

	CTitledSeparator::CTitledSeparator( text_t _txtTitle, float flSpacing, float _flRatio ): IDrawable( _txtTitle.flHeight + LINE_WIDTH + PADDING + flSpacing ), txtTitle( _txtTitle ), flRatio( _flRatio )
	{ }

	CTitledSeparator::CTitledSeparator( const wchar_t* wszTitle, float flSpacing, float _flRatio ): IDrawable( 0.f ), txtTitle( wszTitle, fntDefault ), flRatio( _flRatio )
	{
		*this = CTitledSeparator( txtTitle, flSpacing, flRatio );
	}

	void CTitledSeparator::Draw( )
	{
		const auto recAbsolute = GetLocation( );
		const auto flWidth = recAbsolute.flWidth * flRatio;
		const auto flStart = recAbsolute.y + ( recAbsolute.flHeight - txtTitle.flHeight - PADDING - LINE_WIDTH ) / 2.f;

		SDK::pSurface->DrawSetColor( clrTitledSeparator );
		SDK::pSurface->DrawFilledRect( int( recAbsolute.x + recAbsolute.flWidth / 2.f - flWidth / 2.f ), int( flStart + txtTitle.flHeight + PADDING / 2.f ),
									   int( recAbsolute.x + recAbsolute.flWidth / 2.f + flWidth / 2.f ), int( flStart + txtTitle.flHeight + PADDING / 2.f + LINE_WIDTH ) );

		txtTitle.Draw( recAbsolute.x + recAbsolute.flWidth / 2.f - txtTitle.flWidth / 2.f, flStart, clrTitledSeparator );
	}

	CSpacing::CSpacing( float flSpacing ): IDrawable( flSpacing )
	{ }

	void CSpacing::Draw( )
	{ }

	CColorDisplay::CColorDisplay( Color * _pColor ): IDrawable( DISPLAY_HEIGHT ), pColor( _pColor )
	{ }

	void CColorDisplay::Draw( )
	{
		const auto recAbsolute = GetLocation( );

		SDK::pSurface->DrawSetColor( *pColor );
		SDK::pSurface->DrawFilledRect( int( recAbsolute.x ), int( recAbsolute.y ), int( recAbsolute.x + recAbsolute.flWidth ), int( recAbsolute.y + recAbsolute.flHeight ) );
		SDK::pSurface->DrawSetColor( clrOutline );
		SDK::pSurface->DrawOutlinedRect( int( recAbsolute.x ), int( recAbsolute.y ), int( recAbsolute.x + recAbsolute.flWidth ), int( recAbsolute.y + recAbsolute.flHeight ) );
	}

	IInteractable::IInteractable( float flHeight ): IDrawable( flHeight )
	{ }

	bool IInteractable::KeyTyped( DWORD dwKey )
	{
		return false;
	}

	void CContainer::CorrectLocation( )
	{
		if ( pParent != nullptr )
		{
			recLocation.x = pParent->recLocation.x;
			recLocation.y = pParent->recLocation.y;
		}

		const auto flWidth = float( SDK::Utilities::GetWindowWidth( ) ),
			flHeight = float( SDK::Utilities::GetWindowHeight( ) );

		if ( recLocation.x < 0.f )
			recLocation.x = 0.f;

		if ( recLocation.y < 0.f )
			recLocation.y = 0.f;

		if ( recLocation.x + recLocation.flWidth > flWidth )
			recLocation.x = flWidth - recLocation.flWidth;

		if ( recLocation.y + recLocation.flHeight > flHeight )
			recLocation.y = flHeight - recLocation.flHeight;
	}

	CContainer::CContainer( const wchar_t* wszHeader, bool bMainWindow ): IDrawable( rectangle_t( float( Config::_General.iWindowX ), float( Config::_General.iWindowY ), WIDTH, PADDING * 2.f ) ), iSelectedSelectable( 0 )
	{
		Add( new CHeaderLabel( wszHeader ) );
		Add( new CSeparator( CSeparator::SMALL_SPACE, CSeparator::LONG_LINE )  );
		if ( !bMainWindow )
		{
			Add( new CCallbackLabel( [ & ]( )
			{
				PreviousWindow( );
			}, L"<< Back" ) );
			Add( new CSpacing( CSpacing::LARGE_SPACE ) );
		}
	}

	CContainer::~CContainer( )
	{
		for ( auto& drawable : vecThingsToDraw )
			delete drawable;
	}

	void CContainer::Add( IDrawable* pOption )
	{
		pOption->recLocation = rectangle_t( PADDING, recLocation.flHeight - PADDING, recLocation.flWidth - PADDING * 2.f - 1.f, pOption->recLocation.flHeight );
		pOption->pParent = this;
		recLocation.flHeight += PADDING + pOption->recLocation.flHeight;

		vecThingsToDraw.emplace_back( pOption );
	}

	void CContainer::Add( IInteractable* pOption )
	{
		if ( vecSelectables.empty( ) )
			pOption->bSelected = true;

		pOption->recLocation = rectangle_t( PADDING * 4.f + flIndicatorHeight, recLocation.flHeight - PADDING, recLocation.flWidth - PADDING * 5.f - flIndicatorHeight - 1.f, pOption->recLocation.flHeight );
		pOption->pParent = this;
		recLocation.flHeight += PADDING + pOption->recLocation.flHeight;

		vecThingsToDraw.emplace_back( pOption );
		vecSelectables.emplace_back( pOption );
	}

	bool CContainer::Event( DWORD dwKey )
	{
		if ( dwKey == VK_DOWN )
		{
			vecSelectables[ iSelectedSelectable ]->bSelected = false;
			iSelectedSelectable = LoopValue( 0, int( vecSelectables.size( ) ) - 1, iSelectedSelectable + 1 );
			vecSelectables[ iSelectedSelectable ]->bSelected = true;
			return true;
		}

		if ( dwKey == VK_UP )
		{
			vecSelectables[ iSelectedSelectable]->bSelected = false;
			iSelectedSelectable = LoopValue( 0, int( vecSelectables.size( ) ) - 1, iSelectedSelectable - 1 );
			vecSelectables[ iSelectedSelectable]->bSelected = true;
			return true;
		}

		return vecSelectables[ iSelectedSelectable ]->Event( dwKey );
	}

	bool CContainer::KeyTyped( DWORD dwKey )
	{
		return vecSelectables[ iSelectedSelectable ]->KeyTyped( dwKey );
	}

	void CContainer::ResetSelection( )
	{
		vecSelectables[ iSelectedSelectable ]->bSelected = false;
		vecSelectables[ ( iSelectedSelectable = 0 ) ]->bSelected = true;
	}

	void CContainer::Draw( )
	{
		CorrectLocation( );

		const auto recSelected = vecSelectables[ iSelectedSelectable ]->GetLocation( );
		const auto flIndicatorTop = recSelected.y + recSelected.flHeight / 2.f - INDICATOR_SIZE / 2.f,
			flIndicatorRight = recSelected.x - PADDING * 2.f;

		SDK::pSurface->DrawSetColor( clrBackground );
		SDK::pSurface->DrawFilledRect( int( recLocation.x ), int( recLocation.y ), int( recLocation.x + recLocation.flWidth ), int( recLocation.y + recLocation.flHeight ) );
		SDK::pSurface->DrawSetColor( clrOutline );
		SDK::pSurface->DrawOutlinedRect( int( recLocation.x ), int( recLocation.y ), int( recLocation.x + recLocation.flWidth ), int( recLocation.y + recLocation.flHeight ) );

		const Vector2D vecIndicator[ ]
		{
			Vector2D( flIndicatorRight - flIndicatorHeight, flIndicatorTop ),
			Vector2D( flIndicatorRight - flIndicatorHeight, flIndicatorTop + INDICATOR_SIZE ),
			Vector2D( flIndicatorRight, flIndicatorTop + INDICATOR_SIZE / 2.f )
		};
		SDK::pSurface->DrawSetColor( clrWindowIndicator );
		SDK::pSurface->DrawLine( int( vecIndicator[ 0 ].x ), int( vecIndicator[ 0 ].y ), int( vecIndicator[ 1 ].x ), int( vecIndicator[ 1 ].y ) );
		SDK::pSurface->DrawLine( int( vecIndicator[ 1 ].x ), int( vecIndicator[ 1 ].y ), int( vecIndicator[ 2 ].x ), int( vecIndicator[ 2 ].y ) );
		SDK::pSurface->DrawLine( int( vecIndicator[ 0 ].x ), int( vecIndicator[ 0 ].y ), int( vecIndicator[ 2 ].x ), int( vecIndicator[ 2 ].y ) );

		for ( auto& u: vecThingsToDraw )
			u->Draw( );
	}

	CCallbackLabel::CCallbackLabel( callback_t _cbCallback, const wchar_t* wszLabel ): IInteractable( TEXT_HEIGHT ), cbCallback( _cbCallback ), txtLabel( wszLabel, fntDefault )
	{ }

	void CCallbackLabel::Draw( )
	{
		txtLabel.Draw( text_t::CENTER, text_t::LEFT, bSelected ? clrSelected : clrUnselected, GetLocation( ) );
	}

	bool CCallbackLabel::Event( DWORD dwKey )
	{
		if ( dwKey == VK_RETURN )
		{
			cbCallback( );
			return true;
		}

		return false;
	}

	CWindowLabel::CWindowLabel( const wchar_t* wszLabel, CContainer** _pSetContainer ): IInteractable( TEXT_HEIGHT ), txtLabel( wszLabel, fntDefault ), pSetContainer( _pSetContainer )
	{ }

	void CWindowLabel::Draw( )
	{
		const auto recAbsolute = GetLocation( );

		txtLabel.Draw( text_t::CENTER, text_t::LEFT, bSelected ? clrSelected : clrUnselected, recAbsolute );
	}

	bool CWindowLabel::Event( DWORD dwKey )
	{
		if ( dwKey == VK_RETURN )
		{
			SetForegroundWindow( *pSetContainer );
			return true;
		}

		return false;
	}

	CColorSelector::CColorSelector( const wchar_t * wszLabel, Color* _pColor ): IInteractable( TEXT_HEIGHT ), txtLabel( wszLabel, fntDefault ), pColor( _pColor ),
		pEditWindow( new CContainer( L"Color Selector", false ) )
	{
		pEditWindow->Add( new CSlider< unsigned char >( &pColor->r( ), 0, UCHAR_MAX, L"R:" ) );
		pEditWindow->Add( new CSlider< unsigned char >( &pColor->g( ), 0, UCHAR_MAX, L"G:" ) );
		pEditWindow->Add( new CSlider< unsigned char >( &pColor->b( ), 0, UCHAR_MAX, L"B:" ) );
		pEditWindow->Add( new CSlider< unsigned char >( &pColor->a( ), 0, UCHAR_MAX, L"A:" ) );
		pEditWindow->Add( new CColorDisplay( pColor ) );
	}

	CColorSelector::~CColorSelector( )
	{
		delete pEditWindow;
	}

	void CColorSelector::Draw( )
	{
		const auto recAbsolute = GetLocation( );

		txtLabel.Draw( text_t::CENTER, text_t::LEFT, bSelected ? clrSelected : clrUnselected, recAbsolute );

		SDK::pSurface->DrawSetColor( *pColor );
		SDK::pSurface->DrawFilledRect( int( recAbsolute.x + recAbsolute.flWidth - DISPLAY_WIDTH ), int( recAbsolute.y + recAbsolute.flHeight / 2.f - DISPLAY_HEIGHT / 2.f ),
										 int( recAbsolute.x + recAbsolute.flWidth ), int( recAbsolute.y + recAbsolute.flHeight / 2.f + DISPLAY_HEIGHT / 2.f ) );

		SDK::pSurface->DrawSetColor( bSelected ? clrSelectedOutline : clrDarkOutline );
		SDK::pSurface->DrawOutlinedRect( int( recAbsolute.x + recAbsolute.flWidth - DISPLAY_WIDTH ), int( recAbsolute.y + recAbsolute.flHeight / 2.f - DISPLAY_HEIGHT / 2.f ),
									   int( recAbsolute.x + recAbsolute.flWidth ), int( recAbsolute.y + recAbsolute.flHeight / 2.f + DISPLAY_HEIGHT / 2.f ) );
	}

	bool CColorSelector::Event( DWORD dwKey )
	{
		if( dwKey == VK_RETURN )
		{
			SetForegroundWindow( pEditWindow );
			return true;
		}
		return false;
	}

	CToggle::CToggle( bool* _pEnabled, const wchar_t* wszSubject ): IInteractable( TEXT_HEIGHT ), pEnabled( _pEnabled ), txtSubject( wszSubject, fntDefault )
	{ }

	void CToggle::Draw( )
	{
		const auto recAbsolute = GetLocation( ),
			recToggle = rectangle_t( recAbsolute.x + recAbsolute.flWidth - TOGGLE_WIDTH,
									 recAbsolute.y + recAbsolute.flHeight / 2.f - TOGGLE_HEIGHT / 2.f,
									 TOGGLE_WIDTH, TOGGLE_HEIGHT );

		txtSubject.Draw( text_t::CENTER, text_t::LEFT, bSelected ? clrSelected: clrUnselected, recAbsolute );

		SDK::pSurface->DrawSetColor( *pEnabled ? clrBackground : bSelected ? clrDisabledSelected : clrDisabledUnselected );
		SDK::pSurface->DrawFilledRect( int( recToggle.x ), int( recToggle.y ), int( recToggle.x + recToggle.flWidth / 2.f ),
									   int( recToggle.y + recToggle.flHeight ) );

		SDK::pSurface->DrawSetColor( *pEnabled ? ( bSelected ? clrEnabledSelected : clrEnabledUnselected ) : clrBackground );
		SDK::pSurface->DrawFilledRect( int( recToggle.x + recToggle.flWidth / 2.f ), int( recToggle.y ),
									   int( recToggle.x + recToggle.flWidth ), int( recToggle.y + recToggle.flHeight ) );

		SDK::pSurface->DrawSetColor( bSelected ? clrSelectedOutline : clrDarkOutline );
		SDK::pSurface->DrawOutlinedRect( int( recToggle.x ), int( recToggle.y ), int( recToggle.x + recToggle.flWidth ), int( recToggle.y + recToggle.flHeight ) );
	}

	bool CToggle::Event( DWORD dwKey )
	{
		switch( dwKey )
		{
			case VK_RIGHT:
			{
				*pEnabled = true;
				return true;
			}

			case VK_LEFT:
			{
				*pEnabled = false;
				return true;
			}

			case VK_RETURN:
			{
				*pEnabled = !*pEnabled;
				return true;
			}

			default:
				return false;
		}
	}

	CInputBox::CInputBox( const wchar_t* wszSubject, std::size_t _sMaxCharacters, int iFilter, callback_t _cbOnValueEntered /*= nullptr*/, std::string* _pString /*= nullptr*/ ): IInteractable( TEXT_HEIGHT ),
		txtSubject( wszSubject, fntDefault ), strBuffer( { } ), sMaxCharacters( _sMaxCharacters ), iInsertionPoint( 0 ), cbOnValueEntered( _cbOnValueEntered ),
		pString( _pString ), fnFilter( FILTERS[ iFilter ] )
	{ }

	void CInputBox::Draw( )
	{
		const auto recAbsolute = GetLocation( );
		std::wstring wstrBuffer { };

		wstrBuffer.resize( strBuffer.length( ) );
		mbstowcs( &wstrBuffer[ 0 ], &strBuffer[ 0 ], strBuffer.length( ) );
		const auto txtValue = text_t( wstrBuffer.c_str( ), fntDefault );

		txtSubject.Draw( text_t::CENTER, text_t::LEFT, bSelected ? clrSelected : clrUnselected, recAbsolute );
		txtValue.Draw( text_t::CENTER, text_t::RIGHT, clrInputboxText, bSelected && iInsertionPoint == int( strBuffer.length( ) )
					   ? rectangle_t( recAbsolute.x, recAbsolute.y, recAbsolute.flWidth - float( UNDERLINE_WIDTH ) - PADDING, recAbsolute.flHeight )
					   : recAbsolute );

		if ( bSelected )
		{
			const auto wstrPartial = wstrBuffer.substr( 0, iInsertionPoint );
			const auto txtPartial = text_t( wstrPartial.c_str( ), fntDefault );
			auto iTotalTextWidth = int( txtValue.flWidth - txtPartial.flWidth );
			if ( iInsertionPoint == int( strBuffer.length( ) ) )
				iTotalTextWidth += UNDERLINE_WIDTH;

			SDK::pSurface->DrawSetColor( clrUnderline );
			SDK::pSurface->DrawLine( int( recAbsolute.x + recAbsolute.flWidth - PADDING ) - iTotalTextWidth, int( recAbsolute.y + txtPartial.flHeight ),
									 int( recAbsolute.x + recAbsolute.flWidth - PADDING ) - iTotalTextWidth + UNDERLINE_WIDTH, int( recAbsolute.y + txtPartial.flHeight ) );
		}
	}

	bool CInputBox::Event( DWORD dwKey )
	{
		if( bSelected )
			switch( dwKey )
			{
				case VK_LEFT:
				{
					iInsertionPoint = LoopValue( 0, int( min( strBuffer.length( ), sMaxCharacters - 1 ) ), iInsertionPoint - 1 );
					return true;
				}

				case VK_RIGHT:
				{
					iInsertionPoint = LoopValue( 0, int( min( strBuffer.length( ), sMaxCharacters - 1 ) ), iInsertionPoint + 1 );
					return true;
				}

				case VK_BACK:
				{
					if( iInsertionPoint > 0 )
					{
						iInsertionPoint--;
						strBuffer.erase( strBuffer.begin( ) + iInsertionPoint );
					}
					return true;
				}

				case VK_RETURN:
				{
					bSelected = !bSelected;

					if( cbOnValueEntered != nullptr )
						cbOnValueEntered( );

					return true;
				}

				default:
					break;
			}
		else if ( dwKey == VK_RETURN )
			return bSelected = true;

		return false;
	}

	bool CInputBox::KeyTyped( DWORD dwKey )
	{
		if ( bSelected 
			 && fnFilter( strBuffer, iInsertionPoint, dwKey ) )
		{
			if ( iInsertionPoint == int( strBuffer.length( ) )
				 && strBuffer.length( ) < sMaxCharacters )
				strBuffer.insert( strBuffer.end( ), char( dwKey ) );
			else
				strBuffer[ iInsertionPoint ] = char( dwKey );

			iInsertionPoint = std::clamp( iInsertionPoint + 1, 0, int( min( strBuffer.length( ), sMaxCharacters - 1 ) ) );
			if ( pString != nullptr )
				*pString = strBuffer;

			return true;
		}

		return false;
	}

	void CInputBox::SetValue( int iValue )
	{
		strBuffer = std::to_string( iValue );
	}

	void CInputBox::SetValue( float flValue )
	{
		strBuffer = std::to_string( flValue );
	}

	void CInputBox::SetValue( std::string strValue )
	{
		strBuffer = strValue;
	}

	CMultiSelect::CMultiSelect( const wchar_t* wszSubject, int* _pSelection, const std::initializer_list< std::wstring >& initOptions, callback_t _cbOnValueChanged /*= nullptr*/ ): IInteractable( TEXT_HEIGHT ),
		vecOptions( initOptions ), txtSubject( wszSubject, fntDefault ), pSelection( _pSelection ), cbOnValueChanged( _cbOnValueChanged )
	{ }

	CMultiSelect::CMultiSelect( const wchar_t* wszSubject, int* _pSelection, const std::vector< std::wstring >& _vecOptions, callback_t _cbOnValueChanged /*= nullptr*/ ) : IInteractable( TEXT_HEIGHT ),
		vecOptions( _vecOptions ), txtSubject( wszSubject, fntDefault ), pSelection( _pSelection ), cbOnValueChanged( _cbOnValueChanged )
	{ }

	void CMultiSelect::Draw( )
	{
		auto recAbsolute = GetLocation( );
		recAbsolute.flWidth -= ARROW_SIZE / 2.f + PADDING * 2.f;
		const auto txtDisplay = text_t( vecOptions[ *pSelection ].c_str( ), fntDefault );
		const auto flRight = recAbsolute.x + recAbsolute.flWidth + PADDING,
					flLeft = recAbsolute.x + recAbsolute.flWidth - txtDisplay.flWidth - PADDING * 2.f,
					flCenter = recAbsolute.y + recAbsolute.flHeight / 2.f;

		txtSubject.Draw( text_t::CENTER, text_t::LEFT, bSelected ? clrSelected : clrUnselected, recAbsolute );
		txtDisplay.Draw( text_t::CENTER, text_t::RIGHT, bSelected ? clrMultiSelectSelected : clrUnselected, recAbsolute );

		const Vector2D vecLeftArrow[ ]
		{
			Vector2D( flLeft, flCenter - ARROW_SIZE / 2.f ),
			Vector2D( flLeft, flCenter + ARROW_SIZE / 2.f ),
			Vector2D( flLeft - ARROW_SIZE / 2.f, flCenter )
		};
		const Vector2D vecRightArrow[ ]
		{
			Vector2D( flRight, flCenter - ARROW_SIZE / 2.f ),
			Vector2D( flRight, flCenter + ARROW_SIZE / 2.f ),
			Vector2D( flRight + ARROW_SIZE / 2.f, flCenter )
		};
		SDK::pSurface->DrawSetColor( bSelected ? clrMultiSelectArrow : clrUnselected );
		SDK::pSurface->DrawLine( int( vecLeftArrow[ 0 ].x ), int( vecLeftArrow[ 0 ].y ), int( vecLeftArrow[ 1 ].x ), int( vecLeftArrow[ 1 ].y ) );
		SDK::pSurface->DrawLine( int( vecLeftArrow[ 1 ].x ), int( vecLeftArrow[ 1 ].y ), int( vecLeftArrow[ 2 ].x ), int( vecLeftArrow[ 2 ].y ) );
		SDK::pSurface->DrawLine( int( vecLeftArrow[ 0 ].x ), int( vecLeftArrow[ 0 ].y ), int( vecLeftArrow[ 2 ].x ), int( vecLeftArrow[ 2 ].y ) );

		SDK::pSurface->DrawLine( int( vecRightArrow[ 0 ].x ), int( vecRightArrow[ 0 ].y ), int( vecRightArrow[ 1 ].x ), int( vecRightArrow[ 1 ].y ) );
		SDK::pSurface->DrawLine( int( vecRightArrow[ 1 ].x ), int( vecRightArrow[ 1 ].y ), int( vecRightArrow[ 2 ].x ), int( vecRightArrow[ 2 ].y ) );
		SDK::pSurface->DrawLine( int( vecRightArrow[ 0 ].x ), int( vecRightArrow[ 0 ].y ), int( vecRightArrow[ 2 ].x ), int( vecRightArrow[ 2 ].y ) );
	}

	bool CMultiSelect::Event( DWORD dwKey )
	{
		if( dwKey == VK_RIGHT )
		{
			*pSelection = LoopValue( 0, int( vecOptions.size( ) ) - 1, *pSelection + 1 );

			if ( cbOnValueChanged != nullptr )
				cbOnValueChanged( );
				
			return true;
		}

		if( dwKey == VK_LEFT )
		{
			*pSelection = LoopValue( 0, int( vecOptions.size( ) ) - 1, *pSelection - 1 );

			if ( cbOnValueChanged != nullptr )
				cbOnValueChanged( );

			return true;
		}

		return false;
	}

	void CMultiSelect::SetOptions( std::vector<std::wstring> _vecOptions )
	{
		vecOptions = _vecOptions;
		*pSelection = 0;
	}

	bool CMultiSelect::GetOption( std::size_t sIndex, std::string& strOut )
	{
		if ( sIndex >= vecOptions.size( )
			 || vecOptions.empty( ) )
			return false;

		auto& strOption = vecOptions[ sIndex ];
		strOut.resize( strOption.length( ) );
		wcstombs( &strOut[ 0 ], &strOption[ 0 ], strOption.length( ) );
		return true;
	}

	CMultiToggle::CMultiToggle( const wchar_t * wszSubject, bool * _pEnabled, std::initializer_list<std::wstring> initOptions ): IInteractable( TEXT_HEIGHT ), vecOptions( initOptions ),
		pEnabled( _pEnabled ), txtSubject( wszSubject, fntDefault ), iSelection( 0 )
	{ }

	void CMultiToggle::Draw( )
	{
		auto recAbsolute = GetLocation( );
		recAbsolute.flWidth -= ARROW_SIZE / 2.f + PADDING * 2.f;
		const auto txtDisplay = text_t( vecOptions[ iSelection ].c_str( ), fntDefault );
		const auto flRight = recAbsolute.x + recAbsolute.flWidth + PADDING,
			flLeft = recAbsolute.x + recAbsolute.flWidth - txtDisplay.flWidth - PADDING * 2.f,
			flCenter = recAbsolute.y + recAbsolute.flHeight / 2.f;

		txtSubject.Draw( text_t::CENTER, text_t::LEFT, bSelected ? clrSelected : clrUnselected, recAbsolute );
		txtDisplay.Draw( text_t::CENTER, text_t::RIGHT, pEnabled[ iSelection ] ? ( bSelected ? clrEnabledSelected : clrEnabledUnselected ) : bSelected ? clrDisabledSelected : clrDisabledUnselected, recAbsolute );

		const Vector2D vecLeftArrow[ ]
		{
			Vector2D( flLeft, flCenter - ARROW_SIZE / 2.f ),
			Vector2D( flLeft, flCenter + ARROW_SIZE / 2.f ),
			Vector2D( flLeft - ARROW_SIZE / 2.f, flCenter )
		};
		const Vector2D vecRightArrow[ ]
		{
			Vector2D( flRight, flCenter - ARROW_SIZE / 2.f ),
			Vector2D( flRight, flCenter + ARROW_SIZE / 2.f ),
			Vector2D( flRight + ARROW_SIZE / 2.f, flCenter )
		};
		SDK::pSurface->DrawSetColor( bSelected ? clrMultiSelectArrow : clrUnselected );
		SDK::pSurface->DrawLine( int( vecLeftArrow[ 0 ].x ), int( vecLeftArrow[ 0 ].y ), int( vecLeftArrow[ 1 ].x ), int( vecLeftArrow[ 1 ].y ) );
		SDK::pSurface->DrawLine( int( vecLeftArrow[ 1 ].x ), int( vecLeftArrow[ 1 ].y ), int( vecLeftArrow[ 2 ].x ), int( vecLeftArrow[ 2 ].y ) );
		SDK::pSurface->DrawLine( int( vecLeftArrow[ 0 ].x ), int( vecLeftArrow[ 0 ].y ), int( vecLeftArrow[ 2 ].x ), int( vecLeftArrow[ 2 ].y ) );

		SDK::pSurface->DrawLine( int( vecRightArrow[ 0 ].x ), int( vecRightArrow[ 0 ].y ), int( vecRightArrow[ 1 ].x ), int( vecRightArrow[ 1 ].y ) );
		SDK::pSurface->DrawLine( int( vecRightArrow[ 1 ].x ), int( vecRightArrow[ 1 ].y ), int( vecRightArrow[ 2 ].x ), int( vecRightArrow[ 2 ].y ) );
		SDK::pSurface->DrawLine( int( vecRightArrow[ 0 ].x ), int( vecRightArrow[ 0 ].y ), int( vecRightArrow[ 2 ].x ), int( vecRightArrow[ 2 ].y ) );
	}

	bool CMultiToggle::Event( DWORD dwKey )
	{
		if ( dwKey == VK_RIGHT )
		{
			iSelection = LoopValue( 0, int( vecOptions.size( ) ) - 1, iSelection + 1 );
			return true;
		}

		if ( dwKey == VK_LEFT )
		{
			iSelection = LoopValue( 0, int( vecOptions.size( ) ) - 1, iSelection - 1 );
			return true;
		}

		if( dwKey == VK_RETURN )
		{
			pEnabled[ iSelection ] = !pEnabled[ iSelection ];
			return true;
		}

		return false;
	}

	bool Event( DWORD dwKey, bool bPressed )
	{
		if ( !bPressed )
			return false;

		if ( dwKey == Config::_General.dwMenuKey )
		{
			if ( stkContainers.empty( ) )
			{
				Log( PREFIX_SUCCESS, LOCATION_MENU, "Showing main window." );
				stkContainers.push( pMainWindow );
			}
			else
			{
				Log( PREFIX_SUCCESS, LOCATION_MENU, "Restoring previous window." );
				PreviousWindow( );
			}

			return true;
		}

		if ( stkContainers.empty( ) )
			return false;

		const auto bReturn = stkContainers.top( )->Event( dwKey );

		switch ( Config::_General.iInputMode )
		{
			case Config::general_t::INPUT_BLOCK_NONE:
			{
				return false;
			}

			case Config::general_t::INPUT_BLOCK_PROCESSED:
			{
				return bReturn;
			}

			case Config::general_t::INPUT_BLOCK_ALL:
			{
				return true;
			}

			default:
			{
				return false;
			}
		}
	}

	bool KeyTyped( DWORD dwKey )
	{
		if ( stkContainers.empty( ) )
			return false;

		const auto bReturn = stkContainers.top( )->KeyTyped( dwKey );

		switch( Config::_General.iInputMode )
		{
			case Config::general_t::INPUT_BLOCK_NONE:
			{
				return false;
			}

			case Config::general_t::INPUT_BLOCK_PROCESSED:
			{
				return bReturn;
			}

			case Config::general_t::INPUT_BLOCK_ALL:
			{
				return true;
			}

			default:
			{
				return false;
			}
		}
	}

	void SetForegroundWindow( CContainer* pNewWindow )
	{
		if ( !stkContainers.empty( ) )
			pNewWindow->pParent = stkContainers.top( );

		stkContainers.push( pNewWindow );
	}
	
	void PreviousWindow( )
	{
		stkContainers.pop( );
	}

	std::map< DWORD, text_t > mpKeys; // making it inline static in the CKeyBind class was crashing at destruct for some reason

	void CKeyBind::Setup( )
	{
		txtBinding = text_t( L"Press Any Key", fntDefault );
		txtUnknownKey = text_t( L"Unknown", fntDefault );
		mpKeys = decltype( mpKeys )
		{
			{ VK_LBUTTON, text_t( L"LMOUSE", fntDefault ) },
			{ VK_RBUTTON, text_t( L"RMOUSE", fntDefault ) },
			{ VK_CANCEL, text_t( L"CANCEL", fntDefault ) },
			{ VK_MBUTTON, text_t( L"MMOUSE", fntDefault ) },
			{ VK_XBUTTON1, text_t( L"X1", fntDefault ) },
			{ VK_XBUTTON2, text_t( L"X2", fntDefault ) },
			{ VK_BACK, text_t( L"BACK", fntDefault ) },
			{ VK_TAB, text_t( L"TAB", fntDefault ) },
			{ VK_CLEAR, text_t( L"CLEAR", fntDefault ) },
			{ VK_RETURN, text_t( L"RETURN", fntDefault ) },
			{ VK_SHIFT, text_t( L"SHIFT", fntDefault ) },
			{ VK_CONTROL, text_t( L"CTRL", fntDefault ) },
			{ VK_MENU, text_t( L"MENU", fntDefault ) },
			{ VK_PAUSE, text_t( L"PAUSE", fntDefault ) },
			{ VK_CAPITAL, text_t( L"CAPS", fntDefault ) },
			{ VK_ESCAPE, text_t( L"ESC", fntDefault ) },
			{ VK_SPACE, text_t( L"SPACE", fntDefault ) },
			{ VK_END, text_t( L"END", fntDefault ) },
			{ VK_HOME, text_t( L"HOME", fntDefault ) },
			{ VK_LEFT, text_t( L"LEFT", fntDefault ) },
			{ VK_UP, text_t( L"UP", fntDefault ) },
			{ VK_RIGHT, text_t( L"RIGHT", fntDefault ) },
			{ VK_DOWN, text_t( L"DOWN", fntDefault ) },
			{ VK_SELECT, text_t( L"SELECT", fntDefault ) },
			{ VK_PRINT, text_t( L"PRINT", fntDefault ) },
			{ VK_INSERT, text_t( L"INS", fntDefault ) },
			{ VK_DELETE, text_t( L"DEL", fntDefault ) },
			{ VK_HELP, text_t( L"HELP", fntDefault ) },
			{ '0', text_t( L"0", fntDefault ) },
			{ '1', text_t( L"1", fntDefault ) },
			{ '2', text_t( L"2", fntDefault ) },
			{ '3', text_t( L"3", fntDefault ) },
			{ '4', text_t( L"4", fntDefault ) },
			{ '5', text_t( L"5", fntDefault ) },
			{ '6', text_t( L"6", fntDefault ) },
			{ '7', text_t( L"7", fntDefault ) },
			{ '8', text_t( L"8", fntDefault ) },
			{ '9', text_t( L"9", fntDefault ) },
			{ 'A', text_t( L"A", fntDefault ) },
			{ 'B', text_t( L"B", fntDefault ) },
			{ 'C', text_t( L"C", fntDefault ) },
			{ 'D', text_t( L"D", fntDefault ) },
			{ 'E', text_t( L"E", fntDefault ) },
			{ 'F', text_t( L"F", fntDefault ) },
			{ 'G', text_t( L"G", fntDefault ) },
			{ 'H', text_t( L"H", fntDefault ) },
			{ 'I', text_t( L"I", fntDefault ) },
			{ 'J', text_t( L"J", fntDefault ) },
			{ 'K', text_t( L"K", fntDefault ) },
			{ 'L', text_t( L"L", fntDefault ) },
			{ 'M', text_t( L"M", fntDefault ) },
			{ 'N', text_t( L"N", fntDefault ) },
			{ 'O', text_t( L"O", fntDefault ) },
			{ 'P', text_t( L"P", fntDefault ) },
			{ 'Q', text_t( L"Q", fntDefault ) },
			{ 'R', text_t( L"R", fntDefault ) },
			{ 'S', text_t( L"S", fntDefault ) },
			{ 'T', text_t( L"T", fntDefault ) },
			{ 'U', text_t( L"U", fntDefault ) },
			{ 'V', text_t( L"V", fntDefault ) },
			{ 'W', text_t( L"W", fntDefault ) },
			{ 'X', text_t( L"X", fntDefault ) },
			{ 'Y', text_t( L"Y", fntDefault ) },
			{ 'Z', text_t( L"Z", fntDefault ) },
			{ VK_LWIN, text_t( L"LWIN", fntDefault ) },
			{ VK_RWIN, text_t( L"RWIN", fntDefault ) },
			{ VK_APPS, text_t( L"APPS", fntDefault ) },
			{ VK_SLEEP, text_t( L"SLEEP", fntDefault ) },
			{ VK_NUMPAD0, text_t( L"NUM0", fntDefault ) },
			{ VK_NUMPAD1, text_t( L"NUM1", fntDefault ) },
			{ VK_NUMPAD2, text_t( L"NUM2", fntDefault ) },
			{ VK_NUMPAD3, text_t( L"NUM3", fntDefault ) },
			{ VK_NUMPAD4, text_t( L"NUM4", fntDefault ) },
			{ VK_NUMPAD5, text_t( L"NUM5", fntDefault ) },
			{ VK_NUMPAD6, text_t( L"NUM6", fntDefault ) },
			{ VK_NUMPAD7, text_t( L"NUM7", fntDefault ) },
			{ VK_NUMPAD8, text_t( L"NUM8", fntDefault ) },
			{ VK_NUMPAD9, text_t( L"NUM9", fntDefault ) },
			{ VK_MULTIPLY, text_t( L"*", fntDefault ) },
			{ VK_ADD, text_t( L"+", fntDefault ) },
			{ VK_SUBTRACT, text_t( L"-", fntDefault ) },
			{ VK_DECIMAL, text_t( L".", fntDefault ) },
			{ VK_DIVIDE, text_t( L"/", fntDefault ) },
			{ VK_F1, text_t( L"F1", fntDefault ) },
			{ VK_F2, text_t( L"F2", fntDefault ) },
			{ VK_F3, text_t( L"F3", fntDefault ) },
			{ VK_F4, text_t( L"F4", fntDefault ) },
			{ VK_F5, text_t( L"F5", fntDefault ) },
			{ VK_F6, text_t( L"F6", fntDefault ) },
			{ VK_F7, text_t( L"F7", fntDefault ) },
			{ VK_F8, text_t( L"F8", fntDefault ) },
			{ VK_F9, text_t( L"F9", fntDefault ) },
			{ VK_F10, text_t( L"F10", fntDefault ) },
			{ VK_F11, text_t( L"F11", fntDefault ) },
			{ VK_F12, text_t( L"F12", fntDefault ) },
			{ VK_NUMLOCK, text_t( L"NUMLK", fntDefault ) },
			{ VK_SCROLL, text_t( L"SCROLL", fntDefault ) },
			{ VK_LSHIFT, text_t( L"LSHIFT", fntDefault ) },
			{ VK_RSHIFT, text_t( L"RSHIFT", fntDefault ) },
			{ VK_LCONTROL, text_t( L"LCTRL", fntDefault ) },
			{ VK_RCONTROL, text_t( L"RCTRL", fntDefault ) },
			{ VK_LMENU, text_t( L"LMENU", fntDefault ) },
			{ VK_RMENU, text_t( L"RMENU", fntDefault ) },
			{ VK_OEM_1, text_t( L":", fntDefault ) },
			{ VK_OEM_PLUS, text_t( L"+", fntDefault ) },
			{ VK_OEM_COMMA, text_t( L",", fntDefault ) },
			{ VK_OEM_MINUS, text_t( L"-", fntDefault ) },
			{ VK_OEM_PERIOD, text_t( L".", fntDefault ) },
			{ VK_OEM_2, text_t( L"/", fntDefault ) },
			{ VK_OEM_3, text_t( L"`", fntDefault ) },
			{ VK_OEM_4, text_t( L"[", fntDefault ) },
			{ VK_OEM_5, text_t( LR"(\)", fntDefault ) },
			{ VK_OEM_6, text_t( L"]", fntDefault ) },
			{ VK_OEM_7, text_t( LR"(')", fntDefault ) },
			{ VK_OEM_8, text_t( L"OEM8", fntDefault ) }
		};
	}

	text_t CKeyBind::GetKeyText( DWORD dwKey )
	{
		const auto pSearch = mpKeys.find( dwKey );
		if ( pSearch == mpKeys.end( ) )
			return txtUnknownKey;

		return pSearch->second;
	}

	CKeyBind::CKeyBind( const wchar_t* wszSubject, DWORD* _pKey, callback_t _cbOnKeyBound ): IInteractable( TEXT_HEIGHT ), pKey( _pKey ), txtSubject( wszSubject, fntDefault ),
		bBinding( false ), cbOnKeyBound( _cbOnKeyBound )
	{ }

	void CKeyBind::Draw( )
	{
		const auto recAbsolute = GetLocation( );

		txtSubject.Draw( text_t::CENTER, text_t::LEFT, bSelected ? clrSelected : clrUnselected, recAbsolute );
		if ( bBinding )
			txtBinding.Draw( text_t::CENTER, text_t::RIGHT, clrValue, recAbsolute );
		else
			GetKeyText( *pKey ).Draw( text_t::CENTER, text_t::RIGHT, clrValue, recAbsolute );
	}

	bool CKeyBind::Event( DWORD dwKey )
	{
		if( bBinding 
			&& dwKey != VK_RETURN )
		{
			*pKey = dwKey;
			bBinding = false;
			if ( cbOnKeyBound != nullptr )
				cbOnKeyBound( );

			return true;
		}
		
		if ( dwKey == VK_RETURN )
		{
			bBinding = true;
			return true;
		}

		return false;
	}

	bool Setup( )
	{
		fntDefault = SDK::pSurface->CreateFont_( );
		fntHeader = SDK::pSurface->CreateFont_( );
		if( fntDefault == NULL
			|| fntHeader == NULL )
		{
			Log( PREFIX_ERROR, LOCATION_MENU, "Failed to create fonts." );
			return false;
		}
		Log( PREFIX_SUCCESS, LOCATION_MENU, "Created fonts." );

		SDK::pSurface->SetFontGlyphSet( fntDefault, "Tahoma", int( TEXT_HEIGHT ), 700, 0, 0, FONTFLAG_DROPSHADOW );
		SDK::pSurface->SetFontGlyphSet( fntHeader, "Tahoma", int( HEADER_TEXT_HEIGHT ), 1000, 0, 0, FONTFLAG_DROPSHADOW );

		CKeyBind::Setup( );

		pMainWindow = new CContainer( L"RFGH for CSGO", true );
		pMainWindow->Add( new CWindowLabel( L"ESP", &pESPWindow ) );
		pMainWindow->Add( new CWindowLabel( L"Glow", &pGlowWindow ) );
		pMainWindow->Add( new CWindowLabel( L"Visuals", &pVisualsWindow ) );
		pMainWindow->Add( new CWindowLabel( L"Miscellaneous", &pMiscellaneousWindow ) );
		pMainWindow->Add( new CWindowLabel( L"Aimbot", &pAimbotWindow ) );
		pMainWindow->Add( new CWindowLabel( L"Triggerbot", &pTriggerbotWindow ) );
		pMainWindow->Add( new CWindowLabel( L"Config", &pConfigWindow ) );
		pMainWindow->Add( new CWindowLabel( L"Exit", &pExitWindow ) );

		const std::initializer_list< std::wstring > initInformation { L"Name", L"Health", L"Weapon" };
		const std::vector< std::wstring > vecOrientations { L"Left", L"Top", L"Right", L"Bottom" };
		pESPWindow = new CContainer( L"ESP", false );
		pESPWindow->Add( new CToggle( &Config::_Settings._ESP.bGlobalEnable, L"Global Enable" ) );
		pESPWindow->Add( new CTitledSeparator( L"Team", CSeparator::SMALL_SPACE, CSeparator::MEDIUM_LINE ) );
		pESPWindow->Add( new CToggle( &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].bEnabled[ Config::config_t::VISIBLE ], L"Visible" ) );
		pESPWindow->Add( new CToggle( &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].bEnabled[ Config::config_t::INVISIBLE ], L"Invisible" ) );
		pESPWindow->Add( new CToggle( &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].bBoundingBox, L"Bounding Box" ) );
		pESPWindow->Add( new CColorSelector( L"Bounding Box Visible", &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].clrBoundingBox[ Config::config_t::VISIBLE ] ) );
		pESPWindow->Add( new CColorSelector( L"Bounding Box Invisible", &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].clrBoundingBox[ Config::config_t::INVISIBLE ] ) );
		pESPWindow->Add( new CToggle( &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].bSnaplines, L"Snaplines" ) );
		pESPWindow->Add( new CColorSelector( L"Snapline Visible", &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].clrSnapline[ Config::config_t::VISIBLE ] ) );
		pESPWindow->Add( new CColorSelector( L"Snapline Invisible", &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].clrSnapline[ Config::config_t::INVISIBLE ] ) );
		pESPWindow->Add( new CMultiToggle( L"Information", Config::_Settings._ESP._Players[ Config::config_t::TEAM ].bInformation, initInformation ) );
		pESPWindow->Add( new CMultiSelect( L"Information Orientation", &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].iInformationOrientation, vecOrientations ) );
		pESPWindow->Add( new CSlider< int >( &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].iInformationPadding, 0, 15, L"Information Padding" ) );
		pESPWindow->Add( new CColorSelector( L"Information Visible", &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].clrInformation[ Config::config_t::VISIBLE ] ) );
		pESPWindow->Add( new CColorSelector( L"Information Invisible", &Config::_Settings._ESP._Players[ Config::config_t::TEAM ].clrInformation[ Config::config_t::INVISIBLE ] ) );
		pESPWindow->Add( new CTitledSeparator( L"Enemy", CSeparator::SMALL_SPACE, CSeparator::MEDIUM_LINE ) );
		pESPWindow->Add( new CToggle( &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].bEnabled[ Config::config_t::VISIBLE ], L"Visible" ) );
		pESPWindow->Add( new CToggle( &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].bEnabled[ Config::config_t::INVISIBLE ], L"Invisible" ) );
		pESPWindow->Add( new CToggle( &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].bBoundingBox, L"Bounding Box" ) );
		pESPWindow->Add( new CColorSelector( L"Bounding Box Visible", &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].clrBoundingBox[ Config::config_t::VISIBLE ] ) );
		pESPWindow->Add( new CColorSelector( L"Bounding Box Invisible", &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].clrBoundingBox[ Config::config_t::INVISIBLE ] ) );
		pESPWindow->Add( new CToggle( &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].bSnaplines, L"Snaplines" ) );
		pESPWindow->Add( new CColorSelector( L"Snapline Visible", &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].clrSnapline[ Config::config_t::VISIBLE ] ) );
		pESPWindow->Add( new CColorSelector( L"Snapline Invisible", &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].clrSnapline[ Config::config_t::INVISIBLE ] ) );
		pESPWindow->Add( new CMultiToggle( L"Information", Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].bInformation, initInformation ) );
		pESPWindow->Add( new CMultiSelect( L"Information Orientation", &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].iInformationOrientation, vecOrientations ) );
		pESPWindow->Add( new CSlider< int >( &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].iInformationPadding, 0, 15, L"Information Padding" ) );
		pESPWindow->Add( new CColorSelector( L"Information Visible", &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].clrInformation[ Config::config_t::VISIBLE ] ) );
		pESPWindow->Add( new CColorSelector( L"Information Invisible", &Config::_Settings._ESP._Players[ Config::config_t::ENEMY ].clrInformation[ Config::config_t::INVISIBLE ] ) );

		const std::vector< std::wstring > vecGlowStyles { L"Default", L"Pulsate", L"Thin", L"Thin Pulsate" };
		pGlowWindow = new CContainer( L"Glow", false );
		pGlowWindow->Add( new CToggle( &Config::_Settings._Glow.bGlobalEnable, L"Global Enable" ) );
		pGlowWindow->Add( new CTitledSeparator( L"Team", CSeparator::SMALL_SPACE, CSeparator::MEDIUM_LINE ) );
		pGlowWindow->Add( new CToggle( &Config::_Settings._Glow._Players[ Config::config_t::TEAM ].bEnabled[ Config::config_t::VISIBLE ], L"Visible" ) );
		pGlowWindow->Add( new CToggle( &Config::_Settings._Glow._Players[ Config::config_t::TEAM ].bEnabled[ Config::config_t::INVISIBLE ], L"Invisible" ) );
		pGlowWindow->Add( new CMultiSelect( L"Style", &Config::_Settings._Glow._Players[ Config::config_t::TEAM ].iGlowStyle, vecGlowStyles ) );
		pGlowWindow->Add( new CColorSelector( L"Visible", &Config::_Settings._Glow._Players[ Config::config_t::TEAM ].clrGlow[ Config::config_t::VISIBLE ] ) );
		pGlowWindow->Add( new CColorSelector( L"Invisible", &Config::_Settings._Glow._Players[ Config::config_t::TEAM ].clrGlow[ Config::config_t::INVISIBLE ] ) );
		pGlowWindow->Add( new CToggle( &Config::_Settings._Glow._Players[ Config::config_t::TEAM ].bFullBloom, L"Full Bloom" ) );
		pGlowWindow->Add( new CSlider< float >( &Config::_Settings._Glow._Players[ Config::config_t::TEAM ].flFullBloomAmount, 0.f, 1.f, L"Bloom Amount", 2 ) );
		pGlowWindow->Add( new CTitledSeparator( L"Enemy", CSeparator::SMALL_SPACE, CSeparator::MEDIUM_LINE ) );
		pGlowWindow->Add( new CToggle( &Config::_Settings._Glow._Players[ Config::config_t::ENEMY ].bEnabled[ Config::config_t::VISIBLE ], L"Visible" ) );
		pGlowWindow->Add( new CToggle( &Config::_Settings._Glow._Players[ Config::config_t::ENEMY ].bEnabled[ Config::config_t::INVISIBLE ], L"Invisible" ) );
		pGlowWindow->Add( new CMultiSelect( L"Style", &Config::_Settings._Glow._Players[ Config::config_t::ENEMY ].iGlowStyle, vecGlowStyles ) );
		pGlowWindow->Add( new CColorSelector( L"Visible", &Config::_Settings._Glow._Players[ Config::config_t::ENEMY ].clrGlow[ Config::config_t::VISIBLE ] ) );
		pGlowWindow->Add( new CColorSelector( L"Invisible", &Config::_Settings._Glow._Players[ Config::config_t::ENEMY ].clrGlow[ Config::config_t::INVISIBLE ] ) );
		pGlowWindow->Add( new CToggle( &Config::_Settings._Glow._Players[ Config::config_t::ENEMY ].bFullBloom, L"Full Bloom" ) );
		pGlowWindow->Add( new CSlider< float >( &Config::_Settings._Glow._Players[ Config::config_t::ENEMY ].flFullBloomAmount, 0.f, 1.f, L"Bloom Amount", 2 ) );

		pVisualsWindow = new CContainer( L"Visuals", false );
		pVisualsWindow->Add( new CToggle( &Config::_Settings._MiscellaneousVisuals.bVisualizeAimbotRange, L"Visualize Aimbot Range" ) );
		pVisualsWindow->Add( new CColorSelector( L"Aimbot Range Color", &Config::_Settings._MiscellaneousVisuals.clrAimbotRange ) );

		std::vector< const wchar_t* > vecWeaponTypes { L"Knife", L"Pistol", L"SMG", L"Rifle", L"Shotgun", L"Sniper Rifle", L"Machine Gun" };
		const std::initializer_list< std::wstring > initHitboxes {
																L"Head", L"Neck", L"Pelvis", L"Stomach", L"Lower Chest", L"Chest", L"Upper Chest", L"Right Thigh", L"Left Thigh", L"Right Calf", L"Left Calf",
																L"Right Foot", L"Left Foot", L"Right Hand", L"Left Hand", L"Right Upper Arm", L"Right Forearm", L"Left Upper Arm", L"Left Forearm" };
		pAimbotWindow = new CContainer( L"Aimbot", false );
		pAimbotWindow->Add( new CToggle( &Config::_Settings._Aimbot.bGlobalEnable, L"Global Enable" ) );
		pAimbotWindow->Add( new CTitledSeparator( L"Weapon Group Configs", CSeparator::SMALL_SPACE, CSeparator::MEDIUM_LINE ) );
		for ( auto i = 0; i < WEAPONTYPE_MAX; i++ )
		{
			const auto pConfig = &Config::_Settings._Aimbot._Weapons[ i ];
			auto& pWindow = pAimbotSubwindows[ i ];
			pWindow = new CContainer( vecWeaponTypes[ i ], false );
			pAimbotWindow->Add( new CWindowLabel( vecWeaponTypes[ i ], &pWindow ) );

			pWindow->Add( new CMultiToggle( L"Players", pConfig->bEnabled, { L"Team", L"Enemy" } ) );
			pWindow->Add( new CSlider< float >( &pConfig->flMaxCrosshairDistance, 0.1f, 180.f, L"Max Crosshair Distance", 1 ) );
			pWindow->Add( new CSlider< float >( &pConfig->flAimTime, 0.f, 5.f, L"Aim Time", 2 ) );
			pWindow->Add( new CToggle( &pConfig->bSilentAim, L"Silent Aim" ) );
			pWindow->Add( new CMultiSelect( L"Targeting Mode", &pConfig->iTargetingMode, { L"Crosshair Distance", L"Distance", L"Health" } ) );
			pWindow->Add( new CToggle( &pConfig->bOnlyScoped, L"Only Scoped" ) );
			pWindow->Add( new CMultiSelect( L"Priority Hitbox", &pConfig->iPriorityHitbox, initHitboxes ) );
			pWindow->Add( new CMultiSelect( L"Reference Hitbox", &pConfig->iReferenceHitbox, initHitboxes ) );
			pWindow->Add( new CMultiToggle( L"Hitboxes", pConfig->bHitboxes, initHitboxes ) );
			pWindow->Add( new CKeyBind( L"Key", &pConfig->dwKey ) );
			pWindow->Add( new CSeparator( CSeparator::LARGE_SPACE, CSeparator::MEDIUM_LINE ) );
			pWindow->Add( new CToggle( &pConfig->bControlRecoil, L"Control Recoil" ) );
			pWindow->Add( new CSlider< float >( &pConfig->flRecoilAccuracy, 0.01f, 1.f, L"Recoil Control Accuracy", 2 ) );
		}

		pTriggerbotWindow = new CContainer( L"Triggerbot", false );
		pTriggerbotWindow->Add( new CToggle( &Config::_Settings._Triggerbot.bGlobalEnable, L"Global Enable" ) );
		pTriggerbotWindow->Add( new CTitledSeparator( L"Weapon Group Configs", CSeparator::LARGE_SPACE, CSeparator::MEDIUM_LINE ) );
		for( auto i = 0; i < WEAPONTYPE_MAX; i++ )
		{
			const auto pConfig = &Config::_Settings._Triggerbot._Weapons[ i ];
			auto& pWindow = pTriggerSubwindows[ i ];
			pWindow = new CContainer( vecWeaponTypes[ i ], false );
			pTriggerbotWindow->Add( new CWindowLabel( vecWeaponTypes[ i ], &pWindow ) );

			pWindow->Add( new CMultiToggle( L"Players", pConfig->bEnabled, { L"Team", L"Enemy" } ) );
			pWindow->Add( new CToggle( &pConfig->bOnlyScoped, L"Only Scoped" ) );
			pWindow->Add( new CMultiToggle( L"Hitboxes", pConfig->bHitboxes, initHitboxes ) );
			pWindow->Add( new CKeyBind( L"Key", &pConfig->dwKey ) );
		}

		pMiscellaneousWindow = new CContainer( L"Miscellaneous", false );
		pMiscellaneousWindow->Add( new CToggle( &Config::_Settings._Miscellaneous._AutoJump.bEnabled, L"Auto Jump" ) );
		pMiscellaneousWindow->Add( new CKeyBind( L"Auto Jump Key", &Config::_Settings._Miscellaneous._AutoJump.dwKey ) );

		pConfigWindow = new CContainer( L"Config", false );
		pConfigs = new CMultiSelect( L"Config", &iSelectedConfig, Config::GetConfigs( ) );
		pConfigWindow->Add( pConfigs );
		pConfigWindow->Add( new CCallbackLabel( [ & ]( )
		{
			std::string strConfig { };

			if ( pConfigs->GetOption( iSelectedConfig, strConfig ) )
				Config::Save( strConfig );
		}, L"Save" ) );
		pConfigWindow->Add( new CCallbackLabel( [ & ]( )
		{
			std::string strConfig { };

			if ( pConfigs->GetOption( iSelectedConfig, strConfig ) )
				Config::Load( strConfig );
		}, L"Load" ) );
		pConfigWindow->Add( new CCallbackLabel( [ & ]( )
		{
			std::string strConfig { };

			if ( pConfigs->GetOption( iSelectedConfig, strConfig ) )
			{
				Config::Remove( strConfig );
				pConfigs->SetOptions( Config::GetConfigs( ) );
			}
		}, L"Remove" ) );
		pConfigWindow->Add( new CCallbackLabel( [ & ]( )
		{
			pConfigs->SetOptions( Config::GetConfigs( ) );
		}, L"Refresh List" ) );
		pConfigWindow->Add( new CSeparator( CSeparator::SMALL_SPACE, CSeparator::MEDIUM_LINE ) );
		pConfigWindow->Add( new CInputBox( L"New Config Name:", 12u, CInputBox::FILTER_TEXT, [ & ]( )
		{ }, &strNewConfig ) );
		pConfigWindow->Add( new CCallbackLabel( [ & ]( )
		{
			if ( Config::Create( strNewConfig ) )
				pConfigs->SetOptions( Config::GetConfigs( ) );
		}, L"Create Config" ) );
		pConfigWindow->Add( new CTitledSeparator( L"General Config", CSeparator::SMALL_SPACE, CSeparator::MEDIUM_LINE ) );
		pConfigWindow->Add( new CSlider< int >( &Config::_General.iWindowX, 0, SDK::Utilities::GetWindowWidth( ), L"Window X", [ & ]( )
		{
			Config::_General.iWindowX = Config::_General.iWindowX;
			pMainWindow->Move( float( Config::_General.iWindowX ), float( Config::_General.iWindowY ) );
		} ) );
		pConfigWindow->Add( new CSlider< int >( &Config::_General.iWindowY, 0, SDK::Utilities::GetWindowHeight( ), L"Window Y", [ & ]( )
		{
			Config::_General.iWindowY = Config::_General.iWindowY;
			pMainWindow->Move( float( Config::_General.iWindowX ), float( Config::_General.iWindowY ) );
		} ) );
		pConfigWindow->Add( new CMultiSelect( L"Input Mode", &Config::_General.iInputMode, { L"Block None", L"Block Processed", L"Block All" }, [ & ]( )
		{
			Config::_General.iInputMode = Config::_General.iInputMode;
		} ) );
		pConfigWindow->Add( new CKeyBind( L"Menu Key", &Config::_General.dwMenuKey, [ & ]( )
		{
			Config::_General.dwMenuKey = Config::_General.dwMenuKey;
		} ) );

		pExitWindow = new CContainer( L"Are You Sure?", false );
		pExitWindow->Add( new CCallbackLabel( [ & ]( )
		{
			bExit = true;
		}, L"Yes, exit" ) );
		pExitWindow->Add( new CCallbackLabel( [ & ]( )
		{
			PreviousWindow( );
		}, L"No, go back" ) );

		Input::AddGlobalKeyCallback( Event );
		Input::AddKeyTypedCallback( KeyTyped );

		return true;
	}

	void Close( )
	{
		while ( !stkContainers.empty( ) )
			PreviousWindow( );

		delete pMainWindow;
		delete pESPWindow;
		delete pGlowWindow;
		delete pVisualsWindow;
		delete pAimbotWindow;
		for ( auto& pAimbotSubwindow : pAimbotSubwindows )
			delete pAimbotSubwindow;

		delete pTriggerbotWindow;
		for ( auto& pTriggerSubwindow: pTriggerSubwindows )
			delete pTriggerSubwindow;

		delete pMiscellaneousWindow;
		delete pConfigWindow;
		delete pExitWindow;
	}

	void Draw( )
	{
		if ( !stkContainers.empty( ) )
			stkContainers.top( )->Draw( );
	}
}
