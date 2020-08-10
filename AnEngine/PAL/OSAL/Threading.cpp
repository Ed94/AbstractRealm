// Parent Header
#include "Threading.hpp"





namespace OSAL
{
	uint32 NumberOfLogicalCores;  

	void QueryInfo()
	{
		/*
		STL way of getting number of logical cores.

		Intended use is to find out the number of maximum concurrent threads supported.
		*/
		NumberOfLogicalCores = Thread::hardware_concurrency(); 
	}

	uint32 GetNumberOfLogicalCores()
	{
		return NumberOfLogicalCores;
	}
}