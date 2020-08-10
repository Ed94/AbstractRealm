// Parent Header
#include "Console.hpp"



#include "Meta/EngineInfo.hpp"

#include "OSAL/OSAL_Console.hpp"



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

	uInt16 DevLogLineEnd = 40;
	uInt16 StatusStart   = 45;



	// Public functions

	std::stringstream CharStream;

	void WriteToBufferLine(int _line)
	{
		auto str = CharStream.str();

		for (int index = 0; index < str.size(); index++)
		{
			ConsoleCharBuffer[index + ConsoleWidth * _line].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[index + ConsoleWidth * _line].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
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

	void CLog(const CharLine _lineToLog)
	{
		static uInt16 linePos = 3;

		if (linePos == DevLogLineEnd) 
		{
			linePos = 3;
		}

		CharStream.str(std::string());

		CharStream << _lineToLog << std::setw(ConsoleWidth - CharStream.str().size()) << ' ';

		WriteToBufferLine(linePos);

		linePos++;

		WriteConsoleOutput(ConsoleOutput, ConsoleCharBuffer, ConsoleCharBufferSize, ConsoleCharPos, &ConsoleWriteArea);
	}

	void Load_CharStream_EngineTitle()
	{
		using namespace Meta;

		CharStream << std::setfill('-') << std::setw(ConsoleWidth) << '-';

		WriteToBufferLine(0);

		CharStream.str(std::string());

		CharStream << "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << std::setfill(' ') << std::setw(ConsoleWidth - CharStream.str().size()) << ' ';

		WriteToBufferLine(1);

		CharStream.str(std::string());

		CharStream << std::setfill('-') << std::setw(ConsoleWidth) << '-';

		WriteToBufferLine(2);
	}

	void Load_CharStream_DevLog()
	{
		for (uInt16 line = 3; line < DevLogLineEnd; line++)
		{
			CharStream.str(std::string());

			CharStream << std::setw(ConsoleWidth) << std::setfill(' ') << ' ';

			WriteToBufferLine(line);
		}
	}

	void Load_CharStream_Status()
	{
		CharStream.str(std::string());

		CharStream << " " << std::setfill('-') << std::setw(ConsoleWidth) << '-';
		
		WriteToBufferLine(StatusStart);

		CharStream.str(std::string());

		CharStream << "Status: Not Setup.";

		WriteToBufferLine(StatusStart + 1);
		
		for (uInt16 line = StatusStart + 2; line < ConsoleHeight; line++)
		{
			CharStream.str(std::string());

			CharStream << std::setfill(' ') << std::setw(ConsoleWidth) << ' ';

			WriteToBufferLine(line);
		}
	}
	
	void Load_DevConsole()
	{
		cout << "Dev: Load Console Buffer" << endl;

		ConsoleOutput = OSAL::Console_GetHandle(EConsoleHandle::Output);
		ConsoleInput  = OSAL::Console_GetHandle(EConsoleHandle::Input );

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
}