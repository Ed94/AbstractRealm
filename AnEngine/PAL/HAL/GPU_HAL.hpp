/*
GPU Hardware Abstraction Layer

Last Modified: 5/19/2020

Currently GPU Abstraction design is way to costly for what I'm after unless a library
does it for me, so I'm keeping 
*/



#pragma once


#include "HAL_Flags.hpp"


#if HAL_GPU_PLATFORM == HAL_GPU_PLATFORM_VULKAN

	#include "Vulkan/Vulkan.hpp"

#endif



#include "LAL/LAL.hpp"


#include "Meta/AppInfo.hpp"



namespace HAL
{
	namespace GPU
	{
		using Core::Meta::AppVersion;

		namespace PlatformBackend
		{
			using namespace LAL;

			using Core::Meta::GPU_API;
			using Core::Meta::EGPUPlatformAPI;

			using AppInstance = Choose<GPU_API == EGPUPlatformAPI::Vulkan,
				Vulkan::AppInstance,
				void>;

			using PhysicalDevice = Choose<GPU_API == EGPUPlatformAPI::Vulkan,
				Vulkan::PhysicalDevice,
				void>;
		}

		using AppInstance    = PlatformBackend::AppInstance   ;
		using PhysicalDevice = PlatformBackend::PhysicalDevice;

		using PhysicalDeviceList = std::vector< PhysicalDevice>;



		// Static Data




		// Functionality

		/*
		Initializes the GPU Application state context object.

		Note: Not sure if this exists conceptually in other API's other than Vulkan.
		*/
		void InitalizeApplication(RoCStr _appName, AppVersion _version);
	}
}
