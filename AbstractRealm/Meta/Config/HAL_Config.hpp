/*
Hardware Abstraction Layer Configuration Flags


Currently has GPU HAL related configuration
*/



#pragma once



// Includes

#include "LAL/LAL.hpp"

#include "Meta/Specification.hpp"



// Macros

#define Khronos_Vulkan_HPP        0   // Khronos's vulkan wrapper.
#define VaultedThermals_Interface 1   // Vaulted thermals interface.

/*
Determines which Vulkan library will be used to interface with the Vulkan API.
*/
#define VulkanAPI_Interface \
VaultedThermals_Interface



namespace Meta
{
	using namespace LAL;


	// Enums

	enum class EGPUPlatformAPI
	{
		No_API,
		Vulkan,
	};

	enum class EGPU_PresentMode
	{
		// No V-Sync (Vertical Synchronization) Frames are presented immediately on submission. (Least amount of input lag)
		Immediate   ,   		

		// First-In, First-Out	(V-Sync) Frames only replace during vertical blanking periods. 
		// The frame displayed was the buffer in the "front of the line" of the queue (is not the most recently rendered).
		FIFO        ,   		

		// Same as FIFO, but if running below framerate, just submit as soon as possible (Tearing if below refresh rate).
		FIFO_Relaxed,   

		// Lowest input lag (other than immediate), present the more recently rendered frame.
		MailBox         	
	};

	enum class EGPU_FrameBuffering
	{
		Double,   // Have one frame in flight for presentation to display, the other one is processed for rendering.
		Triple 	  // Have one frame in flight for presentation to display, the other two are available for rendering.
	};


	// Compile-time

	constexpr EGPUPlatformAPI Default_GPU_API = EGPUPlatformAPI::Vulkan;


	// Runtime

	StaticData()

		/*
		Note: Changing the GPU_API will require the GPU_HAL to reinitialize.
		*/
		eGlobal EGPUPlatformAPI GPU_API;

		eGlobal EGPU_PresentMode    GPU_PresentationPref  ;
		eGlobal EGPU_FrameBuffering GPU_FrameBufferingPref;

	namespace  Vulkan
	{
		StaticData()

			/**
			 * Determines if Vulkan will use injectable layers.
			 */
			eGlobal bool EnableLayers;

			/**
			 * Enables the wall of debug essentially for Vulkan.
			 */
			eGlobal bool Enable_API_Dump;

			/**
			 * Enables the fps monitor of the surface on the window title bar.
			 */
			eGlobal bool Enable_FPSMonitor;

			/*
			Determines if Vulkan will use validation layers.
			*/
			eGlobal bool Enable_Validation;

			eGlobal bool Enable_LogVerbose;
			eGlobal bool Enable_LogInfo   ;
			eGlobal bool Enable_LogWarning;
			eGlobal bool Enable_LogError  ;
	}
}
