#pragma once



#include "LAL/LAL.hpp"



namespace HAL
{
	/*
	Loads the HAL submodule.

	Submodules that will be loaded:
	Audio
	GPU
	*/
	void Load();

	/*
	Unloads the HAL submodule.

	Submodules that will be unloaded:
	Audio
	GPU
	*/
	void Unload();
}
