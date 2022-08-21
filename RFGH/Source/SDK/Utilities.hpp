/// Utilities.hpp

#pragma once

namespace SDK::Utilities
{
	struct signature_t
	{
		std::vector< unsigned char > vecPattern;
		uintptr_t ptrOffset;
	};

	uintptr_t FindPattern( HMODULE hModule, signature_t sigSignature );
	bool WorldToScreen( const Vector& vecEntityPosition, Vector &vecScreenPosition );
	Vector TransformVector( const Vector& vecTransformee, const matrix3x4_t& mtxTransformation );
	int GetWindowWidth( );
	int GetWindowHeight( );
	QAngle GetAngle( const Vector& vecBase, const Vector& vecTarget );
	float GetAngleDifference( const QAngle& angFirst, const QAngle& angSecond );
	Vector TransformAngle( const QAngle& angAngle );
	void ClampAngle( QAngle& angClampee );
}
