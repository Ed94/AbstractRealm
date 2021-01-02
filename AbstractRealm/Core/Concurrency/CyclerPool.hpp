#pragma once



// Engine
#include "Cycler.hpp"



namespace Core::Concurrency
{
	// Usings

	using namespace Execution;



	// Structs

	struct CyclerPool
	{
		static void ActivateUnit();

		static const Cycler& GetCycler(uI16 _unit);

		static uI16 GetNumUnits();

		static void Initialize();

		static bool IsShutdown();

		static bool RequestShutdown();
	};
}
