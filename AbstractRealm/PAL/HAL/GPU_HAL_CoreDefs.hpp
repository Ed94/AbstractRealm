#pragma once



#include "LAL/LAL.hpp"



namespace HAL::GPU
{
	using namespace LAL;


	// Structs

	namespace WIP
	{
		/*
		Used for keeping track of physical GPUs present.
		*/
		struct PhysicalDevice
		{
			String Name;
		};
	}



	struct ARenderContext {};
}
