/*
Hardware Abstraction Layer Configuration Flags
*/



#pragma once


#define KHRONOS_VULCAN_HPP         0
#define VAULTED_THERMALS_INTERFACE 1


#define VULCAN_INTERFACE \
VAULTED_THERMALS_INTERFACE


// Set to what HAL_DEBUG will have.
#define HAL_VULKAN_ENABLE_VALIDATION_LAYERS \
true



namespace Core::Meta
{
	constexpr
	bool Vulkan_ValidationLayersEnabled = HAL_VULKAN_ENABLE_VALIDATION_LAYERS;
}
