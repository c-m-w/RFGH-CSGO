/// Utilities.cpp

#include "../RFGH.hpp"

namespace SDK::Utilities
{
	uintptr_t GetModuleEnd( HMODULE hModule )
	{
		return reinterpret_cast< uintptr_t >( hModule ) + reinterpret_cast< PIMAGE_NT_HEADERS > ( reinterpret_cast< unsigned char* >( hModule ) + reinterpret_cast< PIMAGE_DOS_HEADER >( hModule )->e_lfanew )->OptionalHeader.SizeOfImage;
	}

	uintptr_t FindPattern( HMODULE hModule, signature_t sigSignature )
	{
		const auto ptrStart = uintptr_t( hModule );
		const auto ptrEnd = uintptr_t( GetModuleEnd( hModule ) );
		const auto uPatternLength = sigSignature.vecPattern.size( );
		auto ptrPosition = ptrStart;

		while ( ptrPosition < ptrEnd - uPatternLength )
		{
			auto bFound = true;
			for ( auto u = 0u; u < uPatternLength; u++ )
				if ( sigSignature.vecPattern[ u ] != 0 && *reinterpret_cast< unsigned char* >( ptrPosition + u ) != sigSignature.vecPattern[ u ] )
				{
					bFound = false;
					break;
				}

			if ( bFound )
				return ptrPosition + sigSignature.ptrOffset;
			ptrPosition++;
		}
		return NULL;
	}

	bool WorldToScreen( const Vector& vecEntityPosition, Vector &vecScreenPosition )
	{
		int iWidth, iHeight;
		pEngineClient->GetScreenSize( iWidth, iHeight );
		auto vmMatrix = pEngineClient->WorldToScreenMatrix( );

		const auto flTemp = vmMatrix[ 3 ][ 0 ] * vecEntityPosition.x + vmMatrix[ 3 ][ 1 ] * vecEntityPosition.y + vmMatrix[ 3 ][ 2 ] * vecEntityPosition.z + vmMatrix[ 3 ][ 3 ];

		if ( flTemp <= 0.01f )
			return false;

		vecScreenPosition.x = vmMatrix[ 0 ][ 0 ] * vecEntityPosition.x + vmMatrix[ 0 ][ 1 ] * vecEntityPosition.y + vmMatrix[ 0 ][ 2 ] * vecEntityPosition.z + vmMatrix[ 0 ][ 3 ];
		vecScreenPosition.y = vmMatrix[ 1 ][ 0 ] * vecEntityPosition.x + vmMatrix[ 1 ][ 1 ] * vecEntityPosition.y + vmMatrix[ 1 ][ 2 ] * vecEntityPosition.z + vmMatrix[ 1 ][ 3 ];
		vecScreenPosition = { iWidth / 2.f + 0.5f * ( vecScreenPosition.x / flTemp ) * iWidth + 0.5f, iHeight / 2.f - 0.5f * ( vecScreenPosition.y / flTemp ) * iHeight + 0.5f, 0.f };
		return true;
	}

	Vector TransformVector( const Vector& vecTransformee, const matrix3x4_t& mtxTransformation )
	{
		return Vector( vecTransformee.Dot( mtxTransformation[ 0 ] ) + mtxTransformation[ 0 ][ 3 ],
					   vecTransformee.Dot( mtxTransformation[ 1 ] ) + mtxTransformation[ 1 ][ 3 ],
					   vecTransformee.Dot( mtxTransformation[ 2 ] ) + mtxTransformation[ 2 ][ 3 ] );
	}

	int GetWindowWidth( )
	{
		int iWindowWidth, iBuffer;

		pEngineClient->GetScreenSize( iWindowWidth, iBuffer );
		return iWindowWidth;
	}

	int GetWindowHeight( )
	{
		int iBuffer, iWindowHeight;

		pEngineClient->GetScreenSize( iBuffer, iWindowHeight );
		return iWindowHeight;
	}

	QAngle GetAngle( const Vector& vecBase, const Vector& vecTarget )
	{
		const auto vecDifference = vecTarget - vecBase;
		auto flAbsoluteYaw = atan( fabsf( vecDifference.y / vecDifference.x ) ) * 180.f / D3DX_PI;

		if ( vecDifference.x == 0.f )
			if ( vecDifference.y < 0.f )
				flAbsoluteYaw = 270.f;
			else
				flAbsoluteYaw = 90.f;
		else
		{
			if ( vecDifference.x < 0.f )
			{
				if ( vecDifference.y < 0.f )
					flAbsoluteYaw += 180.f;
				else
					flAbsoluteYaw = 180.f - flAbsoluteYaw;
			}
			else
				if ( vecDifference.y < 0.f )
					flAbsoluteYaw = 360.f - flAbsoluteYaw;
		}

		return QAngle( atan( vecDifference.z / powf( powf( vecDifference.x, 2.f ) + powf( vecDifference.y, 2.f ), 0.5f ) ) * -180.f / D3DX_PI,
							 flAbsoluteYaw, 0.f );
	}

	float GetAngleDifference( const QAngle& angFirst, const QAngle& angSecond )
	{
		auto vecDifference = QAngle( max( angFirst.pitch, angSecond.pitch ) - min( angFirst.pitch, angSecond.pitch ),
										   max( angFirst.yaw, angSecond.yaw ) - min( angFirst.yaw, angSecond.yaw ),
										   0.f );

		while ( vecDifference.yaw > 180.f )
			vecDifference.yaw -= 360.f;

		while ( vecDifference.yaw < -180.f )
			vecDifference.yaw += 360.f;

		return powf( powf( vecDifference.pitch, 2.f ) + powf( vecDifference.yaw, 2.f ), 0.5f );
	}

	Vector TransformAngle( const QAngle& angAngle )
	{
		float flSin, flSin2, flCos, flCos2;

		DirectX::XMScalarSinCos( &flSin, &flCos, D3DXToRadian( angAngle[ 0 ] ) );
		DirectX::XMScalarSinCos( &flSin2, &flCos2, D3DXToRadian( angAngle[ 1 ] ) );

		return { flCos * flCos2, flCos * flSin2, -flSin };
	}

	void ClampAngle( QAngle& angClampee )
	{
		while ( angClampee.yaw > 180.f )
			angClampee.yaw -= 360.f;

		while ( angClampee.yaw < -180.f )
			angClampee.yaw += 360.f;

		if ( angClampee.pitch > 89.f )
			angClampee.pitch = 89.f;

		if ( angClampee.pitch < -89.f )
			angClampee.pitch = -89.f;

		if ( angClampee.roll > 45.f )
			angClampee.roll = 45.f;

		if ( angClampee.roll < -45.f )
			angClampee.roll = -45.f;
	}
}
