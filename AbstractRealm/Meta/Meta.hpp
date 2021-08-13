/*
File: Meta.hpp
Module: Meta

Last Modified: 2021 April 19

This is the main header for the meta module, and also acts as a "Meta" header for other major modules of the engine source.
*/


#pragma once


// All modules of the engine with exception to LAL, use the LAL.
#include "LAL/LAL.hpp"
// All modules of the engine, including the Dev module of Core, use the LAL.
#include "Dev/Log.hpp"

// Meta
#include "EngineInfo.hpp"


namespace Meta
{
	// Functions

	void LoadModule();
}


#define Meta_SetupModule(_Module) \
namespace _Module\
{\
	using namespace LAL;\
\
	Dev_Declare_LogAPI();\
}

#define Meta_SetupModule_Implementation(_Module)\
namespace _Module\
{\
\
	Dev_Declare_Log(_Module);\
}

// Not used yet.
#define Meta_SetupSubmodule(_Module, _Submodule) \
namespace _Submodule\
{\
	using namespace LAL;\
\
	Dev_Declare_SubLog(_Module, _Submodule);\
}


Meta_SetupModule(Meta);
