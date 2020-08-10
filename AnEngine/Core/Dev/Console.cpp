// Parent Header
#include "Console.hpp"


#include "LAL.hpp"

#include "Meta/EngineInfo.hpp"

#include "OSAL/OSAL_Console.hpp"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "PDCurses/curses.h"



namespace Dev
{
	using namespace LAL;
	using namespace OSAL;

	// Static Data

	// Private

	WINDOW* pls;
	
	void Load_DevConsole()
	{
		pls = initscr();



	}



	






































	// Old attempt...

	//OS_Handle ConsoleOutput;
	//OS_Handle ConsoleInput ;

	//constexpr uInt16 ConsoleWidth  = 140;
	//constexpr uInt16 ConsoleHeight = 90 ;

	//ConsoleRect ConsoleSize = 
	//{
	//	0, 0,
	//	ConsoleWidth -1,   // Width
	//	ConsoleHeight-1     // Height	
	//};

	//ConsoleExtent ConsoleBufferSize = { ConsoleWidth, ConsoleHeight };


	//OS_RoCStr ConsoleTitle = L"Abstract Realm: Dev Console";

	//StaticArray<OS_Handle, 2> ConsoleBuffers;

	//using CharBuffer = CharU[ConsoleHeight * ConsoleWidth];

	//using CharLine = CharU[ConsoleWidth];

	//CharBuffer    ConsoleCharBuffer; 
	//ConsoleExtent ConsoleCharBufferSize = { ConsoleWidth, ConsoleHeight };
	//ConsoleExtent ConsoleCharPos   = { 0, 0 };

	//ConsoleRect ConsoleWriteArea =
	//{
	//	0, 0,
	//	ConsoleWidth - 1,   // Width
	//	ConsoleHeight - 1     // Height	
	//};

	//OS_Handle FrontBuffer = ConsoleBuffers[0],
	//	      BackBuffer  = ConsoleBuffers[1] ;

	//ConsoleRect Console_EngineTitle
	//{
	//	0, 0, 
	//	ConsoleWidth -1, 
	//	3
	//};


	//// Public functions

	//void ClearBuffer()
	//{
	//	for (uInt16 y = 0; y < ConsoleHeight; ++y)
	//	{
	//		for (uInt16 x = 0; x < ConsoleWidth; ++x)
	//		{
	//			ConsoleCharBuffer[x + ConsoleWidth * y].Char.UnicodeChar = ' ';
	//			ConsoleCharBuffer[x + ConsoleWidth * y].Attributes       = 0;
	//		}
	//	}
	//}

	//void SwitchBuffers()
	//{
	//	static bool toggle = true;

	//	if (toggle)
	//	{
	//		FrontBuffer = ConsoleBuffers[1];
	//		BackBuffer  = ConsoleBuffers[0];

	//		toggle = false;
	//	}
	//	else
	//	{
	//		FrontBuffer = ConsoleBuffers[0];
	//		BackBuffer  = ConsoleBuffers[1];

	//		toggle = true;
	//	}
	//}

	//void Load_DevConsole()
	//{
	//	cout << "Dev: Load Console Buffer" << endl;

	//	ConsoleOutput = OSAL::Console_GetHandle(EConsoleHandle::Output);
	//	ConsoleInput  = OSAL::Console_GetHandle(EConsoleHandle::Input );

	//	OSAL::Console_SetTitle(ConsoleTitle);

	//	OSAL::Console_SetBufferSize(ConsoleOutput, ConsoleBufferSize);
	//	OSAL::Console_SetSize      (ConsoleOutput, ConsoleSize);

	//	for (auto buffer : ConsoleBuffers)
	//	{
	//		buffer = OSAL::Console_CreateBuffer();

	//		if (buffer == INVALID_HANDLE_VALUE)
	//		{
	//			cerr << "Dev: Error buffer creation failed." << endl;
	//		}

	//		OSAL::Console_SetBufferSize(buffer, ConsoleBufferSize);
	//		OSAL::Console_SetSize      (buffer, ConsoleSize      );
	//	}

	//	cout << "Dev: Console buffers loaded" << endl;

	//	ClearBuffer();

	//	WriteConsoleOutputA(ConsoleOutput, ConsoleCharBuffer, ConsoleCharBufferSize, ConsoleCharPos, &ConsoleWriteArea);

	//	SetConsoleCursorPosition (ConsoleOutput, {0, 0});

	//	Console_PrintEngineTitle();

	//	cout << "Dev: First clear complete" << endl;
	//}

	//void WriteToBuffer(CharBuffer _lines, SHORT _count, SHORT _start)
	//{
	//	ConsoleRect pos = 
	//	{ 0, _start, 
	//		ConsoleWidth -1, _count + _start
	//	};

	//	WriteConsoleOutputA(ConsoleOutput, _lines, { ConsoleWidth-1, _count }, { 0, _start }, &pos);
	//}

	//void Console_PrintEngineTitle()
	//{
	//	using namespace Meta;

	//	StaticArray<String, 3> lines;

	//	CharU buffer[ConsoleWidth * 3];

	//	lines[0].append(ConsoleWidth - 1, '-');

	//	lines[1] = "Abstract Realm: MVP Build - ";
	//	lines[1] += std::to_string(EEngineVersion::Major) + '.' + std::to_string(EEngineVersion::Minor) + '.' + std::to_string(EEngineVersion::Patch) + ' ';

	//	int linesize = lines[1].size();

	//	int leftover = ConsoleWidth - linesize;

	//	lines[1].append(ConsoleWidth - linesize, '-');

	//	lines[2].append(ConsoleWidth, '-');

	//	int indexY = 0;

	//	for (auto line : lines)
	//	{
	//		const char* array = line.c_str();

	//		for (uInt16 index = 0; index < ConsoleWidth; index++)
	//		{
	//			buffer[index + ConsoleWidth * indexY].Char.UnicodeChar = array[index];
	//			buffer[index + ConsoleWidth * indexY].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	//		}

	//		indexY++;
	//	}

	//	WriteToBuffer(buffer, 3, 0);

	//	/*cout << std::setfill('-') << std::setw(140); cout << " " << endl;

	//	cout << "Abstract Realm: MVP Build - "
	//		<< EEngineVersion::Major << "."
	//		<< EEngineVersion::Minor << "."
	//		<< EEngineVersion::Patch << endl;

	//	cout << std::setfill('-') << std::setw(140); cout << " " << endl << endl;*/
	//}
}