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

			static bool SetBufferSize(OS_Handle _handle, ConsoleTypes::Extent _extent);

			static bool SetSize(OS_Handle _handle, ConsoleTypes::Rect& _rect);

			static bool SetTitle(OS_RoCStr _title);

			static bool WriteToConsole
			(
				OS_Handle _handle     ,
				CharU*    _buffer     ,
				Extent    _bufferSize ,
				Extent    _bufferCoord,
				Rect      _readRegion
			);
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
