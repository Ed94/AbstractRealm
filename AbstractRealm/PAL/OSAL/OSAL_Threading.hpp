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

			uDM GetNumOfActiveThreads();

			u32 GetNumberOfLogicalCores();

			void QueryThreadInfo();

			template<class FN_Type, class... Arguments>
			uDM RequestThread(Function<FN_Type>&& _threadRoutine, Arguments&&... _args);

			void DecommissionThread(uDM _handle);


			// TODO: Move to tpp

			/**
			* Number of threads that are active should be one less than the
			* max number of logical cores.
			* 
			* \param _threadRoutine
			* \return 
			*/
			template<class FN_Type, class... Arguments>
			uDM RequestThread(FN_Type&& _threadRoutine, Arguments&&... _args)
			{
				uDM index = 0;

				for (uDM num = 0; num < threads.size(); num++)
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


		ThreadManager& ThreadPool();
	}

	void DecommissionThread(uDM _handle);

	u32 GetNumberOfLogicalCores();

	void GenerateThreads();

	uDM GetNumOfActiveThreads();

	void QueryThreadInfo();

	template<class FN_Type, class... Arguments>
	uDM RequestThread(FN_Type&& _threadRoutine, Arguments&&... _args)
	{
		return Backend::ThreadPool().RequestThread(_threadRoutine, _args...);
	}
}
