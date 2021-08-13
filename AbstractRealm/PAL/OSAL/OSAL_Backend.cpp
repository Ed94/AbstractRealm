// Parent Header
#include "OSAL_Backend.hpp"



#include "Dev/Dev.hpp"
#include "Dev/Log.hpp"



namespace OSAL
{
	using namespace LAL;

	Dev_Declare_Log(OSAL);

	void Load_Backend()
	{
		SubLogger.Init();
	}
}