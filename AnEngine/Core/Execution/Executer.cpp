// Parent Header
#include "Executer.hpp"



namespace Core::Execution
{
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