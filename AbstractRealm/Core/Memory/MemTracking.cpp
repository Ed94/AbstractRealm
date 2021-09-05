// Parent Header
#include "MemTracking.hpp"


// Engine
#include "Core/Dev/Console.hpp"
#include "Meta/Config/CoreDev_Config.hpp"


namespace Memory
{
#pragma region StaticData
		Map<ptr<void>, Heap::Allocation> AllocationsReported;

		Map<String, Heap::Allocation> AllocationsReported_NoAddress;
#pragma endregion StaticData


	// Heap

	// Public

	void Heap::ReportAllocation(String _identifier, EModule _module)
	{
		if (Meta::Enable_HeapTracking())
		{
			Allocation newAllocation(_identifier, _module);

			AllocationsReported_NoAddress.emplace(_identifier, newAllocation);
		}
	}

	void Heap::ReportAllocation(ptr<void> _address, String _identifier, EModule _module)
	{
		if (Meta::Enable_HeapTracking())
		{
			Allocation newAllocation(_identifier, _module);

			AllocationsReported.emplace(_address, newAllocation);
		}
	}

	void Heap::ReportDeallocation(String _identifier)
	{
		if (Meta::Enable_HeapTracking())
		{
			AllocationsReported_NoAddress.erase(_identifier);
		}
	}

	void Heap::ReportDeallocation(ptr<void> _address)
	{
		if (Meta::Enable_HeapTracking())
		{
			AllocationsReported.erase(_address);
		}
	}

	void Heap::PrintAllocations()
	{
		if (Meta::Enable_HeapTracking())
		{
			if (!AllocationsReported.empty())
			{
				Dev::CLog("Heap: Allocations Reported (Not Deallocated)");

				for (auto entry = AllocationsReported.begin(); entry != AllocationsReported.end(); entry++)
				{
					StringStream addressString;

					addressString << entry->first;

					Dev::CLog("Identifier: " + entry->second.Identifier                 );
					Dev::CLog("Address   : " + addressString.str()                      );
					Dev::CLog("Module    : " + String(nameOf(entry->second.Module  )));
				}
			}

			if (!AllocationsReported_NoAddress.empty())
			{
				Dev::CLog("Heap: Allocations Reported [No Address] (Not Deallocated)");

				for (auto entry = AllocationsReported_NoAddress.begin(); entry != AllocationsReported_NoAddress.end(); entry++)
				{
					Dev::CLog("Identifier: " + entry->first                             );
					Dev::CLog("Module    : " + String(nameOf(entry->second.Module))  );
				}
			}
		}
	}
}
