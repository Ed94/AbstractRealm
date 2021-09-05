#pragma once


#include "Concurrency_Backend.hpp"
#include "Cycler.hpp"


namespace Concurrency
{
	using namespace Execution;


	class CyclerPool
	{
	public:

		static void ActivateUnit();

		static const Cycler& GetCycler(u16 _unit);

		static u16 GetNumUnits();

		static void Initialize();

		static bool IsShutdown();

		static bool RequestShutdown();
	};
}
