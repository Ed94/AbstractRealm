// Parent Header
#include "OSAL_Threading.hpp"



namespace OSAL
{
	uint32 NumberOfLogicalCores;   

	
	namespace Backend
	{
		void ThreadManager::GenerateThreads()
		{
			threads.resize(NumberOfLogicalCores - 1);
		}

		void ThreadManager::DecommissionThread(DataSize _handle)
		{
			threads.at(_handle - 1).join();
		}

		DataSize ThreadManager::GetNumOfActiveThreads()
		{
			DataSize num = 0;

			for (DataSize num = 0; num < threads.size(); num++)
			{
				if (threads[num].joinable())
				{
					num++;
				}
			}

			return num;
		}

		ThreadManager ThreadPool;
	}

	void GenerateThreads() { Backend::ThreadPool.GenerateThreads(); }

	DataSize GetNumOfActiveThreads() { return Backend::ThreadPool.GetNumOfActiveThreads(); }

	void DecommissionThread(DataSize _handle) { Backend::ThreadPool.DecommissionThread(_handle); }

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