// Parent Header
#include "OSAL_Console.hpp"


#include "Core/Memory/MemTracking.hpp"
#include "OSAL_Backend.hpp"


namespace OSAL
{
	namespace PlatformBackend
	{
		using ConsoleAPI_Win = ConsoleAPI_Maker<EOS::Windows>;

		bool ConsoleAPI_Win::Bind_IOBuffersTo_OSIO()
		{				 
			bool result = false;

			ptr<FILE> dummyFile = nullptr;

			freopen_s(getPtr(dummyFile), "CONIN$" , "r", stdin );
			freopen_s(getPtr(dummyFile), "CONOUT$", "w", stdout);
			freopen_s(getPtr(dummyFile), "CONOUT$", "w", stderr);

			// Redirect STDIN if the console has an input handle
			if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
			{
				if (freopen_s(getPtr(dummyFile), "CONIN$", "r", stdin) != 0)
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
				if (freopen_s(getPtr(dummyFile), "CONOUT$", "w", stdout) != 0)
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
				if (freopen_s(getPtr(dummyFile), "CONOUT$", "w", stderr) != 0)
				{
					result = false;
				}
				else
				{
					setvbuf(stderr, NULL, _IONBF, 0);
				}
			}
			
			// Make C++ standard streams point to console as well.
			SyncWith_STDIO(true);

			// Clear the error state for each of the C++ standard streams.
			std::cerr .clear();
			std::cin  .clear();
			std::cout .clear();
			std::wcerr.clear();
			std::wcin .clear();
			std::wcout.clear();

			return true;
		}

		bool ConsoleAPI_Win::Unbind_IOBuffersTo_OSIO()
		{
			bool result = true;

			ptr<FILE> dummyFile = nullptr;

			// Just to be safe, redirect standard IO to NUL before releasing.

			// Redirect STDIN to NUL
			if (freopen_s(getPtr(dummyFile), "NUL:", "r", stdin) != 0)
			{
				result = false;
			}
			else
			{
				setvbuf(stdin, NULL, _IONBF, 0);
			}

			// Redirect STDOUT to NUL
			if (freopen_s(getPtr(dummyFile), "NUL:", "w", stdout) != 0)
			{
				result = false;
			}
			else
			{
				setvbuf(stdout, NULL, _IONBF, 0);
			}

			// Redirect STDERR to NUL
			if (freopen_s(getPtr(dummyFile), "NUL:", "w", stderr) != 0)
			{
				result = false;
			}
			else
			{
				setvbuf(stderr, NULL, _IONBF, 0);
			}

			return result;
		}

		OS_Handle ConsoleAPI_Win::CreateBuffer()
		{
			return CreateConsoleScreenBuffer(GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		}

		bool ConsoleAPI_Win::Create()
		{
			static bool _Created = false;

			if (!_Created)
			{
				_Created = AllocConsole();

				//auto handle = GetStdHandle(EHandle::Output);

				_Created = Bind_IOBuffersTo_OSIO();

				if (_Created) 
					cout << "Console created" << endl;

				return _Created;
			}
			else
			{
				return false;
			}
		}

		bool ConsoleAPI_Win::Destroy()
		{
			bool result = Unbind_IOBuffersTo_OSIO();

			if (!result) 
				return result;

			// Detach from console
			result = FreeConsole();

			if (result) 
				Log("Console destroyed");

			return result;
		}

		OS_Handle ConsoleAPI_Win::GetConsoleHandle(EHandle::NativeT _type)
		{
			return GetStdHandle(_type);
		}

		bool ConsoleAPI_Win::SetBufferSize(OS_Handle _handle, Extent _extent)
		{
			return SetConsoleScreenBufferSize(_handle, _extent);
		}

		bool ConsoleAPI_Win::SetSize(OS_Handle _handle, const Rect& _rect)
		{
			return SetConsoleWindowInfo(_handle, TRUE, getPtr(_rect));
		}

		bool ConsoleAPI_Win::SetTitle(OS_RoCStr _title)
		{
			return SetConsoleTitle(_title);
		}

		bool ConsoleAPI_Win::WriteToConsole
		(
			OS_Handle _handle       ,
			ptr<Char> _buffer_in    ,
			Extent    _bufferSize   ,
			Extent    _bufferCoord  ,
			Rect&     _readRegion_in
		)
		{
			auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

			if (handle == INVALID_HANDLE_VALUE) 
				return false;

			return WriteConsoleOutput(_handle, _buffer_in, _bufferSize, _bufferCoord, getPtr(_readRegion_in));
		}
	}
}
