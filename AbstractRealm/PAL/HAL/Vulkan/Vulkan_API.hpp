/*
Vulkan API

Defines the Vulkan API used with the GPU_HAL.



*/



#pragma once



#include "Meta/Config/HAL_Config.hpp"



#if VulkanAPI_Interface == VaultedVulkan_Interface

	//#define VT_Open_Vault_4
	#include "VaultedVulkan.hpp"

	#include "glslang/SPIRV/GlslangToSpv.h"

#endif



