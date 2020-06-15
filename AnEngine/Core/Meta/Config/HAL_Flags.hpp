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


#define HAL_GPU_PLATFORM_VULKAN 0

#define HAL_GPU_PLATFORM \
HAL_GPU_PLATFORM_VULKAN



namespace Core::Meta
{
	constexpr
	bool Vulkan_ValidationLayersEnabled = HAL_VULKAN_ENABLE_VALIDATION_LAYERS;

	enum class EGPUPlatformAPI
	{
		Vulkan = HAL_GPU_PLATFORM_VULKAN,
	};

	constexpr EGPUPlatformAPI GPU_API = EGPUPlatformAPI(HAL_GPU_PLATFORM);
}
