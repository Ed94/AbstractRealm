#ifdef _WIN32
#define ALLOW_POLLUTION
#include "API.Windows.Backend.hpp"
// PAL
#include "LAL.Declarations.hpp"
#include "LAL.Platform.hpp"
#include "OSAL.Entrypoint.hpp"
#include "OSAL.Platform.hpp"
#include "LAL.Memory.hpp"
// MS
#include <corecrt_malloc.h>


#ifdef BUILD_NO_DEFAULT_LIB
#pragma region ExtenralSymbols
ExternLink "C"	int _fltused;
#pragma endregion ExternalSymbols

#pragma region CRT
void __stdcall 
WinMainCRTStartup()
{
	int Result = 
	WinMain(GetModuleHandle(0), 0, 0, 0);
	ExitProcess(Result);
}
#pragma endregion CRT
#endif

#pragma region Static Data
InternLink
HINSTANCE AppInstance;
#pragma endregion Static Data

#pragma region Allocator API
namespace OSAL
{
#if defined(Compiler_MSVC) || defined(Compiler_GCC) || defined(Compiler_TinyC)
	constexpr auto _malloc	= _aligned_malloc;
	constexpr auto _free	= _aligned_free;
#endif

	p<void>	Alloc_WAlign(sw size, sw alignment)
	{
		p<void> ptr = nullptr;

		if (! alignment)
			alignment = DefaultAlignment;

		ptr = _malloc(size, alignment);

		return ptr;
	}

	void Free(p<void> ptr)
	{
		_free(ptr);
	}

	// Does nothing.
	void Free_All()
	{ }

	p<void> Resize_WAlign(p<void> ptr, sw oldSize, sw newSize, sw alignment)
	{
		if (! ptr)
			return _malloc(newSize, alignment);

		if (newSize == 0) 
		{
			_free(ptr);
			return nullptr;
		}

		if (newSize < oldSize) 
			newSize = oldSize;

		if (oldSize == newSize)
			return ptr;

		p<void> newMemory = _malloc(newSize, alignment);

		if (! newMemory) 
			return nullptr;

		Memory::Move(newMemory, ptr, min(newSize, oldSize));

		_free(ptr);

		return newMemory;
	}
}
#pragma endregion Allocator API

#pragma region CRT
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
#pragma endregion CRT
#endif
