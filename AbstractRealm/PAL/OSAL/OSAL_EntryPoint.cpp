// Parent Header
#include "OSAL_EntryPoint.hpp"



// Engine
#include "OSAL_Timing.hpp"



// DEFINE YOUR INCLUDE WITH YOUR ENTRY POINT HERE.
#include "Core/Execution/Executer.hpp"


// VLD
//#include <vld/out/build/x64-Debug (default)/include/vld.h>




namespace OSAL
{
	OS_AppHandle AppInstance;

	namespace PlatformBackend
	{
		/**
		 * DEFINE YOUR ENTRYPOINT FUNCTION HERE.
		 */
		constexpr auto EntryPoint = Core::Execution::EntryPoint;
	}
}



#ifdef _WIN32

// Required...
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CLOSE: PostQuitMessage(0); break;

		default: return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /* hPrevInstance */, LPSTR /* lpCmdLine */, int /* nShowCmd */)
{
	OSAL::Record_EntryPoint_StartExecution();

	OSAL::AppInstance = hInstance;

	return OSAL::PlatformBackend::EntryPoint();
}

#endif