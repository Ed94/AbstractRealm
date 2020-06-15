#pragma once


#include "LAL/LAL.hpp"
#include "GLFW_SAL.hpp"



namespace OSAL
{
	enum class EOS
	{
		Windows,
		Mac,
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

	namespace PlatformBackend
	{
		using LAL::Choose;

		enum class EWindowAL
		{
			GLFW,
		};

		constexpr EWindowAL WindowAL = EWindowAL::GLFW;

		using Window = Choose<(WindowAL == EWindowAL::GLFW),
			SAL::GLFW::Window,
			void*
		>;
	}
}
