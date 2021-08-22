// Parent Header
#include "OSAL_Backend.hpp"

Meta_SetupEngineModule_Implementation(OSAL);


#include "Dev/Dev.hpp"
#include "Dev/Log.hpp"


namespace OSAL
{
	void Load_Backend()
	{
		Meta_EngineModule_Init();
	}
}