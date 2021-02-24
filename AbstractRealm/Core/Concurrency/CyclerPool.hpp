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

		static const Cycler& GetCycler(u16 _unit);

		static u16 GetNumUnits();

		static void Initialize();

		static bool IsShutdown();

		static bool RequestShutdown();
	};
}
