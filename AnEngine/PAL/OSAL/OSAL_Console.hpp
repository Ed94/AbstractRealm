/* 
OSAL_Console

This is currently only supports windows and doesn't do a proper abstraction for platform agnosticism. 

*/


#pragma once



#include "OSAL_Platform.hpp"



namespace OSAL
{
	using namespace LAL;

	namespace PlatformBackend
	{
		template<OSAL::EOS>
		struct ConsoleTypes_Maker;

		template<>
		struct ConsoleTypes_Maker<EOS::Windows>
		{
			enum class EAttributeFlag : WORD
			{
				Foreground_Red       = FOREGROUND_RED      ,
				Foreground_Green     = FOREGROUND_GREEN    ,
				Foreground_Blue      = FOREGROUND_BLUE     ,
				Foreground_Intensity = FOREGROUND_INTENSITY,
				Background_Red       = BACKGROUND_RED      ,
				Background_Green     = BACKGROUND_GREEN    ,
				Background_Blue      = BACKGROUND_BLUE     ,
				Background_Intensity = BACKGROUND_INTENSITY
			};

			using AttributeFlags = Bitmask<EAttributeFlag, WORD>;

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

		SpecifyBitmaskable(ConsoleTypes_Maker<EOS::Windows>::EAttributeFlag);

		using ConsoleTypes = ConsoleTypes_Maker<OSAL::OS>;

		template<OSAL::EOS>
		struct ConsoleAPI_Maker;

		template<>
		struct ConsoleAPI_Maker<EOS::Windows>
		{
			using EHandle = ConsoleTypes::EHandle;
			using Extent  = ConsoleTypes::Extent ;
			using Rect    = PSMALL_RECT          ;
			using CharU   = ConsoleTypes::CharU  ;

			// https://stackoverflow.com/questions/311955/redirecting-cout-to-a-console-in-windows
			static void Bind_IOBuffersTo_OSIO()
			{
				FILE* dummyFile;

				freopen_s(&dummyFile, "nul", "r", stdin );
				freopen_s(&dummyFile, "nul", "w", stdout);
				freopen_s(&dummyFile, "nul", "w", stderr);

				OS_Handle stdHandle = GetConsoleHandle(EHandle::Input);

				if (stdHandle != OS_InvalidHandle)
				{
					int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);

					if (fileDescriptor != -1)
					{
						FILE* file = _fdopen(fileDescriptor, "r");

						if (file != nullptr)
						{
							int dup2Result = _dup2(_fileno(file), _fileno(stdin));

							if (dup2Result == 0)
							{
								setvbuf(stdin, nullptr, _IONBF, 0);
							}
						}
					}
				}

				stdHandle = GetConsoleHandle(EHandle::Output);

				if (stdHandle != OS_InvalidHandle)
				{
					int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);

					if (fileDescriptor != -1)
					{
						FILE* file = _fdopen(fileDescriptor, "w");

						if (file != nullptr)
						{
							int dup2Result = _dup2(_fileno(file), _fileno(stdout));

							if (dup2Result == 0)
							{
								setvbuf(stdout, nullptr, _IONBF, 0);
							}
						}
					}
				}

				stdHandle = GetConsoleHandle(EHandle::Error);

				if (stdHandle != OS_InvalidHandle)
				{
					int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);

					if (fileDescriptor != -1)
					{
						FILE* file = _fdopen(fileDescriptor, "w");

						if (file != nullptr)
						{
							int dup2Result = _dup2(_fileno(file), _fileno(stderr));

							if (dup2Result == 0)
							{
								setvbuf(stderr, nullptr, _IONBF, 0);
							}
						}
					}
				}

				std::wcin .clear();
				std::cin  .clear();
				std::wcout.clear();
				std::cout .clear();
				std::wcerr.clear();
				std::cerr .clear();
			}

			static OS_Handle CreateBuffer()
			{
				return CreateConsoleScreenBuffer(GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
			}

			static bool Create()
			{
				return AllocConsole();
			}

			static bool Destroy()
			{
				return FreeConsole();
			}

			static OS_Handle GetConsoleHandle(EHandle::NativeT _type)
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

			static bool WriteToConsole
			(
				OS_Handle _handle     ,
				CharU*    _buffer     ,
				Extent    _bufferSize ,
				Extent    _bufferCoord,
				Rect      _readRegion
			)
			{
				return WriteConsoleOutput(_handle, _buffer, _bufferSize, _bufferCoord, _readRegion);
			}
		};

		using ConsoleAPI = ConsoleAPI_Maker<OSAL::OS>;
	}

	using PlatformBackend::ConsoleTypes;
	using PlatformBackend::ConsoleAPI  ;

	using ConslAttribFlags = ConsoleTypes::AttributeFlags;
	using EConslAttribFlag = ConsoleTypes::EAttributeFlag;
	using CharU            = ConsoleTypes::CharU         ;
	using ConsoleExtent    = ConsoleTypes::Extent        ;
	using ConsoleRect      = ConsoleTypes::Rect          ;
	using EConsoleHandle   = ConsoleTypes::EHandle       ;

	constexpr auto CreateConsole                 = ConsoleAPI::Create               ;
	constexpr auto DestroyConsole                = ConsoleAPI::Destroy              ;
	constexpr auto Console_Bind_IOBuffersTo_OSIO = ConsoleAPI::Bind_IOBuffersTo_OSIO;
	constexpr auto Console_CreateBuffer          = ConsoleAPI::CreateBuffer         ;
	constexpr auto Console_GetHandle             = ConsoleAPI::GetConsoleHandle     ;
	constexpr auto Console_SetBufferSize         = ConsoleAPI::SetBufferSize        ;
	constexpr auto Console_SetSize               = ConsoleAPI::SetSize              ;
	constexpr auto Console_SetTitle              = ConsoleAPI::SetTitle             ;
	constexpr auto WriteToConsole                = ConsoleAPI::WriteToConsole       ;
}
