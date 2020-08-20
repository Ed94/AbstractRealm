// Parent Header
#include "Console.hpp"



// Engine
#include "Meta/EngineInfo.hpp"
#include "OSAL/OSAL_Console.hpp"
#include "OSAL/OSAL_Timing.hpp"
#include "IO/Basic_FileIO.hpp"



namespace Dev
{
	// Usings

	using namespace LAL ;
	using namespace OSAL;

	using OSAL::CharU;

	using CharBuffer = CharU[ConsoleHeight][ConsoleWidth];



	StaticData
	(
		OS_Handle ConsoleOutput;
		OS_Handle ConsoleInput;

		ConsoleRect ConsoleSize =
		{
			0, 0,
			ConsoleWidth  - 1,   // Width
			ConsoleHeight - 1    // Height	
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

		StaticArray<StaticArray<StringStream, 4>, 4> StatusStreams;

		StringStream DevLogStream;

		File_OutputStream DevLogFile;
	)



	// Forwards
		void Load__DevLogFileStream();
	void Load_DevLogStream           ();
	void Load_DevLogStream_FileStream();
	void Load_DevLogStream_Status    ();
	void Load_DevLogStream_Title     ();

	void WriteTo_Buffer      (int _line,           ConslAttribFlags _flags);
	void WriteTo_StatusModule(int _row , int _col, ConslAttribFlags _flags);



	// Public

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
		if (!Meta::UseDebug) return;

		static uInt16 linePos = 4;

		if (linePos == StatusStart + 1 )
		{
			linePos = 4;
		}

		DevLogStream.str(String());

		CalendarDate dateSnapshot = OSAL::GetTime_Local();

		StringStream dateSig; dateSig << "[" << put_time(&dateSnapshot, "%F %I:%M:%S %p") << "] ";

		DataSize lineLength = dateSig.str().size() + _info.size();

		if (lineLength > ConsoleWidth)
		{
			DevLogStream 
				<< dateSig.str()
				<< setfill(' ')
				<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

			DevLogFile << DevLogStream.str() << endl;

			WriteTo_Buffer
			(
				linePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Green, EConslAttribFlag::Foreground_Blue)
			);

			DevLogStream.str(String());

			DataSize pos = 0, endPos = ConsoleWidth;

			while (!_info.empty())
			{
				linePos ++;

				if (linePos == StatusStart + 1)
				{
					linePos = 4;
				}

				DevLogStream 
					<< _info.substr(0, ConsoleWidth)
					<< setfill(' ')
					<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

				String sub = _info.substr(0, ConsoleWidth);

				DevLogFile << sub << endl;

				WriteTo_Buffer
				(
					linePos - 1,
					ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Green, EConslAttribFlag::Foreground_Blue)
				);

				DevLogStream.str(String());

				if (_info.length() < ConsoleWidth)
				{
					_info.erase(0, _info.length());
				}
				else
				{
					_info.erase(0, ConsoleWidth);
				}

				pos += ConsoleWidth + 1;
				endPos += ConsoleWidth;	
			}
		}
		else
		{
			DevLogStream
				<< dateSig.str()
				<< _info
				<< setfill(' ')
				<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

			DevLogFile << DevLogStream.str() << endl;

			WriteTo_Buffer
			(
				linePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Green, EConslAttribFlag::Foreground_Blue)
			);

			DevLogStream.str(String());
		}	

		DevLogStream
			<< setfill('-')
			<< setw(ConsoleWidth - DevLogStream.str().size()) << '-';

		WriteTo_Buffer(linePos++, ConslAttribFlags(EConslAttribFlag::Background_Intensity));
	}

	void CLog_Error(String _info)
	{
		if (!Meta::UseDebug) return;

		static uInt16 linePos = 4;

		if (linePos == StatusStart + 1)
		{
			linePos = 4;
		}

		DevLogStream.str(String());

		CalendarDate dateSnapshot = OSAL::GetTime_Local();

		StringStream dateSig; dateSig << "[" << put_time(&dateSnapshot, "%F %I:%M:%S %p") << "] ";

		DataSize lineLength = dateSig.str().size() + _info.size();

		if (lineLength > ConsoleWidth)
		{
			DevLogStream
				<< dateSig.str()
				<< setfill(' ')
				<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

			DevLogFile << DevLogStream.str() << endl;

			WriteTo_Buffer
			(
				linePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity)
			);

			DevLogStream.str(String());

			DataSize pos = 0, endPos = ConsoleWidth;

			while (!_info.empty())
			{
				linePos++;

				if (linePos == StatusStart + 1)
				{
					linePos = 4;
				}

				DevLogStream
					<< _info.substr(0, ConsoleWidth)
					<< setfill(' ')
					<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

				String sub = _info.substr(0, ConsoleWidth);

				DevLogFile << sub << endl;

				WriteTo_Buffer
				(
					linePos - 1,
					ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity)
				);

				DevLogStream.str(String());

				if (_info.length() < ConsoleWidth)
				{
					_info.erase(0, _info.length());
				}
				else
				{
					_info.erase(0, ConsoleWidth);
				}

				pos += ConsoleWidth + 1;
				endPos += ConsoleWidth;
			}
		}
		else
		{
			DevLogStream
				<< dateSig.str()
				<< _info
				<< setfill(' ')
				<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

			DevLogFile << DevLogStream.str() << endl;

			WriteTo_Buffer
			(
				linePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity)
			);

			DevLogStream.str(String());
		}

		DevLogStream
			<< setfill('-')
			<< setw(ConsoleWidth - DevLogStream.str().size()) << '-';

		WriteTo_Buffer(linePos++, ConslAttribFlags(EConslAttribFlag::Background_Red, EConslAttribFlag::Background_Intensity));
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

	void Load_DevConsole()
	{
		Load__DevLogFileStream();

		ConsoleOutput = OSAL::Console_GetHandle(EConsoleHandle::Output);
		ConsoleInput  = OSAL::Console_GetHandle(EConsoleHandle::Input );

		OSAL::Console_SetTitle(ConsoleTitle);

		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);
		OSAL::Console_SetSize      (ConsoleOutput, ConsoleSize      );
		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);

		cout << "Dev: Console buffers loaded" << endl;
	
		Load_DevLogStream();

		cout << "Dev: DevLogStream loaded";

		WriteToConsole(ConsoleOutput, &ConsoleCharBuffer[0][0], ConsoleCharBufferSize, ConsoleCharPos, &ConsoleWriteArea);

		SetConsoleCursorPosition (ConsoleOutput, {0, 140});
		
		Console_UpdateBuffer();

		CLog("Dev: Console Submodules ready");

		CLog("This is a test of a clog string that is larger than the line size that could traditionally not be taken in the previous version... Its long enough that the lines in console should be around 2...");

		Console_UpdateBuffer();
	}


	// Private

	void Load__DevLogFileStream()
	{
		StringStream dateStream;

		auto dateSnapshot = OSAL::GetExecutionStartDate();

		dateStream << put_time(&dateSnapshot, "%F_%I-%M-%S_%p");

		Path path = DevLogPath;

		if (!CheckPathExists(path))
		{
			Create_Directories(path);
		}
		
		using namespace IO;

		bool openResult = OpenFile
		(
			DevLogFile,
			OpenFlags(EOpenFlag::ForOutput),
			Path(String(DevLogPath) + String("/") + String(DevLogName) + String("__") + dateStream.str() + String(".txt"))
		);

		if (!openResult)
		{
			throw RuntimeError("Failed to create a dev log file...");
		}
	}

	void Load_DevLogStream()
	{
		Load_DevLogStream_Title();
		Load_DevLogStream_FileStream();
		Load_DevLogStream_Status();
	}

	void Load_DevLogStream_FileStream()
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

	void Load_DevLogStream_Status()
	{
		DevLogStream.str(std::string());

		DevLogStream << setfill('=') << setw(ConsoleWidth) << '=';

		WriteTo_Buffer
		(
			StatusStart,
			ConslAttribFlags
			(
			EConslAttribFlag::Foreground_Red,
			EConslAttribFlag::Foreground_Green,
			EConslAttribFlag::Foreground_Blue,
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
				EConslAttribFlag::Foreground_Red,
				EConslAttribFlag::Foreground_Green,
				EConslAttribFlag::Foreground_Blue
			)
			);
		}
	}
		
	void Load_DevLogStream_Title()
	{
		using namespace Meta;

		DevLogStream.str(std::string());

		DevLogStream << setfill('-') << setw(ConsoleWidth) << '-';

		DevLogFile << setfill('-') << setw(ConsoleWidth) << '-' << endl;

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

		DevLogStream 
			<< "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << "    "
			
			<< "Dev Log: "
			<< put_time(&OSAL::GetExecutionStartDate(), "%F %I:%M:%S %p")

			<< setfill(' ') << setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

		DevLogFile 
			<< "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << "    "

			<< "Dev Log: " << put_time(&OSAL::GetExecutionStartDate(), "%F %I:%M:%S %p") 

			<< setfill(' ') << setw(ConsoleWidth - DevLogStream.str().size()) << ' ' << endl; 

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

		DevLogFile << setfill('-') << setw(ConsoleWidth) << '-' << endl;

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