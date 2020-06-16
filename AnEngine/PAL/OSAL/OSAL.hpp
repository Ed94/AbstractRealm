/*
Operating System Abstraction Layer
*/



#pragma once



// Includes

#include "LAL/LAL.hpp"

#include "OSAL/Platform.hpp"
#include "OSAL/Windowing.hpp"



namespace OSAL
{
	using Window = PlatformBackend::Window;
}
