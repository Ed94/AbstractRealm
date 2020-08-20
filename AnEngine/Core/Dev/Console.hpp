#pragma once



#include "LAL.hpp"



namespace Dev
{
	// Usings

	using namespace LAL;



	// Compile-Time

	constexpr uInt16 ConsoleWidth  = 140;
	constexpr uInt16 ConsoleHeight = 50;

	constexpr RoCStr DevLogPath = "Engine/Dev/Logs";
	constexpr RoCStr DevLogName = "DevLog"         ;



	// Functions

	void CLog       (String _info                    );
	void CLog_Error (String _info                    );
	void CLog_Status(String _info, int _row, int _col);

	void Console_UpdateBuffer();

	void Load_DevConsole();
}
