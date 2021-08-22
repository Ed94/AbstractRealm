/*
This is a very rough implementation of a GPU_HAL. 

Its only intended to be used with the current Vulkan HAL used. Although I did mess around with designing for later.

Note: For now this is going to be a big copy and paste to an extent from the triangle test. 
(Only wrapped content though)
*/



// Parent Header
#include "HAL_GPU.hpp"


#include "Meta/Config/OSAL_Config.hpp"
#include "HAL_Backend.hpp"
#include "HAL/Vulkan/GPU_Vulkan.hpp"


namespace HAL::GPU
{
	// Usings

	using namespace LAL ;
	using namespace Meta;

	using Meta::AppVersion;



	namespace PlatformBackend
	{
		using Delegate_Initalize_GPUComms = Function<void(RoCStr, AppVersion)>;
		using Delegate_Cease_GPUComms     = Function<decltype(CeaseComms)>;
		using Delegate_WaitFor_GPUIdle    = Function<decltype(WaitForIdle)>;
		using Delegate_GetRenderContext   = Function<decltype(GetRenderContext)>;

		using Delegate_GetRenderReady         = Function<decltype(HAL::GPU::Default_InitializeRenderer)>;
		using Delegate_DeInitalizeRenderReady = Function<decltype(HAL::GPU::Default_DeinitializeRenderer)>;
		using Delegate_DrawFrame              = Function<decltype(HAL::GPU::Default_DrawFrame)>;
		using Delegate_ReinitalizeRenderer    = Function<decltype(HAL::GPU::Default_ReinitializeRenderer)>;
		


#pragma region StaticData

			Delegate_Initalize_GPUComms Initialize_GPUComms_Bind;
			Delegate_Cease_GPUComms     Cease_GPUComms_Bind     ;
			Delegate_WaitFor_GPUIdle    WaitForIdle         ;
			Delegate_GetRenderContext   GetRenderContext_Bind   ;

			// WIP

			Delegate_GetRenderReady         Default_InitializeRenderer_Bind  ;
			Delegate_DeInitalizeRenderReady Default_DeinitializeRenderer_Bind;
			Delegate_DrawFrame              Default_DrawFrame_Bind           ;

			Delegate_ReinitalizeRenderer Default_ReinitializeRenderer_Bind;

#pragma endregion StaticData



		// Private

		void Determine_PlatformBindings()
		{
			switch (GPU_API())
			{
				case EGPUPlatformAPI::Methane:
				{

				}
				case EGPUPlatformAPI::Vulkan:
				{
					Initialize_GPUComms_Bind = Vulkan::Start_GPUComms;

					Cease_GPUComms_Bind = Vulkan::CeaseComms;
						
					WaitForIdle = Vulkan::WaitForIdle;

					GetRenderContext_Bind = Vulkan::GetRenderContext;

					// WIP

					Default_InitializeRenderer_Bind = Vulkan::Default_InitializeRenderer;

					Default_DeinitializeRenderer_Bind = Vulkan::Default_DeinitializeRenderer;

					Default_DrawFrame_Bind = Vulkan::Default_DrawFrame;

					Default_ReinitializeRenderer_Bind = Vulkan::Default_ReinitializeRenderer;

					Log("Loaded platform bindings: Vulkan");

					break;
				}
				case Meta::EGPUPlatformAPI::No_API:
				{
					Exception::Fatal::Throw("No API Selected.");
				}
			}
		}
	}



	// Public

	void Load()
	{
		Log("Loading module.");

		PlatformBackend::Determine_PlatformBindings();
	}

	void Unload()
	{
		Log("Unloading module.");
	}

	void StartComms(RoCStr _appName, AppVersion _version)
	{
		PlatformBackend::Initialize_GPUComms_Bind(_appName, _version);
	}

	void CeaseComms()
	{
		PlatformBackend::Cease_GPUComms_Bind();
	}

	void WaitForIdle()
	{
		PlatformBackend::WaitForIdle();
	}

	ptr<ARenderContext> GetRenderContext(ptr<OSAL::Window> _window)
	{
		return PlatformBackend::GetRenderContext_Bind(_window);
	}

#pragma region WIP

	void Default_InitializeRenderer(ptr<OSAL::Window> _window)
	{
		PlatformBackend::Default_InitializeRenderer_Bind(_window);
	}


	void Default_DeinitializeRenderer(ptr<OSAL::Window> _window)
	{
		PlatformBackend::Default_DeinitializeRenderer_Bind(_window);
	}

	void Default_DrawFrame(ptr<OSAL::Window> _window)
	{
		PlatformBackend::Default_DrawFrame_Bind(_window);
	}


	void Default_ReinitializeRenderer(ptr<OSAL::Window> _window)
	{
		PlatformBackend::Default_ReinitializeRenderer_Bind(_window);
	}

#pragma endregion WIP
}