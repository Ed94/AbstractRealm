// Parent Header
#include "LAL_Memory.hpp"



namespace LAL
{
#ifdef LAL_TrackHeap

	DataSize AllocationCount = 0;

	DataSize Internal_Heap_AllocationsLeft()
	{
		return AllocationCount;
	}

	void ProcessHeapAction(EHeap _action)
	{
		switch (_action)
		{
			case EHeap::Allocate:
			{
				AllocationCount++;
			}
			case EHeap::Free:
			{
				AllocationCount--;
			}
		}
	}

#endif
}