// Parent Header
#include "CyclerPool.hpp"



namespace Core::Concurrency
{
	// Structs

	struct Unit
	{
		using CyclerT = Cycler;

		Unit() : Cycler(), Thread(NULL)
		{}

		CyclerT  Cycler;
		WordSize Thread;
	};



	StaticData()

		DynamicArray<Unit> Pool;

		bool Initiated   = false;
		uI16 ActiveUnits = 0    ;



	// Public

	void CyclerPool::ActivateUnit()
	{
		Pool[ActiveUnits].Thread = OSAL::RequestThread(&Cycler::Initiate, &Pool[ActiveUnits].Cycler);

		ActiveUnits++;
	}

	const Cycler& CyclerPool::GetCycler(uI16 _unit)
	{
		return Pool[_unit].Cycler;
	}

	uI16 CyclerPool::GetNumUnits()
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
		Initiated = true ?
			ActiveUnits > 0 ? false : true : 
		true;
	}

	bool CyclerPool::RequestShutdown()
	{		
		for (uI16 unitIndex = 0; unitIndex < ActiveUnits; unitIndex++)
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