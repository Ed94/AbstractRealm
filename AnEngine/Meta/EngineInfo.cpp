// Parent Header
#include "EngineInfo.hpp"



namespace Meta
{
	StaticData
	(
		data< EOperationMode> OperationalMode = Default_OperationalMode();

		data< bool> UseEditor    = OperationalMode == EOperationMode::Development;
		data< bool> UseDebug     = OperationalMode <  EOperationMode::Profiling  ;
		data< bool> UseProfiling = OperationalMode <  EOperationMode::User       ;

		data< bool> UseConcurrency = false;
	)
}