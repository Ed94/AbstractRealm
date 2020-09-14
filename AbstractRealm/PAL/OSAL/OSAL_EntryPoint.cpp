// Parent Header
#include "OSAL_EntryPoint.hpp"



// Engine
#include "OSAL_Timing.hpp"



namespace OSAL
{
	OS_AppHandle AppInstance;
}



#ifdef _WIN32

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /* hPrevInstance */, LPSTR /* lpCmdLine */, int /* nShowCmd */)
{
	OSAL::Record_EntryPoint_StartExecution();

	OSAL::AppInstance = hInstance;

	return OSAL::EntryPoint();
}

#endif