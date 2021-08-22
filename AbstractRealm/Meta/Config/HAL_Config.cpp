// Parent Header
#include "Config/HAL_Config.hpp"


#include "Meta/EngineInfo.hpp"


namespace Meta
{
	namespace StaticData
	{
		EGPUPlatformAPI GPU_API = Default_GPU_API;

		EGPU_PresentMode    GPU_PresentationPref   = EGPU_PresentMode   ::MailBox;
		EGPU_FrameBuffering GPU_FrameBufferingPref = EGPU_FrameBuffering::Triple ;
	}

	// Public

	EGPUPlatformAPI     GPU_API() { return StaticData::GPU_API; }

	EGPU_PresentMode    GPU_PresentationPref  () { return StaticData::GPU_PresentationPref  ; }
	EGPU_FrameBuffering GPU_FrameBufferingPref() { return StaticData::GPU_FrameBufferingPref; }



	namespace  Vulkan
	{
		namespace StaticData
		{
			// TODO: I needed to inverse the result of use debug. I have no idea why...
			bool EnableLayers = true;

			bool Enable_API_Dump   = false;
			bool Enable_FPSMonitor = true;
			bool Enable_Validation = true;

			bool Enable_LogVerbose = false;
			bool Enable_LogInfo    = true;
			bool Enable_LogWarning = true;
			bool Enable_LogError   = true;
		}



		// Public

		bool EnableLayers() { return StaticData::EnableLayers; }

		bool Enable_API_Dump  () { return StaticData::Enable_API_Dump  ; }
		bool Enable_FPSMonitor() { return StaticData::Enable_FPSMonitor; }
		bool Enable_Validation() { return StaticData::Enable_Validation; }

		bool Enable_LogVerbose() { return StaticData::Enable_LogVerbose; }
		bool Enable_LogInfo   () { return StaticData::Enable_LogInfo   ; }
		bool Enable_LogWarning() { return StaticData::Enable_LogWarning; }
		bool Enable_LogError  () { return StaticData::Enable_LogError  ; }
	}
}