/*
Operating System Abstraction Layer
*/



#pragma once



// Includes

#include "LAL/LAL.hpp"

#include "OSAL/Platform.hpp"
#include "OSAL/Windowing.hpp"



namespace OSAL
{
	/*
	Loads the OSAL submodule.

	Note: This prepares the submodule for use of its related functionality supported.
	*/
	void Load();


	void Unload();


	/*
	Currently based of GLFW events (Since there are no other OS related events yet).
	*/
	void PollEvents();
}
