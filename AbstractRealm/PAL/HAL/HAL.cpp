// Parent Header
#include "HAL/HAL.hpp"



// Engine
#include "HAL_Backend.hpp"
#include "HAL_GPU.hpp"



namespace HAL
{
	void Load()
	{
		Load_Backend();

		Log("Loading module.");

		Log("Loading GPU HAL");

		GPU::Load();
	}

	void Unload()
	{
		Log("Unloading module.");
	}
}