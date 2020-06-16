/*
Operating System Abstraction Layer: Platform Definitions

*/

#pragma once


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
}
