// Parent Header
#include "Dev.hpp"



// Engine
#include "LAL.hpp"
#include "Console.hpp"



namespace Dev
{
	// Usings

	using namespace LAL;



	// Public

	void LoadModule()
	{
		cout << "Dev: Load Module" << endl;

		Load_DevConsole();

		CLog("Dev: Module loaded");
	}
}