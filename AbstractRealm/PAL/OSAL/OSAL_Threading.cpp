// Parent Header
#include "OSAL_Threading.hpp"



namespace OSAL
{
	StaticData()
		ui32 NumberOfLogicalCores;
		

	
	namespace Backend
	{
		namespace StaticData { ThreadManager ThreadPool; };

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

		ThreadManager& ThreadPool() { return StaticData::ThreadPool; }
	}



	void GenerateThreads() { Backend::ThreadPool().GenerateThreads(); }

	WordSize GetNumOfActiveThreads() { return Backend::StaticData::ThreadPool.GetNumOfActiveThreads(); }

	void DecommissionThread(WordSize _handle) 
	{
		Backend::StaticData::ThreadPool.DecommissionThread(_handle); 
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

	ui32 GetNumberOfLogicalCores()
	{
		return NumberOfLogicalCores;
	}
}