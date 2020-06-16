/*
GPU Hardware Abstraction Layer


*/



#pragma once




// Engine

#include "HAL/Vulkan/Vulkan_API.hpp"

#include "LAL/LAL.hpp"

#include "Meta/AppInfo.hpp"
#include "Meta/Config/HAL.hpp"



namespace HAL
{
	namespace GPU
	{
		// Usings

		using namespace LAL;

		using Meta::AppVersion;


		// Structs

		/*
		Used for keeping track of physical GPUs present.
		*/
		struct PhysicalDevice
		{
			String Name;
		};


		// Functionality

		// Submodule Related

		/*
		Loads the GPU HAL submodule.

		Note: This prepares the submodule for use of its related functionality supported.
		*/
		void Load();

		/*
		Unloads the GPU HAL submodule.

		Note: Unloading the submodule is not possible unless the GPU is not being used.
		Please make sure the GPU is properly deinitialized with the application first.
		*/
		void Unload();



		// GPU Related


		/*
		Initializes the essential object instances to provide interfacing to the GPU.

		_appName: Name to represent this application to the GPU.
		_version: Version of the application.
		*/
		void Initalize_GPUComms(RoCStr _appName, AppVersion _version);
	}
}
