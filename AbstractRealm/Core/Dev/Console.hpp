#pragma once



#include "LAL.hpp"



namespace Dev
{
	// Usings

	using namespace LAL;



	// Compile-Time

	constexpr RoCStr DevLogPath = "Engine/Dev/Logs";
	constexpr RoCStr DevLogName = "DevLog"         ;



	// Functions

	/*class CLog
	{
	public:

	protected:
	}*/;

	void CLog       (String _info                    );
	void CLog_Error (String _info                    );
	void CLog_Status(String _info, int _row, int _col);

	void Console_UpdateInput();
	void Console_UpdateBuffer();

	void Load_DevConsole();
	void Unload_DevConsole();

	void Console_EnableAutoUpdate();
	void Console_DisableAutoUpdate();
}
