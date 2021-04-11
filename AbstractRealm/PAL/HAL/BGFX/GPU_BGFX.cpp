// Parent
#include "GPU_BGFX.hpp"



#include "HAL_Backend.hpp"
#include "BGFX_API.hpp"



namespace HAL::GPU::BGFX
{
	void Start_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion)
	{
		CLog("Initializing BGFX");

		bgfx::init();
	}

	void Cease_GPUComms()
	{
		bgfx::shutdown();

		CLog("BGFX Shutdown");
	}
}