// Parent Header
#include "Config/HAL_Config.hpp"


#include "Meta/EngineInfo.hpp"



namespace Meta
{
	StaticData()

		EGPUPlatformAPI GPU_API = Default_GPU_API;

		eGlobal EGPU_PresentMode    GPU_PresentationPref   = EGPU_PresentMode   ::MailBox;
		eGlobal EGPU_FrameBuffering GPU_FrameBufferingPref = EGPU_FrameBuffering::Triple ;

	namespace Vulkan
	{
		StaticData()

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
}