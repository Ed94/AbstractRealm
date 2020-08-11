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

			// TODO: Maximize use of generic types / function binds even though this is just for windows.
			static void Console_Bind_IOBuffersTo_OSIO()
			{
				// Re-initialize the C runtime "FILE" handles with clean handles bound to "nul". We do this because it has been
				// observed that the file number of our standard handle file objects can be assigned internally to a value of -2
				// when not bound to a valid target, which represents some kind of unknown internal invalid state. In this state our
				// call to "_dup2" fails, as it specifically tests to ensure that the target file number isn't equal to this value
				// before allowing the operation to continue. We can resolve this issue by first "re-opening" the target files to
				// use the "nul" device, which will place them into a valid state, after which we can redirect them to our target
				// using the "_dup2" function.

				FILE* dummyFile;
				freopen_s(&dummyFile, "nul", "r", stdin);

				freopen_s(&dummyFile, "nul", "w", stdout);

				freopen_s(&dummyFile, "nul", "w", stderr);

				// Redirect unbuffered stdin from the current standard input handle

				HANDLE stdHandle = GetStdHandle(STD_INPUT_HANDLE);

				if (stdHandle != INVALID_HANDLE_VALUE)
				{
					int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
					if (fileDescriptor != -1)
					{
						FILE* file = _fdopen(fileDescriptor, "r");
						if (file != NULL)
						{
							int dup2Result = _dup2(_fileno(file), _fileno(stdin));
							if (dup2Result == 0)
							{
								setvbuf(stdin, NULL, _IONBF, 0);
							}
						}
					}
				}

				// Redirect unbuffered stdout to the current standard output handle

				stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

				if (stdHandle != INVALID_HANDLE_VALUE)
				{
					int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
					if (fileDescriptor != -1)
					{
						FILE* file = _fdopen(fileDescriptor, "w");
						if (file != NULL)
						{
							int dup2Result = _dup2(_fileno(file), _fileno(stdout));
							if (dup2Result == 0)
							{
								setvbuf(stdout, NULL, _IONBF, 0);
							}
						}
					}
				}

				stdHandle = GetStdHandle(STD_ERROR_HANDLE);

				if (stdHandle != INVALID_HANDLE_VALUE)
				{
					int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
					if (fileDescriptor != -1)
					{
						FILE* file = _fdopen(fileDescriptor, "w");
						if (file != NULL)
						{
							int dup2Result = _dup2(_fileno(file), _fileno(stderr));
							if (dup2Result == 0)
							{
								setvbuf(stderr, NULL, _IONBF, 0);
							}
						}
					}
				}

				// Clear the error state for each of the C++ standard stream objects. We need to do this, as attempts to access the
				// standard streams before they refer to a valid target will cause the iostream objects to enter an error state. In
				// versions of Visual Studio after 2005, this seems to always occur during startup regardless of whether anything
				// has been read from or written to the targets or not.
				std::wcin.clear();
				std::cin.clear();
				std::wcout.clear();
				std::cout.clear();
				std::wcerr.clear();
				std::cerr.clear();
			}

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
	using PlatformBackend::ConsoleAPI;

	using EConsoleHandle = ConsoleTypes::EHandle;
	using ConsoleRect = ConsoleTypes::Rect;
	using ConsoleExtent = ConsoleTypes::Extent;
	using CharU = ConsoleTypes::CharU;

	OS_Handle Console_CreateBuffer();
	
	OS_Handle Console_GetHandle(EConsoleHandle::NativeT _type);

	bool Console_SetBufferSize(OS_Handle _handle, ConsoleExtent _extent);

	bool Console_SetSize(OS_Handle _handle, ConsoleRect _rect);

	bool Console_SetTitle(OS_RoCStr _title);

	constexpr auto Console_Bind_IOBuffersTo_OSIO = ConsoleAPI::Console_Bind_IOBuffersTo_OSIO;
}
