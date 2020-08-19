// Parent Header
#include "Console.hpp"



#include "Meta/EngineInfo.hpp"
#include "OSAL/OSAL_Console.hpp"
#include "OSAL/OSAL_Timing.hpp"



namespace Dev
{
	using namespace LAL ;
	using namespace OSAL;


	// Static Data

	// Private

	using OSAL::CharU;

	using CharBuffer = CharU[ConsoleHeight][ConsoleWidth];

	OS_Handle ConsoleOutput;
	OS_Handle ConsoleInput ;

	ConsoleRect ConsoleSize = 
	{
		0, 0,
		ConsoleWidth - 1,   // Width
		ConsoleHeight- 1    // Height	
	};

	ConsoleExtent ConsoleBufferSize = { ConsoleWidth, ConsoleHeight };

	OS_RoCStr ConsoleTitle = L"Abstract Realm: Dev Console";

	StaticArray<OS_Handle, 2> ConsoleBuffers;

	CharBuffer ConsoleCharBuffer; 

	ConsoleExtent ConsoleCharBufferSize = { ConsoleWidth, ConsoleHeight };
	ConsoleExtent ConsoleCharPos        = { 0           , 0             };

	ConsoleRect ConsoleWriteArea =
	{
		0, 0,
		ConsoleWidth  - 1,   // Width
		ConsoleHeight - 1    // Height	
	};

	OS_Handle FrontBuffer = ConsoleBuffers[0],
		      BackBuffer  = ConsoleBuffers[1] ;

	// 0-3, 4-70, 71-90: Engine Title, DevLog, Status

	uInt16 DevLogLineEnd = 44;
	uInt16 StatusStart   = 45;

	uInt16 StatusColumnWidth = 35;

	StaticArray<StaticArray<Stringstream, 4>, 4> StatusStreams;

	Stringstream DevLogStream;


	// Forwards

	void WriteTo_Buffer      (int _line,           ConslAttribFlags _flags);
	void WriteTo_StatusModule(int _row , int _col, ConslAttribFlags _flags);


	// Public functions

	void ClearBuffer()
	{
		for (uInt16 y = 0; y < ConsoleHeight; ++y)
		{
			for (uInt16 x = 0; x < ConsoleWidth; ++x)
			{
				ConsoleCharBuffer[y][x].Char.UnicodeChar = ' ';
				ConsoleCharBuffer[y][x].Attributes       = 0  ;
			}
		}
	}

	void CLog(String _info)
	{
		if (Meta::UseDebug)
		{
			static uInt16 linePos = 4;

			if (linePos == StatusStart +1 )
			{
				linePos = 4;
			}

			DevLogStream.str(String());

			CalendarDate dateSnapshot = OSAL::GetTime_Local();

			DevLogStream
				<< "[" << put_time(&dateSnapshot,"%F %I:%M:%S %p") << "] "
				<< _info 
				<< setfill(' ')
				<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

			WriteTo_Buffer
			(
				linePos - 1, 
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Green, EConslAttribFlag::Foreground_Blue)
			);

			DevLogStream.str(String());

			DevLogStream
				<< setfill('-')
				<< setw(ConsoleWidth- DevLogStream.str().size()) << '-';

			WriteTo_Buffer(linePos++, ConslAttribFlags(EConslAttribFlag::Background_Intensity) );
		}
	}

	void CLog_Error(String _lineToLog)
	{
		static uInt16 linePos = 4;

		if (linePos == StatusStart +1)
		{
			linePos = 4;
		}

		DevLogStream.str(String());

		CalendarDate dateSnapshot = OSAL::GetTime_Local();

		DevLogStream
			<< "[" << put_time(&dateSnapshot, "%F %I:%M:%S %p") << "] "
			<< _lineToLog
			<< setfill(' ')
			<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

		WriteTo_Buffer(linePos - 1, ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity));

		DevLogStream.str(String());

		DevLogStream
			<< setfill('^')
			<< setw(ConsoleWidth) << '^';

		WriteTo_Buffer(linePos++, ConslAttribFlags(EConslAttribFlag::Foreground_Red));
	}

	void CLog_Status(String _info, int _row, int _col)
	{
		if (Meta::UseDebug)
		{
			StatusStreams[_row][_col].str(String());

			StatusStreams[_row][_col]
				<< _info
				<< setfill(' ')
				<< setw(StatusColumnWidth - StatusStreams[_row][_col].str().size()) << ' ';
		}
	}

	void Console_UpdateBuffer()
	{
		for  (int y = 0; y  < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				auto str = StatusStreams[y][x].str();

				for (int index = 0; index < str.size(); index++)
				{
					int col = index + StatusColumnWidth * x;

					ConsoleCharBuffer[y + StatusStart + 1 ][col].Char.UnicodeChar = str.at(index);

					ConsoleCharBuffer[y + StatusStart + 1 ][col].Attributes = 
						ConslAttribFlags
						(
							EConslAttribFlag::Foreground_Red      , 
							EConslAttribFlag::Foreground_Green    , 
							EConslAttribFlag::Foreground_Blue     , 
							EConslAttribFlag::Foreground_Intensity
						);
				}
			}
		}

		WriteToConsole(ConsoleOutput, &ConsoleCharBuffer[0][0], ConsoleCharBufferSize, ConsoleCharPos, &ConsoleWriteArea);
	}

	void Load_CharStream_DevLog()
	{
		for (uInt16 line = 3; line < DevLogLineEnd; line++)
		{
			DevLogStream.str(std::string());

			DevLogStream << setw(ConsoleWidth) << setfill(' ') << ' ';

			WriteTo_Buffer
			(
				line, 
				ConslAttribFlags
				(
					EConslAttribFlag::Foreground_Red  , 
					EConslAttribFlag::Foreground_Green, 
					EConslAttribFlag::Foreground_Blue
				)
			);
		}
	}
		
	void Load_CharStream_EngineTitle()
	{
		using namespace Meta;

		DevLogStream.str(std::string());

		DevLogStream << setfill('-') << setw(ConsoleWidth) << '-';

		WORD test = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;

		if (FOREGROUND_RED != WORD(EConslAttribFlag::Foreground_Red))
		{
			cout << "whey?" << endl;
		}

		WriteTo_Buffer
		(
			0, 
			ConslAttribFlags
			(
				EConslAttribFlag::Foreground_Red      ,
				EConslAttribFlag::Foreground_Green    ,
				EConslAttribFlag::Foreground_Blue     ,
				EConslAttribFlag::Foreground_Intensity
			)
		);

		DevLogStream.str(std::string());

		DevLogStream << "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << setfill(' ') << setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

		WriteTo_Buffer
		(
			1, 
			ConslAttribFlags
			(
				EConslAttribFlag::Foreground_Red      ,
				EConslAttribFlag::Foreground_Green    ,
				EConslAttribFlag::Foreground_Blue     ,
				EConslAttribFlag::Foreground_Intensity
			)
		);

		DevLogStream.str(String());

		DevLogStream << setfill('-') << setw(ConsoleWidth) << '-';

		WriteTo_Buffer
		(
			2, 
			ConslAttribFlags
			(
				EConslAttribFlag::Foreground_Red      ,  
				EConslAttribFlag::Foreground_Green    ,
				EConslAttribFlag::Foreground_Blue     ,
				EConslAttribFlag::Foreground_Intensity
			)
		);
	}

	
	void Load_CharStream_Status()
	{
		DevLogStream.str(std::string());

		DevLogStream << setfill('=') << setw(ConsoleWidth) << '=';
		
		WriteTo_Buffer
		(
			StatusStart, 
			ConslAttribFlags
			(
				EConslAttribFlag::Foreground_Red      ,
				EConslAttribFlag::Foreground_Green    ,
				EConslAttribFlag::Foreground_Blue     ,
				EConslAttribFlag::Foreground_Intensity
			)
		);

		DevLogStream.str(std::string());

		DevLogStream << "Status: Not Setup.";

		WriteTo_Buffer(StatusStart + 1, ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity));
		
		for (uInt16 line = StatusStart + 2; line < ConsoleHeight; line++)
		{
			DevLogStream.str(std::string());

			DevLogStream << setfill(' ') << setw(ConsoleWidth) << ' ';

			WriteTo_Buffer
			(
				line, 
				ConslAttribFlags
				(
					EConslAttribFlag::Foreground_Red  , 
					EConslAttribFlag::Foreground_Green, 
					EConslAttribFlag::Foreground_Blue
				)
			);
		}
	}
	
	void Load_DevConsole()
	{
		cout << "Dev: Load Console Buffer" << endl;

		SetConsole_IOHooks();
		
		OSAL::Console_SetTitle(ConsoleTitle);

		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);
		OSAL::Console_SetSize      (ConsoleOutput, ConsoleSize);
		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);

		cout << "Dev: Console buffers loaded" << endl;

		ClearBuffer();

		WriteToConsole(ConsoleOutput, &ConsoleCharBuffer[0][0], ConsoleCharBufferSize, ConsoleCharPos, &ConsoleWriteArea);

		SetConsoleCursorPosition (ConsoleOutput, {0, 140});

		Load_CharStream_EngineTitle();

		Load_CharStream_DevLog();

		Load_CharStream_Status();

		Console_UpdateBuffer();

		CLog("Dev: Console Submodules ready.");

		Console_UpdateBuffer();
	}

	void SetConsole_IOHooks()
	{
		ConsoleOutput = OSAL::Console_GetHandle(EConsoleHandle::Output);
		ConsoleInput  = OSAL::Console_GetHandle(EConsoleHandle::Input );
	}

	void WriteTo_Buffer(int _line, ConslAttribFlags _flags)
	{
		auto str = DevLogStream.str();

		for (int index = 0; index < str.size(); index++)
		{
			ConsoleCharBuffer[_line][index].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[_line][index].Attributes       = _flags       ;
		}
	}

	void WriteTo_StatusModule(int _row, int _col, ConslAttribFlags _flags)
	{
		auto str = StatusStreams[_row][_col].str();

		_row = StatusStart + _row + 1;

		for (int index = 0; index < str.size(); index++)
		{
			int col = index + StatusColumnWidth * _col;

			ConsoleCharBuffer[_row][col].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[_row][col].Attributes       = WORD(_flags);
		}
	}
}