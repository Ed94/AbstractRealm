// Parent Header
#include "EngineInfo.hpp"



namespace Meta
{
	StaticData()

		EOperationMode OperationalMode = Default_OperationalMode();

		bool UseEditor    = OperationalMode == EOperationMode::Development;
		bool UseDebug     = OperationalMode <  EOperationMode::Profiling  ;
		bool UseProfiling = OperationalMode <  EOperationMode::User       ;

		bool UseConcurrency = false;

		bool FixRenderRateToRefreshRate = false;
}