/*!
 

@brief Low level memory tracking on the heap. Should not be enabled unless doing memory leak management.
*/


#pragma once


#include "Meta/Meta.hpp"
#undef Heap   // This will be redefined.


namespace Memory
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
		

		static void ReportAllocation  (                    String _identifier, EModule _module);
		static void ReportAllocation  (ptr<void> _address, String _identifier, EModule _module);
		static void ReportDeallocation(                    String _identifier);
		static void ReportDeallocation(ptr<void> _address                    );

		static void PrintAllocations();
	};
}
