// Parent Header
#include "Config/HAL_Config.hpp"


#include "Meta/EngineInfo.hpp"



namespace Meta
{
	data< EGPUPlatformAPI> GPU_API = Default_GPU_API;

	// TODO: I needed to inverse the result of use debug. I have no idea why...
	data< bool> Vulkan_EnableValidationLayers = !UseDebug;
}