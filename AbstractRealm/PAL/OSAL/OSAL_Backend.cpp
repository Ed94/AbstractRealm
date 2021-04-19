// Parent Header
#include "OSAL_Backend.hpp"



#include "Dev/Dev.hpp"
#include "Dev/Log.hpp"



namespace OSAL
{
	using namespace LAL;

	Dev::Log log;

	void Load_Backend()
	{
		log.Init("OSAL");
	}

	void Log(String _lineLog)
	{
		Dev::CLog("OSAL: " + _lineLog);

		//Dev::Console_UpdateBuffer();

		log.Record(Dev::Severity::Info, _lineLog);
	}
}