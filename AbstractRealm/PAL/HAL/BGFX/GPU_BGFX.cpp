// Parent
#include "GPU_BGFX.hpp"



#include "HAL_Backend.hpp"
#include "BGFX_API.hpp"



namespace HAL::GPU::BGFX
{
	void Start_GPUComms(RoCStr _applicationName, AppVersion _applicationVersion)
	{
		Log("Initializing BGFX");

		//bgfx::init();
	}

	void Setup_Window()
	{
	}

	void CeaseComms()
	{
		//bgfx::shutdown();

		Log("BGFX Shutdown");
	}
}