/// Menu.hpp

#pragma once

namespace Menu
{
	using callback_t = std::function< void( ) >;

	class CInputBox;

	constexpr auto TEXT_HEIGHT = 12.f,
		HEADER_TEXT_HEIGHT = 16.f,
		PADDING = 2.f;
	inline vgui::HFont fntDefault { }, fntHeader { };
	inline const Color clrBlack { 0, 0, 0, 255 };
	inline const Color clrWindowIndicator { 164, 194, 242, 255 };
	inline const Color clrBackground { 22, 46, 84, 255 };
	inline const Color clrLightBackground { 12, 45, 96, 255 };
	inline const Color clrOutline { 114, 115, 117, 255 };
	inline const Color clrDarkOutline { 89, 90, 91, 255 };
	inline const Color clrSelectedOutline { 6, 95, 150, 255 };
	inline const Color clrIndicator { 197, 246, 14, 255 };
	inline const Color clrSelected { 255, 255, 255, 255 };
	inline const Color clrUnselected { 175, 180, 187, 255 };
	inline const Color clrTitledSeparator { 85, 112, 155, 255 };
	inline const Color clrEnabledSelected { 39, 244, 24, 255 };
	inline const Color clrEnabledUnselected { 26, 173, 15, 255 };
	inline const Color clrDisabledSelected { 209, 4, 8, 255 };
	inline const Color clrDisabledUnselected { 140, 1, 3, 255 };
	inline const Color clrValue { 13, 165, 216, 255 };
	inline const Color clrMultiSelectArrow { 128, 151, 188, 255 };
	inline const Color clrMultiSelectSelected { 129, 173, 244, 255 };
	inline const Color clrInputboxText { 147, 191, 188, 255 };
	inline const Color clrUnderline { 81, 183, 188, 255 };
	inline const Color clrSliderBar { 72, 106, 160, 255 };
	inline const Color clrSliderValueSelected { 129, 173, 244, 255 };
	inline const Color clrSliderIndicator { 128, 151, 188, 255 };

	struct rectangle_t
	{
		float x, y, flWidth, flHeight;

		rectangle_t( );
		rectangle_t( float _x, float _y, float _flWidth, float _flHeight );
	};

	struct text_t
	{
		enum EAlignment
		{
			TOP,		// Vertical
			CENTER,		// Both
			BOTTOM,		// Vertical
			LEFT = 0,	// Horizontal
			RIGHT = 2	// Horizontal
		};

		const wchar_t* wszText;
		vgui::HFont fntText;
		float flWidth, flHeight;
		std::size_t sText;

		text_t( );
		text_t( const wchar_t* _wszText, vgui::HFont _fntText );

		void Draw( float x, float y, const Color& clrText ) const;
		void Draw( unsigned uVerticalAlignment, unsigned uHorizontalAlignment, const Color& clrText, const rectangle_t& recUsableSpace ) const;
	};

	class IDrawable
	{
	protected:

		IDrawable* pParent;

	public:

		rectangle_t recLocation;

		explicit IDrawable( float flHeight );
		explicit IDrawable( const rectangle_t& _recLocation );
		virtual ~IDrawable( ) = default;

		rectangle_t GetLocation( );
		void Move( float flNewX, float flNewY );

		virtual void Draw( ) = 0;

		friend class CContainer;
	};

	class CHeaderLabel: public IDrawable
	{
		text_t txtHeader;

	public:

		CHeaderLabel( const wchar_t* wszHeader );

		void Draw( ) override;
	};

	class CSeparator: public IDrawable
	{
		float flRatio;

	public:

		constexpr static auto LINE_WIDTH = 1.f;
		constexpr static auto SMALL_SPACE = 1.f;
		constexpr static auto LARGE_SPACE = 5.f;
		constexpr static auto LONG_LINE = 0.95f;
		constexpr static auto MEDIUM_LINE = 0.75f;
		constexpr static auto SMALL_LINE = 0.5f;

		CSeparator( float flSpacing, float _flRatio );

		void Draw( ) override;
	};

	class CTitledSeparator: public IDrawable
	{
		text_t txtTitle;
		float flRatio;

		CTitledSeparator( text_t _txtTitle, float flSpacing, float _flRatio );

	public:

		constexpr static auto LINE_WIDTH = 1.f;
		constexpr static auto SMALL_SPACE = 1.f;
		constexpr static auto LARGE_SPACE = 5.f;
		constexpr static auto LONG_LINE = 0.95f;
		constexpr static auto MEDIUM_LINE = 0.75f;
		constexpr static auto SMALL_LINE = 0.5f;

		CTitledSeparator( const wchar_t* wszTitle, float flSpacing, float _flRatio );

		void Draw( ) override;
	};

	class CSpacing: public IDrawable
	{
	public:

		constexpr static auto SMALL_SPACE = 1.f;
		constexpr static auto LARGE_SPACE = 5.f;

		CSpacing( float flSpacing );

		void Draw( ) override;
	};

	class CColorDisplay: public IDrawable
	{
		Color* pColor;

	public:

		constexpr static auto DISPLAY_HEIGHT = 10.f;

		CColorDisplay( Color* _pColor );

		void Draw( ) override;
	};

	class IInteractable: public IDrawable
	{
	protected:
		bool bSelected = false;

	public:

		explicit IInteractable( float flHeight );

		virtual bool Event( DWORD dwKey ) = 0;
		virtual bool KeyTyped( DWORD dwKey );

		friend class CContainer;
	};

	class CContainer: public IDrawable
	{
		std::vector< IDrawable* > vecThingsToDraw;
		std::vector< IInteractable* > vecSelectables;
		int iSelectedSelectable;

		void CorrectLocation( );

	public:

		constexpr static auto WIDTH = 250.f;
		constexpr static auto INDICATOR_SIZE = 6.f;
		inline const static auto flIndicatorHeight = roundf( powf( powf( INDICATOR_SIZE, 2.f ) - powf( INDICATOR_SIZE / 2.f, 2.f ), 0.5f ) ); // isoceles

		CContainer( const wchar_t* wszHeader, bool bMainWindow );
		~CContainer( );

		void Add( IDrawable* pOption );
		void Add( IInteractable* pOption );
		bool Event( DWORD dwKey );
		bool KeyTyped( DWORD dwKey );
		void ResetSelection( );

		void Draw( ) override;

		friend void SetForegroundWindow( CContainer* pNewWindow );
	};

	class CCallbackLabel: public IInteractable
	{
		callback_t cbCallback;
		text_t txtLabel;

	public:

		explicit CCallbackLabel( callback_t _cbCallback, const wchar_t* wszLabel );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
	};

	class CWindowLabel: public IInteractable
	{
		text_t txtLabel;
		CContainer** pSetContainer;

	public:

		CWindowLabel( const wchar_t* wszLabel, CContainer** _pSetContainer );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
	};

	class CColorSelector: public IInteractable
	{
		text_t txtLabel;
		Color* pColor;
		CContainer* pEditWindow;

	public:

		static constexpr auto DISPLAY_WIDTH = 15.f;
		static constexpr auto DISPLAY_HEIGHT = 8.f;

		explicit CColorSelector( const wchar_t* wszLabel, Color* _pColor );
		~CColorSelector( );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
	};

	class CToggle: public IInteractable
	{
		bool* pEnabled;
		text_t txtSubject;

	public:

		constexpr static auto TOGGLE_HEIGHT = 8.f;
		constexpr static auto TOGGLE_WIDTH = 24.f;

		explicit CToggle( bool* _pEnabled, const wchar_t* wszSubject );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
	};

	template< typename _t >
	class CSlider: public IInteractable
	{ };

	template< >
	class CSlider< int >: public IInteractable
	{
		int* pValue;
		int iMin, iMax;
		text_t txtMin, txtMax, txtSubject;
		CContainer* pEditWindow;
		CInputBox* pInput;
		callback_t cbOnValueChanged;

	public:

		constexpr static auto BAR_WIDTH = 50.f;
		constexpr static auto BAR_HEIGHT = 2.f;
		constexpr static auto INDICATOR_SIZE = 6.f;
		static_assert( BAR_HEIGHT <= TEXT_HEIGHT );

		explicit CSlider( int* _pValue, int _iMin, int _iMax, const wchar_t* wszSubject, callback_t _cbOnValueChanged = nullptr );
		~CSlider( );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
	};

	template< >
	class CSlider< unsigned char >: public IInteractable
	{
		unsigned char* pValue;
		unsigned char bMin, bMax;
		text_t txtMin, txtMax, txtSubject;
		CContainer* pEditWindow;
		CInputBox* pInput;
		callback_t cbOnValueChanged;

	public:

		constexpr static auto BAR_WIDTH = 50.f;
		constexpr static auto BAR_HEIGHT = 2.f;
		constexpr static auto INDICATOR_SIZE = 6.f;
		static_assert( BAR_HEIGHT <= TEXT_HEIGHT );

		explicit CSlider( unsigned char* _pValue, unsigned char _bMin, unsigned char _bMax, const wchar_t* wszSubject, callback_t _cbOnValueChanged = nullptr );
		~CSlider( );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
	};

	template< >
	class CSlider< float >: public IInteractable
	{
		float* pValue;
		float flMin, flMax, flStep;
		std::size_t sPrecision;
		text_t txtMin, txtMax, txtSubject;
		CContainer* pEditWindow;
		CInputBox* pInput;
		callback_t cbOnValueChanged;

	public:

		constexpr static auto BAR_WIDTH = 50.f;
		constexpr static auto BAR_HEIGHT = 2.f;
		constexpr static auto INDICATOR_SIZE = 6.f;
		static_assert( BAR_HEIGHT <= TEXT_HEIGHT );

		explicit CSlider( float* _pValue, float _flMin, float _flMax, const wchar_t* _wszSubject, std::size_t _sPrecision, callback_t _cbOnValueChanged = nullptr );
		~CSlider( );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
	};

	class CInputBox: public IInteractable
	{
		text_t txtSubject;
		std::string strBuffer;
		std::size_t sMaxCharacters;
		int iInsertionPoint;
		callback_t cbOnValueEntered;
		std::string* pString;
		std::function< bool( const std::string&, std::size_t, DWORD ) > fnFilter;

	public:

		enum
		{
			FILTER_NONE,
			FILTER_TEXT,
			FILTER_INTEGER,
			FILTER_INTEGER_UNSIGNED,
			FILTER_FLOAT,
			FILTER_MAX
		};

		inline const static std::function< bool( const std::string&, std::size_t, DWORD ) > FILTERS[ FILTER_MAX ]
		{
			[ & ]( const std::string& strBuffer, std::size_t sInsertionPoint, DWORD dwKey )
			{
				return true;
			},
			[ & ]( const std::string& strBuffer, std::size_t sInsertionPoint, DWORD dwKey )
			{
				return dwKey == ' '
				|| dwKey >= '0' && dwKey <= '9'
				|| dwKey >= 'A' && dwKey <= 'Z'
				|| dwKey >= 'a' && dwKey <= 'z';
			},
			[ & ]( const std::string& strBuffer, std::size_t sInsertionPoint, DWORD dwKey )
			{
				if ( dwKey == '-' )
					return sInsertionPoint == 0
						&& strBuffer.find( '-' ) == std::string::npos;

				return dwKey >= '0' && dwKey <= '9';
			},
			[ & ]( const std::string& strBuffer, std::size_t sInsertionPoint, DWORD dwKey )
			{
				return dwKey >= '0' && dwKey <= '9';
			},
			[ & ]( const std::string& strBuffer, std::size_t sInsertionPoint, DWORD dwKey )
			{
				if ( dwKey == '.' )
					return ( strBuffer.find( '-' ) == std::string::npos
							|| strBuffer.find( '-' ) <= sInsertionPoint )
					&& strBuffer.find( '.' ) == std::string::npos;

				if ( dwKey == '-' )
					return sInsertionPoint == 0
					&& strBuffer.find( '-' ) == std::string::npos;

				return dwKey >= '0' && dwKey <= '9';
			}
		};
		constexpr static auto UNDERLINE_WIDTH = 5;

		CInputBox( const wchar_t* wszSubject, std::size_t _sMaxCharacters, int iFilter, callback_t _cbOnValueEntered = nullptr, std::string* _pString = nullptr );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
		bool KeyTyped( DWORD dwKey ) override;

		void SetValue( int iValue );
		void SetValue( float flValue );
		void SetValue( std::string strValue );

		template< typename _t > _t GetValue( );
		template< > int GetValue< int >( );
		template< > unsigned char GetValue< unsigned char >( );
		template< > float GetValue< float >( );
		template< > std::string GetValue< std::string >( );
	};

	class CMultiSelect: public IInteractable
	{
		std::vector< std::wstring > vecOptions;
		text_t txtSubject;
		int* pSelection;
		callback_t cbOnValueChanged;

	public:

		constexpr static auto ARROW_SIZE = 6.f;

		CMultiSelect( const wchar_t* wszSubject, int* _pSelection, const std::initializer_list< std::wstring >& initOptions, callback_t _cbOnValueChanged = nullptr );
		CMultiSelect( const wchar_t* wszSubject, int* _pSelection, const std::vector< std::wstring >& _vecOptions, callback_t _cbOnValueChanged = nullptr );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
		void SetOptions( std::vector< std::wstring > _vecOptions );
		bool GetOption( std::size_t sIndex, std::string& strOut );
	};

	class CMultiToggle: public IInteractable
	{
		std::vector< std::wstring > vecOptions;
		bool* pEnabled;
		text_t txtSubject;
		int iSelection;

	public:

		constexpr static auto ARROW_SIZE = 6.f;

		CMultiToggle( const wchar_t* wszSubject, bool* _pEnabled, std::initializer_list< std::wstring > initOptions );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
	};

	class CKeyBind: public IInteractable
	{
		DWORD* pKey;
		text_t txtSubject;
		bool bBinding;
		callback_t cbOnKeyBound;

	public:

		inline static text_t txtBinding;
		inline static text_t txtUnknownKey;

		static void Setup( );
		static text_t GetKeyText( DWORD dwKey );

		CKeyBind( const wchar_t* wszSubject, DWORD* _pKey, callback_t _cbOnKeyBound = nullptr );

		void Draw( ) override;
		bool Event( DWORD dwKey ) override;
	};

	bool Setup( );
	void Close( );
	void Draw( );
	void SetForegroundWindow( CContainer* pNewWindow );
	void PreviousWindow( );
	template< typename _t > _t LoopValue( _t _Min, _t _Max, _t _Current );
	float LoopValue( float flMin, float flMax, float flCurrent, std::size_t sPrecision );
}

#include "Menu.inl"
