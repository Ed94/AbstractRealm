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

		void ThreadManager::DecommissionThread(WordSize _handle)
		{
			threads.at(_handle - 1).join();
		}

		WordSize ThreadManager::GetNumOfActiveThreads()
		{
			WordSize num = 0;

			for (; num < threads.size(); num++)
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

	WordSize GetNumOfActiveThreads() { return Backend::ThreadPool.GetNumOfActiveThreads(); }

	void DecommissionThread(WordSize _handle) 
	{
		Backend::ThreadPool.DecommissionThread(_handle); 
	}

	void QueryThreadInfo()
	{
		/*
		STL way of getting number of logical cores.

		Intended use is to find out the number of maximum concurrent threads supported.
		*/
		NumberOfLogicalCores = Thread::hardware_concurrency(); 

		CLog(String("Number of Logical Cores: " + std::to_string(NumberOfLogicalCores)));
	}

	uint32 GetNumberOfLogicalCores()
	{
		return NumberOfLogicalCores;
	}
}