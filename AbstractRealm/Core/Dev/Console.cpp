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

	using OSAL::ConsoleChar;



	//StaticData
	//(
		// Windows uses short instead of unsigned short for rect...
		sInt16 ConsoleWidth  = 160;   //140;
		sInt16 ConsoleHeight = 118;   //50 ;

		using CharBuffer = ConsoleChar[1024][1024];

		OS_Handle ConsoleOutput;
		OS_Handle ConsoleInput ;

		ConsoleRect ConsoleSize =
		{
			0, 0,
			sInt16(ConsoleWidth  - 1),   // Width
			sInt16(ConsoleHeight - 1)    // Height	
		};

		ConsoleExtent ConsoleBufferSize = { ConsoleWidth, ConsoleHeight };

		OS_RoCStr ConsoleTitle = L"Abstract Realm: Dev Console";

		StaticArray<OS_Handle, 2> ConsoleBuffers;

		DynamicArray<ConsoleChar> ConsoleCharBuffer(uInt32(ConsoleHeight) * uInt32(ConsoleWidth));

		ConsoleExtent ConsoleCharBufferSize = { ConsoleWidth, ConsoleHeight };
		ConsoleExtent ConsoleCharPos        = { 0           , 0             };

		 //Just use console size.
		ConsoleRect ConsoleWriteArea =
		{
			0, 0,
			sInt16(ConsoleWidth  - 1),   // Width
			sInt16(ConsoleHeight - 1)    // Height	
		};

		OS_Handle FrontBuffer = ConsoleBuffers[0],
			      BackBuffer  = ConsoleBuffers[1] ;

		uInt16 DevLogLineEnd = ConsoleHeight - 6;
		uInt16 StatusStart   = ConsoleHeight - 5;

		uInt16 StatusColumnWidth = ConsoleWidth / 4;

		StaticArray<StaticArray<StringStream, 4>, 4> StatusStreams;

		StringStream DevLogStream;

		File_OutputStream DevLogFileOut;
		File_InputStream  DevLogFileIn ;
	//)



	// Forwards
	void Load__DevLog_IOFileStream();
	void Load_DevLogStream        ();
	void Load_DevLogStream_LogFeed();
	void Load_DevLogStream_Status ();
	void Load_DevLogStream_Title  ();

	void WriteTo_Buffer      (int _line,           ConslAttribFlags _flags);
	void WriteTo_StatusModule(int _row , int _col, ConslAttribFlags _flags);



	// Public

	void ClearBuffer()
	{
		for (uInt16 y = 0; y < ConsoleHeight; ++y)
		{
			for (uInt16 x = 0; x < ConsoleWidth; ++x)
			{
				ConsoleCharBuffer[y * ConsoleWidth  + x].Char.UnicodeChar = ' ';
				ConsoleCharBuffer[y * ConsoleHeight + x].Attributes       = 0  ;
			}
		}

		//memcpy()
	}

	void CLog(String _info)
	{
		if (!Meta::UseDebug) return;

		static uInt16 linePos = 3;

		if (linePos == StatusStart + 1 )
		{
			linePos = 3;
		}

		DevLogStream.str(String());

		CalendarDate dateSnapshot = OSAL::GetTime_Local();

		StringStream dateSig; dateSig << "[" << put_time(&dateSnapshot, "%F %I:%M:%S %p") << "] ";

		WordSize lineLength = dateSig.str().size() + _info.size();

		if (lineLength > ConsoleWidth)
		{
			DevLogStream 
				<< dateSig.str()
				<< setfill(' ')
				<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

			DevLogFileOut << DevLogStream.str() << endl;

			WriteTo_Buffer
			(
				linePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Green, EConslAttribFlag::Foreground_Blue)
			);

			DevLogStream.str(String());

			WordSize pos = 0, endPos = ConsoleWidth;

			while (!_info.empty())
			{
				linePos ++;

				if (linePos == StatusStart + 1)
				{
					linePos = 3;
				}

				DevLogStream 
					<< _info.substr(0, ConsoleWidth)
					<< setfill(' ')
					<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

				String sub = _info.substr(0, ConsoleWidth);

				DevLogFileOut << sub << endl;

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

				pos    += ConsoleWidth + 1;
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

			DevLogFileOut << DevLogStream.str() << endl;

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

		StringStream dateSig; dateSig << "[" << put_time(&dateSnapshot, "%F %I:%M:%S %p") << "] Error:  ";

		WordSize lineLength = dateSig.str().size() + _info.size();

		if (lineLength > ConsoleWidth)
		{
			DevLogStream
				<< dateSig.str()
				<< setfill(' ')
				<< setw(ConsoleWidth - DevLogStream.str().size()) << ' ';

			DevLogFileOut << DevLogStream.str() << endl;

			WriteTo_Buffer
			(
				linePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity)
			);

			DevLogStream.str(String());

			WordSize pos = 0, endPos = ConsoleWidth;

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

				DevLogFileOut << sub << endl;

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

			DevLogFileOut << DevLogStream.str() << endl;

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

	// Offload to OSAL...
	VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
	{
	#ifndef MOUSE_HWHEELED
	#define MOUSE_HWHEELED 0x0008
	#endif
		printf("Mouse event: ");

		switch (mer.dwEventFlags)
		{
			case MOUSE_WHEELED:
			{
				if (mer.dwButtonState > 0)
				{
					CLog("Mouse Up");
				}
				else
				{
					CLog("MouseDown");
				}

				break;
			}
		}
	}

	void BufferEvent(WINDOW_BUFFER_SIZE_RECORD wbsr)
	{
		ConsoleWidth  = wbsr.dwSize.X;
		ConsoleHeight = wbsr.dwSize.Y;

		ConsoleSize.Right  = ConsoleWidth  - 1;
		ConsoleSize.Bottom = ConsoleHeight - 1;

		ConsoleBufferSize.X = ConsoleWidth ;
		ConsoleBufferSize.Y = ConsoleHeight;

		DevLogLineEnd = ConsoleHeight - 6;
		StatusStart   = ConsoleHeight - 5;

		StatusColumnWidth = ConsoleWidth / 4;

		/*SMALL_RECT screen = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(ConsoleOutput, true, &screen);*/

		//OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);

		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);
		OSAL::Console_SetSize      (ConsoleOutput, ConsoleSize      );

		ConsoleCharBuffer.resize(ConsoleHeight * ConsoleWidth);

		ClearBuffer();
		
		Load_DevLogStream();
	}

	// Offload windows stuff to OSAL...
	void Console_UpdateInput()
	{
		static DWORD cNumRead, fdwMode, i; 
		static INPUT_RECORD irInBuf[1]; 

		// Wait for the events. 

		if 
		(
			PeekConsoleInput 
			(
				ConsoleInput,      // input buffer handle 
				irInBuf,     // buffer to read into 
				1,         // size of read buffer 
				&cNumRead
			) && cNumRead > 0
		)
		{
			ReadConsoleInput
			(
				ConsoleInput,      // input buffer handle 
				irInBuf,     // buffer to read into 
				1,         // size of read buffer 
				&cNumRead
			);

			bool takeInput = true;

			// Dispatch the events to the appropriate handler. 

			for (i = 0; i < cNumRead; i++)
			{
				switch (irInBuf[i].EventType)
				{
					case KEY_EVENT: // keyboard input 
					{
						//KeyEventProc(irInBuf[i].Event.KeyEvent);

						break;
					}
					case MOUSE_EVENT: // mouse input 
					{
						MouseEventProc(irInBuf[i].Event.MouseEvent);

						break;
					}
					case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
					{
						auto WBSE = irInBuf[i].Event.WindowBufferSizeEvent;

						CLog("BufferEvent Y: " + ToString(WBSE.dwSize.Y));

						if (WBSE.dwSize.Y != ConsoleHeight || WBSE.dwSize.X != ConsoleWidth)
						{
							CLog("Lead to something...");

							BufferEvent(WBSE);


							Console_UpdateBuffer();
						}

						break;
					}
					case FOCUS_EVENT:  // disregard focus events 
					default:
						//ErrorExit("Unknown event type");
						break;
				}
			}				
		}		
	}

	void Console_UpdateBuffer()
	{
		// Read from File up to buffer allows

		// Status Update
		for  (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				String str = StatusStreams[y][x].str();

				for (int index = 0; index < str.size(); index++)
				{
					int col = index + StatusColumnWidth * x;

					/*ConsoleCharBuffer.data()[y + StatusStart + 1 ][col].Char.UnicodeChar = str.at(index);

					ConsoleCharBuffer[y + StatusStart + 1 ][col].Attributes = 
						ConslAttribFlags
						(
							EConslAttribFlag::Foreground_Red      , 
							EConslAttribFlag::Foreground_Green    , 
							EConslAttribFlag::Foreground_Blue     , 
							EConslAttribFlag::Foreground_Intensity
						);*/

					ConsoleCharBuffer[(y + StatusStart + 1) * ConsoleWidth + col].Char.UnicodeChar = str.at(index);

					ConsoleCharBuffer[(y + StatusStart + 1) * ConsoleWidth + col].Attributes =
						ConslAttribFlags
						(
						EConslAttribFlag::Foreground_Red,
						EConslAttribFlag::Foreground_Green,
						EConslAttribFlag::Foreground_Blue,
						EConslAttribFlag::Foreground_Intensity
						);
				}
			}
		}

		WriteToConsole(ConsoleOutput, &ConsoleCharBuffer[0], ConsoleCharBufferSize, ConsoleCharPos, &ConsoleSize);
	}

	void Load_DevConsole()
	{
		Load__DevLog_IOFileStream();

		ConsoleOutput = OSAL::Console_GetHandle(EConsoleHandle::Output);
		ConsoleInput  = OSAL::Console_GetHandle(EConsoleHandle::Input );

		OSAL::Console_SetTitle(ConsoleTitle);

		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);
		OSAL::Console_SetSize      (ConsoleOutput, ConsoleSize      );

		ConsoleCharBuffer.resize(ConsoleHeight * ConsoleWidth);

		cout << "Dev: Console buffers loaded" << endl;
	
		Load_DevLogStream();

		cout << "Dev: DevLogStream loaded";

		WriteToConsole(ConsoleOutput, &ConsoleCharBuffer[0], ConsoleCharBufferSize, ConsoleCharPos, &ConsoleSize);

		auto error = GetLastError();

		SetConsoleCursorPosition (ConsoleOutput, {0, sInt16(ConsoleBufferSize.Y + 1)});
		
		Console_UpdateBuffer();

		CLog("Dev: Console / Logging ready");

		Console_UpdateBuffer();
	}

	void Unload_DevConsole()
	{
		CLog("Unloading dev console (there will be no logs after this)");

		DevLogFileOut.close();
		DevLogFileIn .close();
	}


	// Private

	void Load__DevLog_IOFileStream()
	{
		StringStream dateStream;

		auto dateSnapshot = OSAL::GetExecutionStartDate();

		dateStream << put_time(&dateSnapshot, "%F_%I-%M-%S_%p");

		Path path = DevLogPath;

		if (!CheckPathExists(path))
		{
			Create_Directories(path);
		}
		
		using namespace Core::IO;

		bool openResult = OpenFile
		(
			DevLogFileOut,
			OpenFlags(EOpenFlag::ForOutput),
			Path(String(DevLogPath) + String("/") + String(DevLogName) + String("__") + dateStream.str() + String(".txt"))
		);

		if (!openResult)
		{
			throw RuntimeError("Failed to create a dev log file...");
		}

		openResult = OpenFile
		(
			DevLogFileIn,
			OpenFlags(EOpenFlag::ForInput),
			Path(String(DevLogPath) + String("/") + String(DevLogName) + String("__") + dateStream.str() + String(".txt"))
		);

		if (!openResult)
		{
			throw RuntimeError("Failed to open a dev log file...");
		}
	}

	void Load_DevLogStream()
	{
		Load_DevLogStream_Title();
		Load_DevLogStream_LogFeed();
		Load_DevLogStream_Status();
	}

	void Load_DevLogStream_LogFeed()
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

		DevLogFileOut << setfill('-') << setw(ConsoleWidth) << '-' << endl;

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

		DevLogFileOut 
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

		DevLogFileOut << setfill('-') << setw(ConsoleWidth) << '-' << endl;

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
			/*ConsoleCharBuffer[_line][index].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[_line][index].Attributes       = _flags       ;*/

			ConsoleCharBuffer[_line * uInt32(ConsoleWidth) + index].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[_line * uInt32(ConsoleWidth) + index].Attributes       = _flags       ;
		}
	}

	void WriteTo_StatusModule(int _row, int _col, ConslAttribFlags _flags)
	{
		auto str = StatusStreams[_row][_col].str();

		_row = StatusStart + _row + 1;

		for (int index = 0; index < str.size(); index++)
		{
			int col = index + StatusColumnWidth * _col;

			/*ConsoleCharBuffer[_row][col].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[_row][col].Attributes       = WORD(_flags);*/

			ConsoleCharBuffer[_row * ConsoleWidth + col].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[_row * ConsoleWidth + col].Attributes = WORD(_flags); 
		}
	}
}