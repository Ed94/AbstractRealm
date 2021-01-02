/*!
 

@brief Low level memory tracking on the heap. Should not be enabled unless doing memory leak management.
*/


#pragma once



#include "LAL.hpp"
#undef Heap   // This will be redefined.
#include "Meta/EngineInfo.hpp"



namespace Core::Memory
{
	using namespace LAL;
	using namespace Meta;


	// Helps with heap tracking. Should only be used in places where implementation is a scrapyard.
	class Heap
	{
	public:
		struct Allocation
		{
			Allocation(String _identifier, EModule _module) :
				Identifier(_identifier), Module(_module)
			{}

			String Identifier;

			EModule Module;
		};
		

		unbound void ReportAllocation  (                    String _identifier, EModule _module);
		unbound void ReportAllocation  (ptr<void> _address, String _identifier, EModule _module);
		unbound void ReportDeallocation(                    String _identifier);
		unbound void ReportDeallocation(ptr<void> _address                    );

		unbound void PrintAllocations();
	};
}
