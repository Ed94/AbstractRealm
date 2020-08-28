/*
Hardware Abstraction Layer Configuration Flags


Currently has GPU HAL related configuration
*/



#pragma once



// Includes

#include "LAL/LAL.hpp"

#include "Meta/EngineInfo.hpp"
#include "Meta/Specification.hpp"



// Macros

#define KHRONOS_VULCAN_HPP         0   // Khronos's vulkan wrapper.
#define VAULTED_THERMALS_INTERFACE 1   // Vaulted thermals interface.

/*
Determines which Vulkan library will be used to interface with the Vulkan API.
*/
#define VULCAN_INTERFACE \
VAULTED_THERMALS_INTERFACE



namespace Meta
{
	using namespace LAL;


	// Enums

	enum class EGPUPlatformAPI
	{
		No_API,
		Vulkan,
	};


	// Compile-time

	constexpr EGPUPlatformAPI Default_GPU_API = EGPUPlatformAPI::Vulkan;


	// Runtime

	/*
	Note: Changing the GPU_API will require the GPU_HAL to reinitialize.
	*/
	eGlobal data< EGPUPlatformAPI> GPU_API;

	namespace  Vulkan
	{
		/**
		 * Determines if vulkan will use injectable layers.
		 */
		eGlobal data< bool> EnableLayers   ;

		/**
		 * Enables the wall of debug essentially for Vulkan.
		 */
		eGlobal data< bool> Enable_API_Dump;

		/**
		 * Enables the fps monitor of the surface on the window title bar.
		 */
		eGlobal data< bool> Enable_FPSMonitor;

		/*
		Determines if Vulkan will use validation layers.
		*/
		eGlobal data< bool> Enable_Validation;
	}
}
