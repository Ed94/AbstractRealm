#pragma once



#include "OSAL_Backend.hpp"



namespace OSAL
{
	namespace Backend
	{
		//struct Thread
		//{
		//	DataSize Handle;

		//	//uint16 Core;
		//};

		struct ThreadManager
		{
		public:

			void GenerateThreads();

			DataSize GetNumOfActiveThreads();

			uint32 GetNumberOfLogicalCores();

			void QueryThreadInfo();

			template<class FN_Type, class... Arguments>
			DataSize RequestThread(Function<FN_Type>&& _threadRoutine, Arguments&&... _args);

			void DecommissionThread(DataSize _handle);


			// TODO: Move to tpp

			/**
			* Number of threads that are active should be one less than the
			* max number of logical cores.
			* 
			* \param _threadRoutine
			* \return 
			*/
			template<class FN_Type, class... Arguments>
			DataSize RequestThread(FN_Type&& _threadRoutine, Arguments&&... _args)
			{
				DataSize index = 0;

				for (DataSize num = 0; num < threads.size(); num++)
				{
					if (!threads[index].joinable())
					{
						threads[index] = Thread(_threadRoutine, _args...);

						return index + 1;
					}

					index++;
				}

				return NULL;
			}

		private:
			DynamicArray<Thread> threads;
		};


		eGlobal ThreadManager ThreadPool;
	}

	void DecommissionThread(DataSize _handle);

	uint32 GetNumberOfLogicalCores();

	void GenerateThreads();

	DataSize GetNumOfActiveThreads();

	void QueryThreadInfo();

	template<class FN_Type, class... Arguments>
	DataSize RequestThread(FN_Type&& _threadRoutine, Arguments&&... _args)
	{
		return Backend::ThreadPool.RequestThread(_threadRoutine, _args...);
	}
}
