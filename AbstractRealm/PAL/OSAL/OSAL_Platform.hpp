/*
Operating System Abstraction Layer: Platform Definitions

*/

#pragma once


#include "OSAL_API_Windows.hpp"
#include "LAL/LAL.hpp"


namespace OSAL
{
	using namespace LAL;


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
		template<typename PlatformType>
		struct PlatformStruct_Base
		{
			using BaseType = PlatformType;

			/**
			@brief Does a pointer r-cast to the desired struct type.
			(Since any wrapped platform struct have the same members this is possible)
			*/
			operator PlatformType()
			{
				return dref(RCast<PlatformType>(this));
			}

			/**
			@brief Does a pointer r-cast to the desired struct type.
			(Since any wrapped platform struct have the same members this is possible)
			*/
			operator const PlatformType& () const
			{
				return dref(RCast<const PlatformType>(this));
			}

			/**
			@brief Does a pointer r-cast to the desired struct type.
			(Since any wrapped platform struct have the same members this is possible)
			*/
			operator PlatformType* ()
			{
				return RCast<PlatformType>(this);
			}

			/**
			@brief Does a pointer r-cast to the desired struct type.
			(Since any wrapped platform struct have the same members this is possible)
			*/
			operator const PlatformType* () const
			{
				return RCast<const PlatformType>(this);
			}
		};

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

			static OS_Handle InvalidHandle() 
			{
				return INVALID_HANDLE_VALUE; 
			};

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

		UINT32 Build;

		String Str() const
		{
			StringStream stream;

			stream << Major << "." << Minor << "." << Patch << " Build: " << Build;

			return stream.str();
		}
	};

	const OS_Version& Get_OSVersion();
}
