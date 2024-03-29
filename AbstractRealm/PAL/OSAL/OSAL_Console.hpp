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
		struct ConsoleAPI_Maker;

		template<>
		struct ConsoleAPI_Maker<EOS::Windows>
		{
#pragma region Types
			enum class EAttributeFlag : WORD
			{
				Foreground_Red       = FOREGROUND_RED      ,
				Foreground_Green     = FOREGROUND_GREEN    ,
				Foreground_Blue      = FOREGROUND_BLUE     ,
				Foreground_Intensity = FOREGROUND_INTENSITY,
				Background_Red       = BACKGROUND_RED      ,
				Background_Green     = BACKGROUND_GREEN    ,
				Background_Blue      = BACKGROUND_BLUE     ,
				Background_Intensity = BACKGROUND_INTENSITY,

				SpecifyBitmaskable = sizeof(WORD)
			};

			using AttributeFlags = Bitfield<EAttributeFlag, WORD>;

			struct EHandle
			{
				using NativeT = DWORD;

				static constexpr NativeT Input  = STD_INPUT_HANDLE ;
				static constexpr NativeT Output = STD_OUTPUT_HANDLE;
				static constexpr NativeT Error  = STD_ERROR_HANDLE ;
			};

			using Rect   = SMALL_RECT;
			using Extent = COORD     ;
			using Char   = CHAR_INFO ;
#pragma endregion Types

			// CAUSED MEMORY LEAK. (Sigh)
			// https://stackoverflow.com/questions/311955/redirecting-cout-to-a-console-in-windows   
			// Chris's answer. (He did it right)
		    // https://stackoverflow.com/questions/191842/how-do-i-get-console-output-in-c-with-a-windows-program
			static bool Bind_IOBuffersTo_OSIO  ();
			static bool Unbind_IOBuffersTo_OSIO();
			
			static OS_Handle CreateBuffer();

			static bool Create();
			
			static bool Destroy();
			
			static OS_Handle GetConsoleHandle(EHandle::NativeT _type);

			static bool SetBufferSize(OS_Handle _handle, Extent _extent);

			static bool SetSize(OS_Handle _handle, const Rect& _rect);

			static bool SetTitle(OS_RoCStr _title);

			static bool WriteToConsole
			(
				OS_Handle _handle       ,
				ptr<Char> _buffer_in    ,
				Extent    _bufferSize   ,
				Extent    _bufferCoord  , 
				Rect&     _readRegion_in
			);
		};

		using ConsoleAPI = ConsoleAPI_Maker<OSAL::OS>;


		// The console black boxed properly...

		template<EOS> class Console;
		
		template<> class Console<EOS::Windows>
		{

		protected:
			SHORT ConsoleWidth, ConsoleHeight;
		};
	}

	using PlatformBackend::ConsoleAPI;

	using ConslAttribFlags = ConsoleAPI::AttributeFlags;
	using EConslAttribFlag = ConsoleAPI::EAttributeFlag;
	using ConsoleChar      = ConsoleAPI::Char          ;
	using ConsoleExtent    = ConsoleAPI::Extent        ;
	using ConsoleRect      = ConsoleAPI::Rect          ;
	using EConsoleHandle   = ConsoleAPI::EHandle       ;

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
