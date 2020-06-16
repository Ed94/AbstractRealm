// Parent Header
#include "HAL.hpp"



#include "GPU_HAL.hpp"



namespace HAL
{
	void LoadModule()
	{
		GPU::Load();
	}
}