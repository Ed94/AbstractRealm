// Parent
#include "Exceptions_Backend.hpp"


namespace Exception
{
	Dev_Declare_Log(Exception);

	void Load()
	{
		SubLogger.Init();
	}

	void Unload();
}