#pragma once



#include "LAL.hpp"



namespace Dev
{
	using namespace LAL;


	constexpr uInt16 ConsoleWidth = 120;
	constexpr uInt16 ConsoleHeight = 50;

	using CharLine = char[ConsoleWidth];

	void CLog(const CharLine _lineToLog);

	void Load_DevConsole();
}
