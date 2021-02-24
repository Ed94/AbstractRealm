// Parent Header
#include "OSAL_Threading.hpp"



namespace OSAL
{
	StaticData()
		u32 NumberOfLogicalCores;
		

	
	namespace Backend
	{
		namespace StaticData { ThreadManager ThreadPool; };

		void ThreadManager::GenerateThreads()
		{
			threads.resize(NumberOfLogicalCores - 1);
		}

		void ThreadManager::DecommissionThread(uDM _handle)
		{
			threads.at(_handle - 1).join();
		}

		uDM ThreadManager::GetNumOfActiveThreads()
		{
			uDM num = 0;

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

	uDM GetNumOfActiveThreads() { return Backend::StaticData::ThreadPool.GetNumOfActiveThreads(); }

	void DecommissionThread(uDM _handle) 
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

	u32 GetNumberOfLogicalCores()
	{
		return NumberOfLogicalCores;
	}
}