// Parent Header
#include "OSAL_Console.hpp"


#include "Core/Memory/MemTracking.hpp"
#include "OSAL_Backend.hpp"



namespace OSAL
{
	namespace PlatformBackend
	{
		using namespace Core::Memory;

		//template<>
		bool ConsoleAPI_Maker<EOS::Windows>::Bind_IOBuffersTo_OSIO()
		{				 
			bool result = false;

			FILE* dummyFile = nullptr;

			freopen_s(&dummyFile, "CONIN$" , "r", stdin );
			freopen_s(&dummyFile, "CONOUT$", "w", stdout);
			freopen_s(&dummyFile, "CONOUT$", "w", stderr);

			// Redirect STDIN if the console has an input handle
			if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
			{
				if (freopen_s(&dummyFile, "CONIN$", "r", stdin) != 0)
				{
					result = false;
				}
				else
				{
					setvbuf(stdin, NULL, _IONBF, 0);
				}
			}
				
			// Redirect STDOUT if the console has an output handle
			if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE)
			{
				if (freopen_s(&dummyFile, "CONOUT$", "w", stdout) != 0)
				{
					result = false;
				}
				else
				{
				setvbuf(stdout, NULL, _IONBF, 0);
				}
			}

			// Redirect STDERR if the console has an error handle
			if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
			{
				if (freopen_s(&dummyFile, "CONOUT$", "w", stderr) != 0)
				{
					result = false;
				}
				else
				{
					setvbuf(stderr, NULL, _IONBF, 0);
				}
			}
			
			// Make C++ standard streams point to console as well.
			std::ios::sync_with_stdio(true);

			// Clear the error state for each of the C++ standard streams.
			std::wcout.clear();
			std::cout .clear();
			std::wcerr.clear();
			std::cerr .clear();
			std::wcin .clear();
			std::cin  .clear();

			return true;
		}

		bool ConsoleAPI_Maker<EOS::Windows>::Unbind_IOBuffersTo_OSIO()
		{
			bool result = true;

			FILE* dummyFile;

			// Just to be safe, redirect standard IO to NUL before releasing.

			// Redirect STDIN to NUL
			if (freopen_s(&dummyFile, "NUL:", "r", stdin) != 0)
			{
				result = false;
			}
			else
			{
				setvbuf(stdin, NULL, _IONBF, 0);
			}

			// Redirect STDOUT to NUL
			if (freopen_s(&dummyFile, "NUL:", "w", stdout) != 0)
			{
				result = false;
			}
			else
			{
				setvbuf(stdout, NULL, _IONBF, 0);
			}

			// Redirect STDERR to NUL
			if (freopen_s(&dummyFile, "NUL:", "w", stderr) != 0)
			{
				result = false;
			}
			else
			{
				setvbuf(stderr, NULL, _IONBF, 0);
			}

			return result;
		}

		OS_Handle ConsoleAPI_Maker<EOS::Windows>::CreateBuffer()
		{
			return CreateConsoleScreenBuffer(GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		}

		bool ConsoleAPI_Maker<EOS::Windows>::Create()
		{
			static bool created = false;

			if (!created)
			{
				created = AllocConsole();

				//auto handle = GetStdHandle(EHandle::Output);

				created = Bind_IOBuffersTo_OSIO();

				if (created) cout << "Console created" << endl;

				return created;
			}
			else
			{
				return false;
			}
		}

		bool ConsoleAPI_Maker<EOS::Windows>::Destroy()
		{
			bool result = Unbind_IOBuffersTo_OSIO();

			if (!result) return result;

			// Detach from console
			result = FreeConsole();

			if (result) CLog("Console destroyed");

			return result;
		}

		OS_Handle ConsoleAPI_Maker<EOS::Windows>::GetConsoleHandle(EHandle::NativeT _type)
		{
			return GetStdHandle(_type);
		}

		bool ConsoleAPI_Maker<EOS::Windows>::SetBufferSize(OS_Handle _handle, ConsoleTypes::Extent _extent)
		{
			return SetConsoleScreenBufferSize(_handle, _extent);
		}

		bool ConsoleAPI_Maker<EOS::Windows>::SetSize(OS_Handle _handle, ConsoleTypes::Rect& _rect)
		{
			return SetConsoleWindowInfo(_handle, TRUE, &_rect);
		}

		bool ConsoleAPI_Maker<EOS::Windows>::SetTitle(OS_RoCStr _title)
		{
			return SetConsoleTitle(_title);
		}

		bool ConsoleAPI_Maker<EOS::Windows>::WriteToConsole
		(
			OS_Handle _handle     ,
			Char*    _buffer     ,
			Extent    _bufferSize ,
			Extent    _bufferCoord,
			Rect      _readRegion
		)
		{
			auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

			if (handle == INVALID_HANDLE_VALUE) return false;

			return WriteConsoleOutput(_handle, _buffer, _bufferSize, _bufferCoord, _readRegion);
		}
	}
}
