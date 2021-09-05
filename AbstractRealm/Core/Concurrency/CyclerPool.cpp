// Parent Header
#include "CyclerPool.hpp"


#include "OSAL.hpp"


namespace Concurrency
{
	using Cycler = Execution::Cycler;

	// Structs

	struct Unit
	{
		using CyclerT = Execution::Cycler;

		Unit() : Cycler(), Thread(NULL)
		{}

		CyclerT  Cycler;
		uDM Thread;
	};


	// Static Data

	DynamicArray<Unit> Pool;

	bool Initiated   = false;
	u16  ActiveUnits = 0    ;


	// Public

	void CyclerPool::ActivateUnit()
	{
		Pool[ActiveUnits].Thread = OSAL::RequestThread(&Cycler::Initiate, &Pool[ActiveUnits].Cycler);

		ActiveUnits++;
	}

	const Cycler& CyclerPool::GetCycler(u16 _unit)
	{
		return Pool[_unit].Cycler;
	}

	u16 CyclerPool::GetNumUnits()
	{
		return ActiveUnits;
	}

	void CyclerPool::Initialize()
	{
		Pool.resize(OSAL::GetNumberOfLogicalCores());

		Initiated = true;
	}

	bool CyclerPool::IsShutdown()
	{
		return
		Initiated == true ?
			ActiveUnits > 0 ? 
				false : true : 
			true
		;
	}

	bool CyclerPool::RequestShutdown()
	{		
		for (u16 unitIndex = 0; unitIndex < ActiveUnits; unitIndex++)
		{
			Pool[unitIndex].Cycler.Lapse();

			while(Pool[unitIndex].Cycler.Lapsed())
			{}

			OSAL::DecommissionThread(Pool[unitIndex].Thread);
		}

		ActiveUnits = 0;

		return true;
	}
}