/*
Hardware Abstraction Layer Configuration Flags
*/



#pragma once


// Set to what HAL_DEBUG will have.
#define HAL_VULKAN_ENABLE_VALIDATION_LAYERS \
true



namespace Core::Meta
{
	constexpr 
	bool Vulkan_ValidationLayersEnabled()
	{
		return HAL_VULKAN_ENABLE_VALIDATION_LAYERS;
	}
}
