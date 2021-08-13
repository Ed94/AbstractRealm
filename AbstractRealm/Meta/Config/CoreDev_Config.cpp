#include "CoreDev_Config.hpp"


namespace Meta
{
	namespace StaticData
	{
		bool Enable_HeapTracking = false;

		bool ShouldLogToFile = true;

		ELogToFileMode LogToFile_Mode = ELogToFileMode::Full;

		bool ShouldDump_EngineStateJson_OnCrash = false;
	}

	
	bool Enable_HeapTracking()
	{
		return StaticData::Enable_HeapTracking;
	}

	bool ShouldLogToFile()
	{
		return StaticData::ShouldLogToFile;
	}

	ELogToFileMode LogToFile_Mode()
	{
		return StaticData::LogToFile_Mode;
	}

	bool ShouldDump_EngineStateJson_OnCrash()
	{
		return StaticData::ShouldDump_EngineStateJson_OnCrash;
	}
}