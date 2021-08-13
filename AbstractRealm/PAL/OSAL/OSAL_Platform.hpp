/*
Operating System Abstraction Layer: Platform Definitions

*/

#pragma once


// Engine
#include "Meta/Meta.hpp"

namespace C_API
{
	extern "C"
	{
	// Windows
	#ifdef _WIN32

		//#include "targetver.h"
		// Prevents the numeric limits error in LAL.
		#define NOMINMAX
		#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
		// Windows Header Files
		#include <windows.h>
		#include <corecrt_io.h>
		#include <fcntl.h>

	#endif
	}
}

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
		constexpr EOS OS = EOS::Mac;
	#endif

	#ifdef __linux__
		constexpr EOS OS = EOS::Linux;
	#endif 

	constexpr bool IsWindows = OS == EOS::Windows;
	constexpr bool IsMac     = OS == EOS::Mac    ;
	constexpr bool IsLinux   = OS == EOS::Linux  ;


	namespace PlatformBackend
	{
		using namespace C_API;

		template<OSAL::EOS>
		struct PlatformTypes_Maker;

		template<>
		struct PlatformTypes_Maker<EOS::Windows>
		{
			

			using OS_AppHandle    = HINSTANCE;
			using OS_Handle       = HANDLE   ;
			using OS_WindowHandle = HWND     ;

			using OS_CStr   = LPTSTR ;
			using OS_RoCStr = LPCTSTR;

			using HANDLE = HANDLE;

			static OS_Handle InvalidHandle() { return INVALID_HANDLE_VALUE; };

			using ExitValT = int;
		};

		template<>
		struct PlatformTypes_Maker<EOS::Mac>
		{
			using OS_Handle = int;

			// Fill with mac stuff.
		};

		template<>
		struct PlatformTypes_Maker<EOS::Linux>
		{
			// Fill with linux stuff.
		};
	}

	using PlatformTypes = PlatformBackend::PlatformTypes_Maker<OSAL::OS>;

	using OS_AppHandle    = PlatformTypes::OS_AppHandle   ;
	using OS_Handle       = PlatformTypes::OS_Handle      ;
	using OS_WindowHandle = PlatformTypes::OS_WindowHandle;
	using OS_CStr         = PlatformTypes::OS_CStr        ;
	using OS_RoCStr       = PlatformTypes::OS_RoCStr      ;
	using ExitValT        = PlatformTypes::ExitValT       ;

	constexpr auto OS_InvalidHandle = PlatformTypes::InvalidHandle;


	using namespace LAL;

	void Load_Platform();

	const String& Get_OSName();

	struct OS_Version
	{
		u32 Major;
		u32 Minor;
		u32 Patch;

		C_API::UINT32 Build;

		String Str() const
		{
			StringStream stream;

			stream << Major << "." << Minor << "." << Patch << " Build: " << Build;

			return stream.str();
		}
	};

	const OS_Version& Get_OSVersion();
}
