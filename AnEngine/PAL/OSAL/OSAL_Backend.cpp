// Parent Header
#include "OSAL_Backend.hpp"



#include "Dev/Dev.hpp"



namespace OSAL
{
	void OS_CLog(String _lineLog)
	{
		Dev::CLog("OSAL: " + _lineLog);

		Dev::Console_UpdateBuffer();
	}
}