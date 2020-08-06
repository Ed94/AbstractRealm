/*
Vulkan API

Defines the Vulkan API used with the GPU_HAL.



*/



#pragma once



#include "Meta/Config/HAL_Config.hpp"




#if VULCAN_INTERFACE == VAULTED_THERMALS_INTERFACE

	#define VT_Option__Use_Short_Namespace
	#define VT_Option__Use_STL_Exceptions
	#include "VaultedThermals/VaultedThermals.hpp"

#endif
