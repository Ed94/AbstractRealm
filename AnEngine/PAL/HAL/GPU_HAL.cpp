/*
This is a very rough implementation of a GPU_HAL. 

Its only intended to be used with the current Vulkan HAL used. Although I did mess around with designing for later.

Note: For now this is going to be a big copy and paste to an extent from the triangle test. 
(Only wrapped content though)
*/



// Parent Header
#include "GPU_HAL.hpp"



// Includes

#include "LAL/LAL.hpp"

#include "Meta/EngineInfo.hpp"
#include "Meta/Config/OSAL_Config.hpp"
#include "OSAL/Platform.hpp"

#include "GLFW_SAL.hpp"



namespace HAL
{
	namespace GPU
	{
		using namespace LAL;
		using namespace Meta;

		using Meta::AppVersion;


		namespace PlatformBackend
		{
			using Delegate_Initalize_GPUComms = Function<void(RoCStr, AppVersion)>;


			BSS
			(
				Delegate_Initalize_GPUComms Initalize_GPUComms_Bind;
			);


			void Determine_PlatformBindings()
			{
				switch (GPU_API)
				{
					case EGPUPlatformAPI::Vulkan:
					{
						Initalize_GPUComms_Bind = Platform_Vulkan::Initalize_GPUComms_Vulkan;
					}
				}

			}
		}


		// Functionality

		void Load()
		{
			PlatformBackend::Determine_PlatformBindings();
		}

		void Unload()
		{

		}

		void Initalize_GPUComms(RoCStr _appName, AppVersion _version)
		{
			PlatformBackend::Initalize_GPUComms_Bind(_appName, _version);
		}
	}
}