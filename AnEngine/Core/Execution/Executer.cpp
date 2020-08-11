// Parent Header
#include "Executer.hpp"



#include "Cycler.hpp"




namespace Core::Execution
{
	bool exist = false;

	// Quick and dirty setup (Until proper is fleshed out...

	PrimitiveExecuter<void()> Engine_DirtyExecute1;
	PrimitiveExecuter<void()> Engine_DirtyExecute2;

	Cycler Cycler_Dirty1;
	Cycler Cycler_Dirty2;

	void Dirty_Execute1()
	{
		//Dev::CLog("Thread Delta: " + ToString(Cycler_Dirty.GetDeltaTime().count()));
	}

	void Dirty_Execute2()
	{
		//Dev::CLog("Thread Delta: " + ToString(Cycler_Dirty.GetDeltaTime().count()));
	}

	OSAL::ExitValT EntryPoint()
	{
		OSAL::Record_EntryPoint_StartExecution();

		if (UseDebug)
		{
			using namespace LAL;

			if (!AllocConsole()) return 0;

			Dev::SetConsole_IOHooks();

			cout << "EntryPoint: Starting the Core Execution Module" << endl;

			cout << "Core:Execution: Initiate Execution" << endl;

			cout << setfill('-') << setw(140); cout << " " << endl;

			cout << "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << endl;

			cout << setfill('-') << setw(140); cout << " " << endl << endl;

			cout << "Initializing Dev Module" << endl;

			Dev::LoadModule();
		}

		OSAL::Load();

		HAL::GPU::Load();

		Engine_DirtyExecute1.Bind(Function<void()>(Dirty_Execute1));
		Cycler_Dirty1.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute1));

		Engine_DirtyExecute2.Bind(Function<void()>(Dirty_Execute2));
		Cycler_Dirty2.BindExecuter(ptr<AExecuter>(&Engine_DirtyExecute2));

		auto Thread_1 = OSAL::RequestThread(&Cycler::Initiate,	std::ref(Cycler_Dirty1));
		auto Thread_2 = OSAL::RequestThread(&Cycler::Initiate,	std::ref(Cycler_Dirty2));

		exist = true;

		Dev::CLog_ToggleWriteToConsole();

		int mainCycle = 0;

		while (exist)
		{
			//Cycler_Dirty.Initiate();

			//Sleep(33);

			Dev::CLog_Status("Thread 1 DeltaAvg: " + ToString(Cycler_Dirty1.GetDeltaTime().count()), 0);
			Dev::CLog_Status("Thread 2 DeltaAvg: " + ToString(Cycler_Dirty2.GetDeltaTime().count()), 1);

			Dev::CLog_Status("MainCycle: " + ToString(mainCycle), 2);

			Dev::Console_UpdateBuffer();

			//Cycler_Dirty.Lapse();

			if (mainCycle == 50000)
			{
				Cycler_Dirty1.Lapse();
				Cycler_Dirty2.Lapse();
			}
			else 
			{
				mainCycle++;
			}

			if (Cycler_Dirty1.Lapsed() && Cycler_Dirty2.Lapsed())
			{
				OSAL::DecommissionThread(Thread_1);
				OSAL::DecommissionThread(Thread_2);

				exist = false;
			}
		}

		/*std::chrono::duration<float64> delta;

		while (true)
		{
			auto begin = SteadyClock::now();

			Dev::CLog("Console Log Rendering Delta: " + ToString(delta.count()));

			auto end = SteadyClock::now();

			delta = std::chrono::duration_cast<decltype(delta)>(end - begin);
		}*/

		if (UseDebug)
		{
			FreeConsole();
		}

		return EXIT_SUCCESS;
	}
}