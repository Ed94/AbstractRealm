#pragma once


// Have to manually setup meta for dev.

// Platform related headers should always come first to prevent redef issues with macros.
// This is the only file that so far has this exception.
#include "OSAL/OSAL_Platform.hpp"
// All modules of the engine with exception to LAL, use the LAL.
#include "LAL/LAL.hpp"
// All modules of the engine, EXCEPT the Dev module of Core, use the Log module using this include.
//#include "Dev/Log.hpp"
// All modules of the engien use the Core's exception module.
// However not all modules include the main module directly
//#include "Exceptions/Exceptions.hpp"

// Meta Module Headers

// Note: Config files ommited on purpose

#include "Meta/AppInfo.hpp"
#include "Meta/EngineInfo.hpp"
#include "Meta/Specification.hpp"

// End of Manual Meta Setup


namespace Dev
{
	using namespace LAL;

	enum class ESeverity
	{
		Verbose,
		Info,
		Warning,
		Error
	};

	void Log        (String _message, Dev::ESeverity _severity =  Dev::ESeverity::Info);
	void Log_Verbose(String _message);
	void Log_Warning(String _message);
	void Log_Error  (String _message);

	void LoadBackend();
}
