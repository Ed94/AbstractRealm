// Parent
#include "Dev_Backend.hpp"


#include "Dev/Log.hpp"



namespace Dev
{
	Dev::Logger SubLogger("Dev");

	void Log(String _message, Dev::ESeverity _severity)
	{
		SubLogger.Record(_severity, _message);
	}
	
	void Log_Verbose(String _message)
	{
		SubLogger.Record(Dev::ESeverity::Verbose, _message);
	}
	
	void Log_Warning(String _message)
	{
		SubLogger.Record(Dev::ESeverity::Warning, _message);
	}
	
	void Log_Error(String _message)
	{
		SubLogger.Record(Dev::ESeverity::Error, _message);
	} 

	void LoadBackend()
	{
		SubLogger.Init();
	}
}