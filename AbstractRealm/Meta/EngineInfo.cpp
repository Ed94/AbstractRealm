// Parent Header
#include "EngineInfo.hpp"



namespace Meta
{
	namespace StaticData
	{
		EOperationMode OperationalMode = Default_OperationalMode();

		bool UseEditor    = OperationalMode == EOperationMode::Development;
		bool UseDebug     = OperationalMode < EOperationMode::Profiling;
		bool UseProfiling = OperationalMode < EOperationMode::User;

		bool UseConcurrency = false;

		bool FixRenderRateToRefreshRate = false;
	}



	// Public

	EOperationMode OperationalMode()
	{
		return StaticData::OperationalMode;
	}

	bool UseEditor()
	{
		return StaticData::UseEditor;
	}

	bool UseDebug()
	{
		return StaticData::UseDebug;
	}

	bool UseProfiling()
	{
		return StaticData::UseProfiling;
	}

	bool UseConcurrency()
	{
		return StaticData::UseConcurrency;
	}

	bool FixRenderRateToRefreshRate()
	{
		return StaticData::FixRenderRateToRefreshRate;
	}
}