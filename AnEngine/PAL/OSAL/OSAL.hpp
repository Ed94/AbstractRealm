/*
Operating System Abstraction Layer
*/



#pragma once



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

	namespace DarkSide
	{
		
	}
}
