// Parent Header
#include "OSAL_EntryPoint.hpp"



#include "OSAL_Timing.hpp"



// DEFINE YOUR INCLUDE WITH YOUR ENTRY POINT HERE.
#include "Core/Execution/Executer.hpp"



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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	OSAL::Record_EntryPoint_StartExecution();

	OSAL::AppInstance = hInstance;

	OSAL::PlatformBackend::EntryPoint();

	/**
	* Due to my lack of knowledge this is the smallest required execution I'm aware of that a Win32 App needs in order to safely close.
	*/
	MSG      msg = { 0 };
	WNDCLASS wc  = { 0 };

	wc.lpfnWndProc   = WndProc                   ;
	wc.hInstance     = hInstance                 ;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"minwindowsapp"          ;

	if (!RegisterClass(&wc))
		return 1;

	if (!CreateWindow(wc.lpszClassName, L"Abstract Realm: Closing Win", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 400, 30, 0, 0, hInstance, NULL))
		return 2;

	PostQuitMessage(0);

	while (GetMessage(&msg, NULL, 0, 0) > 0)
		DispatchMessage(&msg);

	return 0;
}

#endif