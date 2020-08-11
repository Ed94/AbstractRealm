#pragma once



#include "LAL.hpp"



namespace Dev
{
	using namespace LAL;


	constexpr uInt16 ConsoleWidth = 140;
	constexpr uInt16 ConsoleHeight = 50;

	using CharLine = char[ConsoleWidth];

	void CLog(String _lineToLog);

	void Load_DevConsole();

	void SetConsole_IOHooks();
}
