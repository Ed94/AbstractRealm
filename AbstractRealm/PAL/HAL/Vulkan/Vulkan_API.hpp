/*
Vulkan API

Defines the Vulkan API used with the GPU_HAL.



*/



#pragma once



#include "Meta/Config/HAL_Config.hpp"



#if VulkanAPI_Interface == VaultedThermals_Interface

	#define VT_Option__Use_STL_Exceptions
	#define VT_Option__Use_Inline_Hinting
	#include "VaultedThermals/VaultedThermals.hpp"

#endif



