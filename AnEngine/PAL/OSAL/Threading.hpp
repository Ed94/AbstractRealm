#pragma once



#include "OSAL_Backend.hpp"



namespace OSAL
{
	namespace Backend
	{
		struct ThreadManager
		{
		public:

			void QueryThreadInfo();

			uint32 GetNumberOfLogicalCores();

			void GenerateThreads();

			DataSize GetNumOfActiveThreads();

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
				/*if (Threads.size() < GetNumberOfLogicalCores())
				{
				Threads.push_back(std::move(Thread(_threadRoutine, _args...)));

				return Threads.size();
				}*/

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


		eGlobal ThreadManager Threads;
	}

	void QueryThreadInfo();

	uint32 GetNumberOfLogicalCores();

	void GenerateThreads();

	DataSize GetNumOfActiveThreads();

	void DecommissionThread(DataSize _handle);

	template<class FN_Type, class... Arguments>
	DataSize RequestThread(FN_Type&& _threadRoutine, Arguments&&... _args)
	{
		return Backend::Threads.RequestThread(_threadRoutine, _args...);
	}
}
