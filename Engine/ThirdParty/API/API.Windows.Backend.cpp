#ifdef _WIN32
#define BACKEND_UNIT
#include "API.Windows.Backend.hpp"

#include "LAL.Declarations.hpp"
//#include "OSAL.Entrypoint.hpp"
import OSAL.EntryPoint;


#pragma region ExtenralSymbols
#ifdef BUILD_NO_DEFAULT_LIB
ExternLink "C"	int _fltused;
#endif
#pragma endregion ExternalSymbols

#pragma region Static Data
InternLink
HINSTANCE AppInstance;
#pragma endregion Static Data

#pragma region Procedures
#ifdef BUILD_NO_DEFAULT_LIB
void __stdcall 
WinMainCRTStartup()
{
	int Result = 
	WinMain(GetModuleHandle(0), 0, 0, 0);
	ExitProcess(Result);
}
#endif

int WINAPI 
WinMain(
	HINSTANCE	hInstance, 
	HINSTANCE	/* hPrevInstance */, 
	LPSTR		/* lpCmdLine */, 
	int			/* nShowCmd */)
{
	// OSAL::Record_EntryPoint_StartExecution();

	 AppInstance = hInstance;

	 OSAL::EExitVal 
	 result = OSAL::EntryPoint();

	 return int(result);
}
#endif
#pragma endregion Procedures