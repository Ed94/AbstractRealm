// Parent Header
#include "OSAL_Console.hpp"




namespace OSAL
{
	using PlatformBackend::ConsoleAPI;

	OS_Handle Console_CreateBuffer()
	{
		return ConsoleAPI::CreateBuffer();
	}

	OS_Handle Console_GetHandle(EConsoleHandle::NativeT _type)
	{
		return ConsoleAPI::GetConsoleHandle(_type);
	}

	bool Console_SetBufferSize(OS_Handle _handle, ConsoleExtent _extent)
	{
		return ConsoleAPI::SetBufferSize(_handle, _extent);
	}

	bool Console_SetSize(OS_Handle _handle, ConsoleRect _rect)
	{
		return ConsoleAPI::SetSize(_handle, _rect);
	}

	bool Console_SetTitle(OS_RoCStr _title)
	{
		return ConsoleAPI::SetTitle(_title);
	}
}