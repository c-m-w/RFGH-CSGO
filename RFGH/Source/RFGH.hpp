/// RFGH.hpp

#pragma once

#define FLOAT_PRECISION ( 6 )
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <stack>
#include <algorithm>
#include <cassert>
#include <filesystem>

#include <nlohmann/json.hpp>

#include <DirectXMath.h>

inline std::atomic< bool > bExit = false;

inline unsigned __int64 Time( )
{
	return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now( ).time_since_epoch( ) ).count( );
}

#include "Files.hpp"
#include "Debug.hpp"
#include "SDK/SDK.hpp"
#include "Config.hpp"
#include "Input.hpp"
#include "Menu.hpp"
#include "Features/Features.hpp"
#include "Hooks.hpp"
