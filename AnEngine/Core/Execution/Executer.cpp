// Parent Header
#include "Executer.hpp"


#include "Meta/EngineInfo.hpp"
#include "Dev/Dev.hpp"
#include "OSAL/OSAL.hpp"
#include "HAL/GPU_HAL.hpp"


namespace Core::Execution
{
	using namespace Meta;

	int InitiateExecution()
	{
		if (UseDebug)
		{
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

		std::chrono::duration<float64> delta;

		while (true)
		{
			auto begin = SteadyClock::now();

			Dev::CLog("Console Log Rendering Delta: " + ToString(delta.count()));

			auto end = SteadyClock::now();

			delta = std::chrono::duration_cast<decltype(delta)>(end - begin);
		}

		return EXIT_SUCCESS;
	}
}