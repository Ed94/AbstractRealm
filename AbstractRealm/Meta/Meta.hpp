/*
File: Meta.hpp
Module: Meta

This is the main header for the meta module, and also acts as a "Meta" header for other major modules of the engine source.
*/


#pragma once


// Platform related headers should always come first to prevent redef issues with macros.
// This is the only file that so far has this exception.
#include "OSAL/OSAL_Platform.hpp"
// All modules of the engine with exception to LAL, use the LAL.
#include "LAL/LAL.hpp"
// All modules of the engine, EXCEPT the Dev module of Core, use the Log module using this include.
#include "Dev/Log.hpp"
// All modules of the engien use the Core's exception module.
// However not all modules include the exception module directly
#include "Exceptions/Exceptions.hpp"


#define Meta_SetupEngineModule(__MODULE) \
namespace __MODULE\
{\
	using namespace LAL;\
\
	Dev_Declare_LogAPI();\
}

#define Meta_SetupEngineModule_Implementation(__MODULE)\
namespace __MODULE\
{\
\
	Dev_Declare_Log(__MODULE);\
}

#define Meta_EngineModule_Init() \
SubLogger.Init();

// Not used yet.
//#define Meta_SetupSubmodule(_Module, _Submodule) \
//namespace _Submodule\
//{\
//	using namespace LAL;\
//\
//	Dev_Declare_SubLog(_Module, _Submodule);\
//}


// Meta Module Headers

// Note: Config files ommited on purpose

#include "AppInfo.hpp"
#include "EngineInfo.hpp"
#include "Specification.hpp"
