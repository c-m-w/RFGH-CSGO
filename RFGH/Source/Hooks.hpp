/// Hooks.hpp

#pragma once

namespace Hooks
{
	class CHook
	{
		void** pNewTable,* pOldTable,* pOrigin;
		std::size_t sTable;

	public:

		static std::size_t GetTableLength( void* pTable );

		CHook( void* pInterface );
		~CHook( );

		bool HookIndex( std::size_t sIndex, void* pFunction );
		void* GetOriginalFunction( std::size_t sIndex );
	};

	bool Setup( );
	void Close( );
}
