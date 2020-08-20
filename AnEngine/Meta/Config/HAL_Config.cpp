// Parent Header
#include "Config/HAL_Config.hpp"


#include "Meta/EngineInfo.hpp"



namespace Meta
{
	data< EGPUPlatformAPI> GPU_API = Default_GPU_API;

	namespace Vulkan
	{
		// TODO: I needed to inverse the result of use debug. I have no idea why...
		data< bool> EnableLayers = true;

		data< bool> Enable_API_Dump   = false;
		data< bool> Enable_FPSMonitor = true ;
		data< bool> Enable_Validation = true ;
	}
}