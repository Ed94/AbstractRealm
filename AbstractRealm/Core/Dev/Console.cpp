// Parent Header
#include "Console.hpp"

// PAL
#include "OSAL_Console.hpp"
#include "OSAL_Timing.hpp"
#include "TPAL/TPAL_Imgui.hpp"
//Core
#include "IO/Basic_FileIO.hpp"


namespace Dev
{
	using namespace OSAL;

	using OSAL::ConsoleChar;

	void Load_DevLogStream();
	void Load_DevLogStream_LogFeed();
	void Load_DevLogStream_Status();
	void Load_DevLogStream_Title();

	void WriteTo_Buffer(int _line, ConslAttribFlags _flags);
	void WriteTo_StatusModule(int _row, int _col, ConslAttribFlags _flags);


#pragma region StaticData
	// Windows uses short instead of unsigned short for rect...
	s16 ConsoleWidth  = 160;   //140;
	s16 ConsoleHeight = 118;   //50 ;

	using CharBuffer = ConsoleChar[1024][1024];

	OS_Handle ConsoleOutput;
	OS_Handle ConsoleInput ;

	ConsoleRect ConsoleSize =
	{
		0, 0,
		s16(ConsoleWidth  - 1),   // Width
		s16(ConsoleHeight - 1)    // Height	
	};

	ConsoleExtent ConsoleBufferSize = { ConsoleWidth, ConsoleHeight };

	OS_RoCStr ConsoleTitle = L"Abstract Realm: Dev Console";

	StaticArray<OS_Handle, 2> ConsoleBuffers;

	DynamicArray<ConsoleChar> ConsoleCharBuffer(u32(ConsoleHeight) * u32(ConsoleWidth));

	ConsoleExtent ConsoleCharBufferSize = { ConsoleWidth, ConsoleHeight };
	ConsoleExtent ConsoleCharPos        = { 0           , 0             };

	 //Just use console size.
	ConsoleRect ConsoleWriteArea =
	{
		0, 0,
		s16(ConsoleWidth  - 1),   // Width
		s16(ConsoleHeight - 1)    // Height	
	};

	OS_Handle FrontBuffer = ConsoleBuffers[0],
			  BackBuffer  = ConsoleBuffers[1] ;

	u16 DevLogLineEnd = ConsoleHeight - 6;
	u16 StatusStart   = ConsoleHeight - 5;

	u16 StatusColumnWidth = ConsoleWidth / 4;

	StaticArray<StaticArray<StringStream, 4>, 4> StatusStreams;

	StringStream DevLogStream;

	bool AutoUpdateConsole = false;
#pragma endregion StaticData


#pragma region Public

	void Console_Record_EditorDevDebugUI()
	{
		using namespace TPAL::Imgui; 

		#define Args(_Entry) NameOf(_Entry).str(), _Entry

		if (TreeNode("Console"))
		{
			if (Table2C::Record())
			{
				Table2C::Entry("Console Title", "Abstract Realm: Dev Console");

				Table2C::Entry("Console Size", ToString(ConsoleWidth) + String("x") + ToString(ConsoleHeight));

				Table2C::Entry(Args(AutoUpdateConsole));

				Table2C::Entry(Args(DevLogLineEnd));
				Table2C::Entry(Args(StatusStart));
				Table2C::Entry(Args(StatusColumnWidth));

				StringStream dateStream;

				auto dateSnapshot = OSAL::GetExecutionStartDate();

				dateStream << PutTime(dateSnapshot, "%F_%I-%M-%S_%p");

				String logstr = String(DevLogPath) + String("/") + String(DevLogName) + String("__") + dateStream.str() + String(".txt");

				Table2C::Entry("Log File", logstr);

				Table2C::EndRecord();
			}

			TreePop();
		}

		#undef Args
	}

	void ClearBuffer()
	{
		for (u16 y = 0; y < ConsoleHeight; ++y)
		{
			for (u16 x = 0; x < ConsoleWidth; ++x)
			{
				ConsoleCharBuffer[y * ConsoleWidth  + x].Char.UnicodeChar = ' ';
				ConsoleCharBuffer[y * ConsoleHeight + x].Attributes       = 0  ;
			}
		}
	}

	void CLog(String _info)
	{
		if (! Meta::UseDebug()) 
			return;

		static u16 _LinePos = 3;

		if (_LinePos == StatusStart + 1 )
		{
			_LinePos = 3;
		}

		DevLogStream.str(String());

		CalendarDate dateSnapshot = OSAL::GetTime_Local();

		StringStream dateSig; dateSig << "[" << PutTime(dateSnapshot, "%F %I:%M:%S %p") << "] ";

		uDM lineLength = dateSig.str().size() + _info.size();

		if (lineLength > uDM(ConsoleWidth))
		{
			DevLogStream 
				<< dateSig.str()
				<< SetFill(' ')
				<< SetWidth(ConsoleWidth - DevLogStream.str().size()) << ' ';

			WriteTo_Buffer
			(
				_LinePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Green, EConslAttribFlag::Foreground_Blue)
			);

			DevLogStream.str(String());

			uDM pos = 0, endPos = ConsoleWidth;

			while (! _info.empty())
			{
				_LinePos ++;

				if (_LinePos == StatusStart + 1)
				{
					_LinePos = 3;
				}

				DevLogStream 
					<< _info.substr(0, ConsoleWidth)
					<< SetFill(' ')
					<< SetWidth(ConsoleWidth - DevLogStream.str().size()) << ' ';

				String sub = _info.substr(0, ConsoleWidth);

	/*			DevLogFileOut << sub << endl;*/

				WriteTo_Buffer
				(
					_LinePos - 1,
					ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Green, EConslAttribFlag::Foreground_Blue)
				);

				DevLogStream.str(String());

				if (_info.length() < uDM(ConsoleWidth))
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
				<< SetFill(' ')
				<< SetWidth(ConsoleWidth - DevLogStream.str().size()) << ' ';

			WriteTo_Buffer
			(
				_LinePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Green, EConslAttribFlag::Foreground_Blue)
			);

			DevLogStream.str(String());
		}	

		DevLogStream
			<< SetFill('-')
			<< SetWidth(ConsoleWidth - DevLogStream.str().size()) << '-';

		WriteTo_Buffer(_LinePos++, ConslAttribFlags(EConslAttribFlag::Background_Intensity));

		if (AutoUpdateConsole) 
			Console_UpdateBuffer();
	}

	void CLog_Error(String _info)
	{
		if (! Meta::UseDebug()) 
			return;

		static u16 linePos = 4;

		if (linePos == StatusStart + 1)
		{
			linePos = 4;
		}

		DevLogStream.str(String());

		CalendarDate dateSnapshot = OSAL::GetTime_Local();

		StringStream dateSig; dateSig << "[" << PutTime(dateSnapshot, "%F %I:%M:%S %p") << "] Error:  ";

		uDM lineLength = dateSig.str().size() + _info.size();

		if (lineLength > uDM(ConsoleWidth))
		{
			DevLogStream
				<< dateSig.str()
				<< SetFill(' ')
				<< SetWidth(ConsoleWidth - DevLogStream.str().size()) << ' ';

			WriteTo_Buffer
			(
				linePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity)
			);

			DevLogStream.str(String());

			uDM pos = 0, endPos = ConsoleWidth;

			while (!_info.empty())
			{
				linePos++;

				if (linePos == StatusStart + 1)
				{
					linePos = 4;
				}

				DevLogStream
					<< _info.substr(0, ConsoleWidth)
					<< SetFill(' ')
					<< SetWidth(ConsoleWidth - DevLogStream.str().size()) << ' ';

				String sub = _info.substr(0, ConsoleWidth);

				WriteTo_Buffer
				(
					linePos - 1,
					ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity)
				);

				DevLogStream.str(String());

				if (_info.length() < uDM(ConsoleWidth))
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
				<< SetFill(' ')
				<< SetWidth(ConsoleWidth - DevLogStream.str().size()) << ' ';

			WriteTo_Buffer
			(
				linePos - 1,
				ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity)
			);

			DevLogStream.str(String());
		}

		DevLogStream
			<< SetFill('-')
			<< SetWidth(ConsoleWidth - DevLogStream.str().size()) << '-';

		WriteTo_Buffer(linePos++, ConslAttribFlags(EConslAttribFlag::Background_Red, EConslAttribFlag::Background_Intensity));

		if (AutoUpdateConsole) 
			Console_UpdateBuffer();
	}

	void CLog_Status(String _info, int _row, int _col)
	{
		if (Meta::UseDebug())
		{
			StatusStreams[_row][_col].str(String());

			StatusStreams[_row][_col]
				<< _info
				<< SetFill(' ')
				<< SetWidth(StatusColumnWidth - StatusStreams[_row][_col].str().size()) << ' ';
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

		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);
		OSAL::Console_SetSize      (ConsoleOutput, ConsoleSize      );

		ConsoleCharBuffer.resize(ConsoleHeight * ConsoleWidth);

		ClearBuffer();
		
		Load_DevLogStream();
	}

	void Console_EnableAutoUpdate()
	{
		AutoUpdateConsole = true;
	}

	void Console_DisableAutoUpdate()
	{
		AutoUpdateConsole = false;
	}

	// Offload windows stuff to OSAL...
	void Console_UpdateInput()
	{
		static DWORD 
			cNumRead, 
			//fdwMode,
			i; 
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

			//bool takeInput = true;

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
		for  (uDM y = 0; y < 4; y++)
		{
			for (uDM x = 0; x < 4; x++)
			{
				String str = StatusStreams[y][x].str();

				for (uDM index = 0; index < str.size(); index++)
				{
					uDM col = index + StatusColumnWidth * x;

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

		WriteToConsole(ConsoleOutput, getPtr(ConsoleCharBuffer[0]), ConsoleCharBufferSize, ConsoleCharPos, ConsoleSize);
	}

	void Load_DevConsole()
	{
		ConsoleOutput = OSAL::Console_GetHandle(EConsoleHandle::Output);
		ConsoleInput  = OSAL::Console_GetHandle(EConsoleHandle::Input );

		OSAL::Console_SetTitle(ConsoleTitle);

		OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);
		OSAL::Console_SetSize      (ConsoleOutput, ConsoleSize      );

		ConsoleCharBuffer.resize(ConsoleHeight * ConsoleWidth);

		// Set the floating-point precision to 10 places.
		cout.precision(10);

		cout << "Dev: Console buffers loaded" << endl;
	
		Load_DevLogStream();

		cout << "Dev: DevLogStream loaded";

		WriteToConsole(ConsoleOutput, getPtr(ConsoleCharBuffer[0]), ConsoleCharBufferSize, ConsoleCharPos, ConsoleSize);

		//auto error = GetLastError();

		SetConsoleCursorPosition (ConsoleOutput, {0, s16(ConsoleBufferSize.Y + 1)});
		
		Console_UpdateBuffer();

		Log("Dev: Console / Logging ready");

		Console_UpdateBuffer();
	}

	void Unload_DevConsole()
	{
		Log("Unloading dev console (there will be no logs after this)");
	}

#pragma endregion Public


#pragma region Private

	void Load_DevLogStream()
	{
		Load_DevLogStream_Title();
		Load_DevLogStream_LogFeed();
		Load_DevLogStream_Status();
	}

	void Load_DevLogStream_LogFeed()
	{
		for (u16 line = 3; line < DevLogLineEnd; line++)
		{
			DevLogStream.str(String());

			DevLogStream << SetWidth(ConsoleWidth) << SetFill(' ') << ' ';

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
		DevLogStream.str(String());

		DevLogStream << SetFill('=') << SetWidth(ConsoleWidth) << '=';

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

		DevLogStream.str(String());

		DevLogStream << "Status: Not Setup.";

		WriteTo_Buffer(StatusStart + 1, ConslAttribFlags(EConslAttribFlag::Foreground_Red, EConslAttribFlag::Foreground_Intensity));

		for (u16 line = StatusStart + 2; line < ConsoleHeight; line++)
		{
			DevLogStream.str(String());

			DevLogStream << SetFill(' ') << SetWidth(ConsoleWidth) << ' ';

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

		DevLogStream.str(String());

		DevLogStream << SetFill('-') << SetWidth(ConsoleWidth) << '-';

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

		DevLogStream.str(String());

		DevLogStream 
			<< "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << "    "
			
			<< "Dev Log: "
			<< PutTime(OSAL::GetExecutionStartDate(), "%F %I:%M:%S %p")

			<< SetFill(' ') << SetWidth(ConsoleWidth - DevLogStream.str().size()) << ' ';

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

		DevLogStream << SetFill('-') << SetWidth(ConsoleWidth) << '-';

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

		for (uDM index = 0; index < str.size(); index++)
		{
			ConsoleCharBuffer[_line * u32(ConsoleWidth) + index].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[_line * u32(ConsoleWidth) + index].Attributes       = _flags;
		}
	}

	void WriteTo_StatusModule(int _row, int _col, ConslAttribFlags _flags)
	{
		auto str = StatusStreams[_row][_col].str();

		_row = StatusStart + _row + 1;

		for (uDM index = 0; index < str.size(); index++)
		{
			uDM col = index + StatusColumnWidth * _col;

			ConsoleCharBuffer[_row * ConsoleWidth + col].Char.UnicodeChar = str.at(index);
			ConsoleCharBuffer[_row * ConsoleWidth + col].Attributes       = WORD(_flags); 
		}
	}

#pragma endregion Private
}