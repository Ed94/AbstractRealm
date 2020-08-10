/*
Execute

Last Modified: 5/18/2020

Currently the only defined main in the engine. You can manually define what procedure to execute here...
*/


#include "LAL/LAL.hpp"



#include "Executer.hpp"



int main()
{
	using namespace LAL;

	cout << "EntryPoint: Starting the Core Execution Module" << endl;

	Core::Execution::InitiateExecution();

	return EXIT_SUCCESS;
}
