#pragma once



#include "LAL/LAL.hpp"



namespace HAL
{
	/*
	Loads the HAL module.

	Submodules that will be loaded:
	Audio
	GPU  
	*/
	void LoadModule();

	/*
	Unloads the HAL module.

	Submodules that will be unloaded:
	Audio
	GPU
	*/
	void UnloadModule();
}
