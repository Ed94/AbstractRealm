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
			using Delegate_Cease_GPUComms     = Function<decltype(Cease_GPUComms)>;
			using Delegate_WaitFor_GPUIdle    = Function<decltype(WaitFor_GPUIdle)>;

			namespace Dirty
			{
				using Delegate_GetRenderReady         = Function<decltype(HAL::GPU::Dirty::GetRenderReady)>;
				using Delegate_DeInitalizeRenderReady = Function<decltype(HAL::GPU::Dirty::DeinitializeRenderReady)>;
				using Delegate_DrawFrame              = Function<decltype(HAL::GPU::Dirty::DrawFrame)>;
				using Delegate_ReinitalizeRenderer    = Function<decltype(HAL::GPU::Dirty::ReinitializeRenderer)>;
			}
			


			//BSS 
			//(
				Delegate_Initalize_GPUComms Initialize_GPUComms_Bind;
				Delegate_Cease_GPUComms     Cease_GPUComms_Bind     ;
				Delegate_WaitFor_GPUIdle    WaitFor_GPUIdle         ;

				namespace Dirty
				{
					Delegate_GetRenderReady         GetRenderReady_Bind;
					Delegate_DeInitalizeRenderReady DeinitalizeRenderReady_Bind;
					Delegate_DrawFrame              DrawFrame_Bind;

					Delegate_ReinitalizeRenderer ReinitializeRenderer_Bind;
				}
			//);


			void Determine_PlatformBindings()
			{
				switch (GPU_API)
				{
					case EGPUPlatformAPI::Vulkan:
					{
						Initialize_GPUComms_Bind = Platform_Vulkan::Initialize_GPUComms;

						Cease_GPUComms_Bind = Platform_Vulkan::Cease_GPUComms;
							
						WaitFor_GPUIdle = Platform_Vulkan::WaitFor_GPUIdle;

						Dirty::GetRenderReady_Bind = Platform_Vulkan::Dirty::GetRenderReady;

						Dirty::DeinitalizeRenderReady_Bind = Platform_Vulkan::Dirty::DeInitializeRenderReady;

						Dirty::DrawFrame_Bind = Platform_Vulkan::Dirty::DrawFrame;

						Dirty::ReinitializeRenderer_Bind = Platform_Vulkan::Dirty::ReinitializeRenderer;
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

		void Initialize_GPUComms(RoCStr _appName, AppVersion _version)
		{
			PlatformBackend::Initialize_GPUComms_Bind(_appName, _version);
		}

		void Cease_GPUComms()
		{
			PlatformBackend::Cease_GPUComms_Bind();
		}

		void WaitFor_GPUIdle()
		{
			PlatformBackend::WaitFor_GPUIdle();
		}

		// Dirty

		namespace Dirty
		{
			void GetRenderReady(ptr<OSAL::Window> _window)
			{
				PlatformBackend::Dirty::GetRenderReady_Bind(_window);
			}


			void DeinitializeRenderReady(ptr<OSAL::Window> _window)
			{
				PlatformBackend::Dirty::DeinitalizeRenderReady_Bind(_window);
			}

			void DrawFrame(ptr<OSAL::Window> _window)
			{
				PlatformBackend::Dirty::DrawFrame_Bind(_window);
			}


			void ReinitializeRenderer(ptr<OSAL::Window> _window)
			{
				PlatformBackend::Dirty::ReinitializeRenderer_Bind(_window);
			}
		}
	}
}