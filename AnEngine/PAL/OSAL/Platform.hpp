/*
Operating System Abstraction Layer: Platform Definitions

*/

#pragma once



// Includes

// Windows
#include <windows.h>

// Engine
#include "LAL/LAL.hpp"



namespace OSAL
{
	enum class EOS
	{
		Windows,
		Mac    ,
		Linux
	};


	#ifdef _WIN32
		constexpr EOS OS = EOS::Windows;

	#endif

	#ifdef __MACH__

	#endif

	#ifdef __linux__

	#endif 

	constexpr bool IsWindows = OS == EOS::Windows;
	constexpr bool IsMac     = OS == EOS::Mac    ;
	constexpr bool IsLinux   = OS == EOS::Linux  ;


	namespace PlatformBackend
	{
		template<OSAL::EOS>
		struct PlatformTypes_Maker;

		template<>
		struct PlatformTypes_Maker<EOS::Windows>
		{
			using OS_AppHandle    = HINSTANCE;
			using OS_WindowHandle = HWND     ;
		};

		using PlatformTypes = PlatformTypes_Maker<OSAL::OS>;
	}

	using PlatformTypes = PlatformBackend::PlatformTypes;
}
