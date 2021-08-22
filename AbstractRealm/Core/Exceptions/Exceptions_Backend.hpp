#pragma once


#include "LAL.hpp"
#include "Dev/Dev.hpp"


namespace Exception
{
	using namespace LAL;

	Dev_Declare_LogAPI();

	void Load();
	void Unload();
}
