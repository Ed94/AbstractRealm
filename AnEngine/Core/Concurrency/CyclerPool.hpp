#pragma once



// Engine
#include "Cycler.hpp"



namespace Core::Concurrency
{
	using namespace Execution;

	struct CyclerPool
	{
		static void ActivateUnit();

		static const Cycler& GetCycler(uInt16 _unit);

		static uInt16 GetNumUnits();

		static void Initialize();

		static bool IsShutdown();

		static bool RequestShutdown();
	};
}
