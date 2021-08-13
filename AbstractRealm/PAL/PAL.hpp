/*
Platform Abstraction Layer


This should be the only include the rest of the engine should use for platform specific interfacing.
*/



#pragma once



#include "HAL.hpp"
#include "OSAL/OSAL.hpp"
#include "SAL/SAL.hpp"



namespace PAL
{
	namespace Module
	{
		void Load();
		void Unload();
	}
}
