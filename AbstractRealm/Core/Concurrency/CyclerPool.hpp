#pragma once

// Engine
#include "LAL.hpp"
#include "Cycler.hpp"


namespace Concurrency
{
#pragma region Usings

	using namespace LAL;
	using namespace Execution;

#pragma endregion Usings


#pragma region Structs

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

#pragma endregion Structs
}
