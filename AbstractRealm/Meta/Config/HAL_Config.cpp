// Parent Header
#include "Config/HAL_Config.hpp"


#include "Meta/EngineInfo.hpp"



namespace Meta
{
	data< EGPUPlatformAPI> GPU_API = Default_GPU_API;

	eGlobal data< EGPU_PresentMode   > GPU_PresentationPref   = EGPU_PresentMode   ::MailBox;
	eGlobal data< EGPU_FrameBuffering> GPU_FrameBufferingPref = EGPU_FrameBuffering::Triple ;

	namespace Vulkan
	{
		// TODO: I needed to inverse the result of use debug. I have no idea why...
		data< bool> EnableLayers = true;

		data< bool> Enable_API_Dump   = false;
		data< bool> Enable_FPSMonitor = true;
		data< bool> Enable_Validation = true;
	}
}