// Parent Header
#include "HAL/HAL.hpp"



// Engine
#include "HAL_Backend.hpp"
#include "GPU_HAL.hpp"



namespace HAL
{
	void LoadModule()
	{
		Log("Loading GPU HAL");

		GPU::Load();
	}
}