// Parent Header
#include "Executer.hpp"


#include "Meta/EngineInfo.hpp"
#include "Dev/Dev.hpp"


namespace Core::Execution
{
	using namespace Meta;

	int InitiateExecution()
	{
		if (UseDebug)
		{
			cout << "Core:Execution: Initiate Execution" << endl;

			cout << std::setfill('-') << std::setw(140); cout << " " << endl;

			cout << "Abstract Realm: MVP Build - "
			<< EEngineVersion::Major << "."
			<< EEngineVersion::Minor << "."
			<< EEngineVersion::Patch << endl;

			cout << std::setfill('-') << std::setw(140); cout << " " << endl << endl;

			cout << "Initializing Dev Module" << endl;

			Dev::LoadModule();
		}

		// Do the rest of initiation...
		
		return EXIT_SUCCESS;
	}
}