// Parent Header
#include "Console.hpp"



#include "Meta/EngineInfo.hpp"

#include "OSAL/OSAL_Console.hpp"
#include "OSAL/Timing.hpp"





namespace Dev
{
	using namespace LAL;
	using namespace OSAL;

	// Static Data

	// Private

	OS_Handle ConsoleOutput;
	OS_Handle ConsoleInput ;

	ConsoleRect ConsoleSize = 
	{
		0, 0,
		ConsoleWidth -1,   // Width
		ConsoleHeight-1     // Height	
	};

	ConsoleExtent ConsoleBufferSize = { ConsoleWidth, ConsoleHeight };

	OS_RoCStr ConsoleTitle = L"Abstract Realm: Dev Console";

	StaticArray<OS_Handle, 2> ConsoleBuffers;

	using CharBuffer = CharU[ConsoleHeight * ConsoleWidth];

	CharBuffer    ConsoleCharBuffer; 
	ConsoleExtent ConsoleCharBufferSize = { ConsoleWidth, ConsoleHeight };
	ConsoleExtent ConsoleCharPos   = { 0, 0 };

	ConsoleRect ConsoleWriteArea =
	{
		0, 0,
		ConsoleWidth - 1,   // Width
		ConsoleHeight - 1     // Height	
	};

	OS_Handle FrontBuffer = ConsoleBuffers[0],
		      BackBuffer  = ConsoleBuffers[1] ;

	// 0-3, 4-70, 71-90: Engine Title, DevLog, Status

	uInt16 DevLogLineEnd = 44;
	uInt16 StatusStart   = 45;



	// Public functions

	std::stringstream CharStream;

	void WriteToBufferLine(int _line, WORD _flags)
	{
		auto str = CharStream.str();

		for (int index = 0; index < str.size(); index++)
		{
			ConsoleCharBuffer[index + ConsoleWidth * _line].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[index + ConsoleWidth * _line].Attributes = _flags;
		}
	}

	void ClearBuffer()
	{
		for (uInt16 y = 0; y < ConsoleHeight; ++y)
		{
			for (uInt16 x = 0; x < ConsoleWidth; ++x)
			{
				ConsoleCharBuffer[x + ConsoleWidth * y].Char.UnicodeChar = ' ';
				ConsoleCharBuffer[x + ConsoleWidth * y].Attributes = 0;
			}
		}
	}

	void CLog(String _lineToLog)
	{
		if (Meta::UseDebug)
		{
			static uInt16 linePos = 4;

			if (linePos == StatusStart )
			{
				linePos = 4;
			}

			CharStream.str(String());

			CalendarDate dateSnapshot = OSAL::GetTime_Local();

			CharStream
				<< "[" << put_time(&dateSnapshot,"%F %I:%M:%S %p") << "] "
				<< _lineToLog 
				<< setfill(' ')
				<< setw(ConsoleWidth - CharStream.str().size()) << ' ';

			WriteToBufferLine(linePos-1, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

			CharStream.str(String());

			CharStream
				<< setfill('-')
				<< setw(ConsoleWidth) << '-';

			WriteToBufferLine(linePos++, BACKGROUND_INTENSITY );

			WriteConsoleOutput(ConsoleOutput, ConsoleCharBuffer, ConsoleCharBufferSize, ConsoleCharPos, &ConsoleWriteArea);
		}
	}

	void CLog_Error(String _lineToLog)
	{

	}
	
	void Load_CharStream_EngineTitle()
	{
		using namespace Meta;

		CharStream << setfill('-') << setw(ConsoleWidth) << '-';

		WriteToBufferLine(0, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		CharStream.str(std::string());

		CharStream << "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << setfill(' ') << setw(ConsoleWidth - CharStream.str().size()) << ' ';

		WriteToBufferLine(1, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		CharStream.str(String());

		CharStream << setfill('-') << setw(ConsoleWidth) << '-';

		WriteToBufferLine(2, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}

	void Load_CharStream_DevLog()
	{
		for (uInt16 line = 3; line < DevLogLineEnd; line++)
		{
			CharStream.str(std::string());

			CharStream << setw(ConsoleWidth) << setfill(' ') << ' ';

			WriteToBufferLine(line, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		}
	}

	void Load_CharStream_Status()
	{
		CharStream.str(std::string());

		CharStream << setfill('=') << setw(ConsoleWidth) << '=';
		
		WriteToBufferLine(StatusStart, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		CharStream.str(std::string());

		CharStream << "Status: Not Setup.";

		WriteToBufferLine(StatusStart + 1, FOREGROUND_RED | FOREGROUND_INTENSITY);
		
		for (uInt16 line = StatusStart + 2; line < ConsoleHeight; line++)
		{
			CharStream.str(std::string());

			CharStream << setfill(' ') << setw(ConsoleWidth) << ' ';

			WriteToBufferLine(line, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		}
	}
	
	void Load_DevConsole()
	{
		cout << "Dev: Load Console Buffer" << endl;
		
		OSAL::Console_SetTitle(ConsoleTitle);

		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);
		OSAL::Console_SetSize      (ConsoleOutput, ConsoleSize);
		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);

		cout << "Dev: Console buffers loaded" << endl;

		ClearBuffer();

		WriteConsoleOutputA(ConsoleOutput, ConsoleCharBuffer, ConsoleCharBufferSize, ConsoleCharPos, &ConsoleWriteArea);

		SetConsoleCursorPosition (ConsoleOutput, {0, 140});

		Load_CharStream_EngineTitle();

		Load_CharStream_DevLog();

		Load_CharStream_Status();

		CLog("Dev: Console Submodules ready.");
	}

	void SetConsole_IOHooks()
	{
		ConsoleOutput = OSAL::Console_GetHandle(EConsoleHandle::Output);
		ConsoleInput  = OSAL::Console_GetHandle(EConsoleHandle::Input);

		OSAL::Console_Bind_IOBuffersTo_OSIO();
	}
}