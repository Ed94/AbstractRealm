// Parent Header
#include "Dev.hpp"



// Engine
#include "LAL.hpp"
#include "Console.hpp"



namespace Dev
{
	using namespace LAL;



	void LoadModule()
	{
		cout << "Dev: Load Module" << endl;

		Load_DevConsole();
	}
}