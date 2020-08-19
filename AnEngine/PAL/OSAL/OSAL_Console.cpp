// Parent Header
#include "OSAL_Console.hpp"



namespace OSAL
{
	namespace PlatformBackend
	{
		FILE* ConsoleAPI_Maker<EOS::Windows>::InFile = nullptr;
		FILE* ConsoleAPI_Maker<EOS::Windows>::OutFile = nullptr;
		FILE* ConsoleAPI_Maker<EOS::Windows>::ErrFile = nullptr;
	}
}
