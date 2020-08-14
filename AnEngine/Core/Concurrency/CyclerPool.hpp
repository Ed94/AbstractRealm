#pragma once



#include "Cycler.hpp"



namespace Core::Concurrency
{
	using namespace Execution;

	struct CyclerPool
	{
		static const Cycler& GetCycler(uInt16 _unit);

		static uInt16 GetNumUnits();

		static void Initialize();

		static bool IsShutdown();

		static bool RequestShutdown();

		static void ActivateUnit();
	};
}
