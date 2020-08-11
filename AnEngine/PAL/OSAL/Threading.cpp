// Parent Header
#include "Threading.hpp"



namespace OSAL
{
	uint32 NumberOfLogicalCores;  

	void QueryThreadInfo()
	{
		OS_CLog("Querying thread info");

		/*
		STL way of getting number of logical cores.

		Intended use is to find out the number of maximum concurrent threads supported.
		*/
		NumberOfLogicalCores = Thread::hardware_concurrency(); 

		OS_CLog(String("Number of Logical Cores: " + std::to_string(NumberOfLogicalCores)));
	}

	uint32 GetNumberOfLogicalCores()
	{
		return NumberOfLogicalCores;
	}
}