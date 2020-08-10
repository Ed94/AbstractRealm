#pragma once



#include "OSAL/Platform.hpp"



namespace OSAL
{
	namespace PlatformBackend
	{
		template<OSAL::EOS>
		struct ConsoleTypes_Maker;

		template<>
		struct ConsoleTypes_Maker<EOS::Windows>
		{
			struct EHandle
			{
				using NativeT = DWORD;

				static constexpr NativeT Input  = STD_INPUT_HANDLE ;
				static constexpr NativeT Output = STD_OUTPUT_HANDLE;
				static constexpr NativeT Error  = STD_ERROR_HANDLE ;
			};

			using Rect   = SMALL_RECT;
			using Extent = COORD     ;
			using CharU  = CHAR_INFO ;
		};

		using ConsoleTypes = ConsoleTypes_Maker<OSAL::OS>;

		template<OSAL::EOS>
		struct ConsoleAPI_Maker;

		template<>
		struct ConsoleAPI_Maker<EOS::Windows>
		{
			using EHandle = ConsoleTypes::EHandle::NativeT;

			static OS_Handle CreateBuffer()
			{
				return CreateConsoleScreenBuffer(GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
			}

			static OS_Handle GetConsoleHandle(EHandle _type)
			{
				return GetStdHandle(_type);
			}

			static bool SetBufferSize(OS_Handle _handle, ConsoleTypes::Extent _extent)
			{
				return SetConsoleScreenBufferSize(_handle, _extent);
			}

			static bool SetSize(OS_Handle _handle, ConsoleTypes::Rect& _rect)
			{
				return SetConsoleWindowInfo(_handle, TRUE, &_rect);
			}

			static bool SetTitle(OS_RoCStr _title)
			{
				return SetConsoleTitle(_title);
			}
		};


		using ConsoleAPI = ConsoleAPI_Maker<OSAL::OS>;
	}

	using PlatformBackend::ConsoleTypes;

	using EConsoleHandle = ConsoleTypes::EHandle;
	using ConsoleRect = ConsoleTypes::Rect;
	using ConsoleExtent = ConsoleTypes::Extent;
	using CharU = ConsoleTypes::CharU;

	OS_Handle Console_CreateBuffer();
	
	OS_Handle Console_GetHandle(EConsoleHandle::NativeT _type);

	bool Console_SetBufferSize(OS_Handle _handle, ConsoleExtent _extent);

	bool Console_SetSize(OS_Handle _handle, ConsoleRect _rect);

	bool Console_SetTitle(OS_RoCStr _title);
}
