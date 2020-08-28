#pragma once



#include "LAL/LAL.hpp"



namespace Meta
{
	// Usings

	using namespace LAL;


	// Enums

	enum class EWindowingPlatform
	{
		GLFW,
		OSAL
	};


	// Compile-time

	/*
	Currently windowing platform is determined for the build, it cannot be changed after.
	*/
	constexpr EWindowingPlatform WindowingPlatform = EWindowingPlatform::GLFW;
}

