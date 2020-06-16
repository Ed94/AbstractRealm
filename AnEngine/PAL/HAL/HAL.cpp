// Parent Header
#include "HAL/HAL.hpp"



#include "GPU_HAL.hpp"



namespace HAL
{
	void LoadModule()
	{
		GPU::Load();
	}
}